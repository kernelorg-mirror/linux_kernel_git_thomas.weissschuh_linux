#!/bin/bash

vng -v --user root --exec 'echo 1 >/sys/kernel/time/aux_clocks/0/aux_clock_enable; init/test' --append 'tp_printk trace_event=timekeeping:timekeeping_auxclock_deviation:deviation>=25||deviation<=-25'
