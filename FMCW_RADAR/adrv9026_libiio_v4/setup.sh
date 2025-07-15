#!/bin/bash
echo 1 > /sys/kernel/debug/iio/iio:device4/pl_ddr_fifo_enable && 
echo 536870912 > /sys/module/industrialio_buffer_dma/parameters/max_block_size

