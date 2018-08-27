/* hello.c − Demonstrating the simple kernel module. This is the
 *
 * demonstration code for command line arg in kernel module
 *
 * Copyright (C) 2018 by Pawan Kumar
 *
 * Kernel Programming */


/*Required Includes*/
#include <linux/module.h>	//required for all
#include <linux/kernel.h>	//for kernel loglevels like KERN_INFO
#include <linux/init.h>		//nedded if we want to use user defined name for init and exit
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "testdev"    // The device will appear at /dev/testdev using this value
#define  CLASS_NAME  "test"        // The device class -- this is a character device driver

#define MOD_AUTHOR "Pawan Kumar <jmppawanhit@gmail.com>"
#define MOD_DESC "Hello world driver"
#define MOD_SUPPORT "test device"


/*-----------------GLOBAL-------------------*/
static int	debug = 4;			// This global variable can be set with command line arguments when you insmod
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  testdevClass  = NULL; ///< The device-driver class struct pointer
static struct device* testdevDevice = NULL; ///< The device-driver device struct pointer

/*---------------PROTOTYPES---------------*/
static int testdev_init(void);
static void testdev_exit(void);
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/*------- from command line argument -------*/
module_param(debug, int, 0);			//this function actually sets the variable (defined in module.h)
MODULE_PARM_DESC(debug, "this variable can be used to get logs at different debug level");	//description of param (defined in module.h)

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init testdev_init(void)
{
	printk(KERN_INFO "%d %s:\tInitializing test driver with debug=%d\n",__LINE__,__func__,debug);		//find this message when you do insmod in /var/log/kern.log
	if (debug > 4)
	{
		printk("%d %s:\tcritical problem while initialization\n",__LINE__,__func__);
	}

	// Try to dynamically allocate a major number for the device 
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);   
	if (majorNumber<0){
    	printk(KERN_ALERT "%d %s:\ttestdev: failed to register a major number\n",__LINE__,__func__);
    	return majorNumber;
   }

   printk(KERN_INFO "%d %s:\ttestdev: registered correctly with major number %d\n",__LINE__, __func__, majorNumber);

   // Register the device class
   testdevClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(testdevClass)){                // Check for error and clean up if there is

    	unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "%d %s:\ttestdev: failed to register device class\n",__LINE__, __func__);
        
		return PTR_ERR(testdevClass);          // Correct way to return an error on a pointer
   }

   printk(KERN_INFO "%d %s:\ttestdev: device class registered correctly\n",__LINE__, __func__);

   // Register the device driver
   testdevDevice = device_create(testdevClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(testdevDevice)){               // Clean up if there is an error

        class_destroy(testdevClass);           // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "%d %s:\ttestdev: Failed to create the device\n",__LINE__, __func__);
      
	    return PTR_ERR(testdevDevice);
   }

   printk(KERN_INFO "%d %s:\ttestdev: device class created correctly\n",__LINE__, __func__); // Made it! device was initialized

	return 0;					//indicates failure if not zero
}

static void __exit testdev_exit(void)
{
	device_destroy(testdevClass, MKDEV(majorNumber, 0));     // remove the device
   	class_unregister(testdevClass);                          // unregister the device class
   	class_destroy(testdevClass);                             // remove the device class
   	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number

	printk(KERN_INFO "%d %s:\ttestdev: driver deregistrartion success\n",__LINE__,__func__);
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){

   numberOpens++;

   printk(KERN_INFO "%d %s:\ttestdev: device has been opened %d time(s)\n",__LINE__,__func__, numberOpens);

   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success

      printk(KERN_INFO "%d %s:\ttestdev: sent %d characters to the user\n",__LINE__,__func__, size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0

   }
   else {

      printk(KERN_INFO "%d %s:\ttestdev: failed to send %d characters to the user\n",__LINE__,__func__, error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)

   }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message

   printk(KERN_INFO "%d %s:\ttestdev: received %zu characters from the user\n",__LINE__,__func__, len);
   return len;

}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){

   printk(KERN_INFO "%d %s:\ttestdev: device successfully closed\n",__LINE__,__func__);
   return 0;

}

/*this will call user defined initialization function, 
 *when module insmoded and default syntax for module initialization*/
module_init(testdev_init);
/*this will call user defined exit function when module id rmmodded
 *this module is nott mandatory for */
module_exit(testdev_exit);
/*This is the documentation part, after doing insmod of the module 
 *you can find details about this module by using "modinfo hello.ko" */

MODULE_LICENSE("Dual BSD/GPL");			// for getting rid of kernel taint warning
MODULE_AUTHOR(MOD_AUTHOR);		// name of the module author
MODULE_DESCRIPTION(MOD_DESC);		// a short description about module
MODULE_VERSION("0.0.1");            // A version number to inform users
MODULE_SUPPORTED_DEVICE(MOD_SUPPORT);	// devices this module uses
