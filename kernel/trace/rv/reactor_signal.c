// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Thomas Weißschuh, Linutronix GmbH
 *
 * Signal RV reactor:
 *   Prints the exception msg to the kernel message log and sends a signal to the offending task.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpumask.h>
#include <linux/init.h>
#include <linux/mempool.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/rv.h>
#include <linux/sched/signal.h>
#include <linux/task_work.h>

struct rv_signal_work {
	struct callback_head twork;
	int signal;
	char message[256];
};

static mempool_t *rv_signal_task_work_pool;

static void rv_signal_force_sig(int signal, const char *message)
{
	/* The message already contains a subsystem prefix, so use raw printk() */
	printk(KERN_WARNING "%s", message);
	pr_warn("Killing PID %d with signal %d", task_pid_nr(current), signal);
	force_sig(signal);
}

static void rv_signal_task_work(struct callback_head *cbh)
{
	struct rv_signal_work *work = container_of_const(cbh, struct rv_signal_work, twork);

	rv_signal_force_sig(work->signal, work->message);

	mempool_free(work, rv_signal_task_work_pool);
}

static void rv_reaction_signal(int signal, const char *fmt, va_list args)
{
	struct rv_signal_work *work;
	char message[256];

	work = mempool_alloc_preallocated(rv_signal_task_work_pool);
	if (!work) {
		pr_warn_ratelimited("Unable to signal through task_work, sending directly\n");
		vsnprintf(message, sizeof(message), fmt, args);
		rv_signal_force_sig(signal, message);
		return;
	}

	init_task_work(&work->twork, rv_signal_task_work);
	work->signal = signal;
	vsnprintf(work->message, sizeof(work->message), fmt, args);

	/*
	 * The reactor can be called from any context through tracepoints.
	 * To avoid any locking or other operations not usable from all contexts, use TWA_RESUME.
	 * The signal might be delayed, but that shouldn't be an issue.
	 */
	task_work_add(current, &work->twork, TWA_RESUME);
}

__printf(1, 0)
static void rv_reaction_sigbus(const char *fmt, va_list args)
{
	rv_reaction_signal(SIGBUS, fmt, args);
}

static struct rv_reactor rv_sigbus = {
	.name		= "sigbus",
	.description	= "Kill the current task with SIGBUS",
	.react		= rv_reaction_sigbus,
};

static int __init register_react_signal(void)
{
	int ret;

	rv_signal_task_work_pool = mempool_create_kmalloc_pool(num_possible_cpus(),
							       sizeof(struct rv_signal_work));
	if (!rv_signal_task_work_pool)
		return -ENOMEM;

	ret = rv_register_reactor(&rv_sigbus);
	if (ret) {
		mempool_destroy(rv_signal_task_work_pool);
		return ret;
	}

	return 0;
}

static void __exit unregister_react_signal(void)
{
	rv_unregister_reactor(&rv_sigbus);
	mempool_destroy(rv_signal_task_work_pool);
}

module_init(register_react_signal);
module_exit(unregister_react_signal);

MODULE_AUTHOR("Thomas Weißschuh <thomas.weissschuh@linutronix.de>");
MODULE_DESCRIPTION("signal rv reactor: send a signal if an exception is found.");
