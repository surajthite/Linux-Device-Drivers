#!/bin/bash

make clean

make 

sudo ./aesd_char_unload

sudo ./aesd_char_load 

sudo ./drivertest.sh

dmesg > ./123.txt