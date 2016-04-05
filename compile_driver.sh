#!/bin/bash
####################################
#
# This script compiles the kernel 
# module hello.ko
#
####################################

KERNEL_HEADER_PATH=/usr/src/linux/

make -C ${KERNEL_HEADER_PATH} M=`pwd` clean
make -C ${KERNEL_HEADER_PATH} M=`pwd` modules

