/* hello.c − Demonstrating the simple kernel module. This is the
 * demonstration code with command line arg in kernel module
 * Copyright (C) 2018 by Pawan Kumar
 * Kernel Programming */


/******************************************
 *               includes                 *
 ******************************************/
#include <linux/module.h>	//required for all
#include <linux/kernel.h>	//for kernel loglevels like KERN_INFO
#include <linux/init.h>		//nedded if we want to use user defined name for init and exit


/*******************************************
 *                modinfo                  *
 *******************************************/
#define MOD_AUTHOR "Pawan Kumar <jmppawanhit@gmail.com>"
#define MOD_DESC "Hello world module"
#define MOD_SUPPORT "no device support"

/* This is the documentation part, after doing insmod of the module 
 * you can find details about this module by using "modinfo hello.ko" */

MODULE_LICENSE("Dual BSD/GPL");		    // for getting rid of kernel taint warning
MODULE_AUTHOR(MOD_AUTHOR);		        // name of the module author
MODULE_DESCRIPTION(MOD_DESC);		    // a short description about module
MODULE_VERSION("0.0.1");                // A version number to inform users
MODULE_SUPPORTED_DEVICE(MOD_SUPPORT);	// devices this module uses


/*******************************************
 *                 global                  *
 *******************************************/
static int	debug = 4;			            // This global variable can be set with command line arguments when you insmod


/********************************************
 *                prototypes                *
 ********************************************/
static int hello_init(void);
static void hello_exit(void);


/********************************************
 *       command line variable setting      *
 ********************************************/
module_param(debug, int, 0);			                                                    //this function actually sets the variable ( this is defined in module.h)
MODULE_PARM_DESC(debug, "this variable can be used to get logs at different debug level");	//description of param (defined in module.h)


static int __init hello_init(void)
{
	printk(KERN_INFO "%d %s:\tHello World debug=%d\n",__LINE__,__func__,debug);		        //find this message when you do insmod in /var/log/kern.log
	if (debug > 4)
	{
		printk("%d %s:\tcritical problem while initialization\n",__LINE__,__func__);
	}

	return 0;					                                                            //indicates failure if not zero
}


static void __exit hello_exit(void)
{
	printk(KERN_INFO "%d %s:\tGood Bye World\n",__LINE__,__func__);
}

/* this will call user defined initialization function, 
 * when module insmoded and default syntax for module initialization*/
module_init(hello_init);

/* this will call user defined exit function when module id rmmodded
 * this module is nott mandatory for */
module_exit(hello_exit);
