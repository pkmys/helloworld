/* hello.c − Demonstrating the simple kernel module. This is the
 *
 * bare minimum code for kernel module
 *
 * Copyright (C) 2018 by Pawan Kumar
 *
 * Kernel Programming */


/*Required Includes*/
#include <linux/module.h>	//required for all, and give utilities for documentation
#include <linux/kernel.h>	//for kernel loglevels like KERN_INFO
#include <linux/init.h>		//nedded if we want to use user defined name for init and exit
#define MOD_AUTHOR "Pawan Kumar <pkmys@github.com>"
#define MOD_DESC "Hello world driver"
#define MOD_SUPPORT "test device"

static int hello_init(void)
{
	printk(KERN_ALERT "Hello world!!!\n");		//find this message when you do insmod in /var/log/kern.log
	return 0;					//indicates failure if not zero
}
static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye world!!!\n");
}

/*this will call user defined initialization function, 
 *when module insmoded and default syntax for module initialization*/
module_init(hello_init);
/*this will call user defined exit function when module id rmmodded
 *this module is nott mandatory for */
module_exit(hello_exit);
/*This is the documentation part, after doing insmod of the module 
 *you can find details about this module by using "modinfo hello" */

MODULE_LICENSE("GPL");			// for getting rid of kernel taint warning
MODULE_AUTHOR(MOD_AUTHOR);		// name of the module author
MODULE_DESCRIPTION(MOD_DESC);		// a short description about module
MODULE_SUPPORTED_DEVICE(MOD_SUPPORT);	// devices this module uses
