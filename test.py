import pathlib
import shutil
import subprocess

import pytest

SOURCE_DIR = pathlib.Path('/home/t-8ch/src/linux-trees/module-hashes')

HAS_CONFIG_MODULE_HASHES = 'config MODULE_HASHES' in \
                           SOURCE_DIR.joinpath('kernel', 'module', 'Kconfig').read_text()

LOCKDOWN_FAILURE_MESSAGE = 'Lockdown: modprobe: unsigned module loading is restricted'
SIG_ENFORCE_FAILURE_MESSAGE = 'Loading of unsigned module is rejected'
SUCCESS_MESSAGE = 'Core section allocation order for efivarfs'


def _configure_kernel(cwd, configs):
    default_configs = [
            'CONFIG_MODULES=y',
            'CONFIG_EFIVAR_FS=m',
    ]

    all_configs = configs + default_configs

    subprocess.check_call([
        'vng', '--kconfig',
    ], cwd=cwd)

    with cwd.joinpath('.config').open('a') as f:
        for config in all_configs:
            f.write(f'{config}\n')

    subprocess.check_call([
        'make', 'oldconfig',
    ], cwd=cwd, stdin=subprocess.DEVNULL)

    final_config = cwd.joinpath('.config').read_text()
    for config in all_configs:
        assert config in final_config


def _build_kernel(cwd):
    subprocess.check_call([
        'vng', '--build',
    ], cwd=cwd)


def _run_kernel_and_load_module(cwd, installed_module):
    module_file = 'fs/efivarfs/efivarfs.ko'

    if installed_module:
        module_path = cwd / 'install-modules/lib/modules/7.1.0-rc1-virtme/kernel' / module_file
    else:
        module_path = cwd / module_file

    return subprocess.run([
        'vng', '--user', 'root', '-v',
        '--exec', f'modprobe {module_path}',
    ], cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout


def _install_modules(cwd):
    install_dir = cwd / 'install-modules'

    shutil.rmtree(install_dir, ignore_errors=True)
    subprocess.check_call([
        'make', '-j20', 'modules_install',
        f'INSTALL_MOD_PATH={install_dir}',
    ], cwd=cwd)


def _run_test(request, configs, module_from_build_dir_result, module_from_install_dir_result):
    if 'CONFIG_MODULE_HASHES=y' in configs and \
       'config MODULE_HASHES' not in SOURCE_DIR.joinpath('kernel', 'module', 'Kconfig').read_text():
        pytest.skip()

    cwd = SOURCE_DIR.joinpath('tests', request.node.name)
    cwd.mkdir(exist_ok=True, parents=True)
    cwd.joinpath('Makefile').write_text(f'''
export KBUILD_OUTPUT = {cwd}
include {SOURCE_DIR}/Makefile
    ''')

    _configure_kernel(cwd, configs)
    _build_kernel(cwd)
    _install_modules(cwd)

    output = _run_kernel_and_load_module(cwd, False)
    assert module_from_build_dir_result.encode('ascii') in output

    output = _run_kernel_and_load_module(cwd, True)
    assert module_from_install_dir_result.encode('ascii') in output


def test_unrestricted(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG is not set',
    ], SUCCESS_MESSAGE, SUCCESS_MESSAGE)


# No lockdown without MODULE_SIG
# def test_lockdown(request):
#     _run_test(request, [
#         'CONFIG_MODULE_SIG is not set',
#         'CONFIG_SECURITY=y',
#         'CONFIG_SECURITY_LOCKDOWN_LSM=y',
#         'CONFIG_LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY=y',
#     ], SUCCESS_MESSAGE, SUCCESS_MESSAGE)


def test_sig(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG=y',
    ], SUCCESS_MESSAGE, SUCCESS_MESSAGE)


def test_sig_force(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG=y',
        'CONFIG_MODULE_SIG_FORCE=y',
    ], SIG_ENFORCE_FAILURE_MESSAGE, SUCCESS_MESSAGE)


def test_sig_lockdown(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG=y',
        'CONFIG_SECURITY=y',
        'CONFIG_SECURITY_LOCKDOWN_LSM=y',
        'CONFIG_LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY=y',
    ], LOCKDOWN_FAILURE_MESSAGE, SUCCESS_MESSAGE)


def test_sig_lockdown_force(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG=y',
        'CONFIG_MODULE_SIG_FORCE=y',
        'CONFIG_SECURITY=y',
        'CONFIG_SECURITY_LOCKDOWN_LSM=y',
        'CONFIG_LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY=y',
    ], SIG_ENFORCE_FAILURE_MESSAGE, SUCCESS_MESSAGE)


def test_module_hashes(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG is not set',
        'CONFIG_MODULE_HASHES=y',
        'CONFIG_MODULE_SIG_FORCE=y',
        'CONFIG_SECURITY=y',
        'CONFIG_SECURITY_LOCKDOWN_LSM=y',
        'CONFIG_LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY=y',
    ], SIG_ENFORCE_FAILURE_MESSAGE, SUCCESS_MESSAGE)


def test_module_hashes_and_sig(request):
    _run_test(request, [
        'CONFIG_MODULE_SIG=y',
        '# CONFIG_MODULE_SIG_ALL is not set',
        'CONFIG_MODULE_HASHES=y',
        'CONFIG_MODULE_SIG_FORCE=y',
        'CONFIG_SECURITY=y',
        'CONFIG_SECURITY_LOCKDOWN_LSM=y',
        'CONFIG_LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY=y',
    ], SIG_ENFORCE_FAILURE_MESSAGE, SUCCESS_MESSAGE)
