# This is the make file for the hello world krnel module
# TARGET = name of the module
# PWD 	 = present working directory
# KERNDIR= kernel source directory

TARGET:= hello
PWD:= $(shell pwd)
KERNDIR:= "/lib/modules/$(shell uname -r)/build/"

obj-m:= ${TARGET}.o

all:
	make -C ${KERNDIR} M=${PWD}  modules

clean:
	make -C ${KERNDIR} M=${PWD}  clean
