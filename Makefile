# This is the make file for the hello world krnel module
# TARGET = name of the module
# PWD 	 = present working directory
# KERNDIR= kernel source directory

TARGET:= kernel-test-driver
PWD:= $(shell pwd)
EXTRA_CFLAGS:=-Wno-error
KERNDIR:= "/lib/modules/$(shell uname -r)/build/"
CC:=gcc

obj-m:= ${TARGET}.o

all:
	make -C ${KERNDIR} M=${PWD}  modules
	${CC} -o user-driver user-test-driver.c

clean:
	make -C ${KERNDIR} M=${PWD}  clean
	rm -f user-driver
