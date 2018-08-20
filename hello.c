/* hello.c − Demonstrating the simple kernel module. This is the
 *
 * demonstration code for command line arg in kernel module
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

/*-----------------GLOBAL-------------------*/
static int	debug = 4;			// This global variable can be set with command line arguments when you insmod

/*----------------PROTOTYPES----------------*/
static int hello_init(void);
static void hello_exit(void);

/*------- from command line argument -------*/
module_param(debug, int, 0);			//this function actually sets the variable (defined in module.h)
MODULE_PARM_DESC(debug, "this variable can be used to get logs at different debug level");	//description of param (defined in module.h)

static int hello_init(void)
{
	printk(KERN_INFO "%d %s:\tHello world!!! debug=%d\n",__LINE__,__func__,debug);		//find this message when you do insmod in /var/log/kern.log

	if (debug < 4)
	{
		printk("%d %s:\tcritical problem while initialization\n",__LINE__,__func__);
	}

	return 0;					//indicates failure if not zero
}

static void hello_exit(void)
{
	printk(KERN_INFO "%d %s:\tGoodbye world!!!\n",__LINE__,__func__);
}

/*this will call user defined initialization function, 
 *when module insmoded and default syntax for module initialization*/
module_init(hello_init);
/*this will call user defined exit function when module id rmmodded
 *this module is nott mandatory for */
module_exit(hello_exit);
/*This is the documentation part, after doing insmod of the module 
 *you can find details about this module by using "modinfo hello" */

MODULE_LICENSE("Dual BSD/GPL");			// for getting rid of kernel taint warning
MODULE_AUTHOR(MOD_AUTHOR);		// name of the module author
MODULE_DESCRIPTION(MOD_DESC);		// a short description about module
MODULE_SUPPORTED_DEVICE(MOD_SUPPORT);	// devices this module uses
