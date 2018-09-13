/* @Module: kernel-test-driver.c  
 * @Author: Pawan Kumar
 * @Email: jmppawanhit@gmail.com
 * @License: GPL
 * @Domain: Kernel Module Programming
 * @Description: Demonstrating the simple  kernel  test driver. This  is  the
 *               demonstration code for implementation of character driver in
 *               kernel module.
 *               for more detail on hello world module refer master branch
 * @Copyright: Copyright (C) 2018
 */


/******************************************
 *               includes                 *
 ******************************************/
#include <linux/module.h>       // required for all
#include <linux/kernel.h>       // for kernel loglevels like KERN_INFO
#include <linux/init.h>	        // nedded if we want to use user defined name for init and exit
//#include <linux/device.h>     // Header to support the kernel Driver Model [optional]
#include <linux/fs.h>           // Header for the Linux file system support
#include <linux/uaccess.h>      // Required for the copy to user function
#include <linux/ioctl.h>		// For ioctl support



#define DEVICE_NAME "testdev"   // The device will appear at /dev/testdev using this message
#define CLASS_NAME  "test"      // The device class -- this is a character device driver
#define MOD_AUTHOR "Pawan Kumar <jmppawanhit@gmail.com>"
#define MOD_DESC "Hello world driver"
#define MOD_SUPPORT "test device"
#define MAJOR_NUMBER 240

/*******************************************
 *                modinfo                  *
 *******************************************/
MODULE_LICENSE("Dual BSD/GPL");         // open source licensing
MODULE_AUTHOR(MOD_AUTHOR);              // name of the module author
MODULE_DESCRIPTION(MOD_DESC);           // a short description about module
MODULE_VERSION("0.1");                  // A version number to inform users
MODULE_SUPPORTED_DEVICE(MOD_SUPPORT);   // devices this module uses


/*******************************************
 *                 global                  *
 *******************************************/

static char   message[1023];                // Memory for the string that is passed from userspace
static short  sizeOfMessage;                // Used to remember the size of the string stored
static int    lock;                         // Counts the number of times the device is opened
static int32_t mode;
#define WR_VALUE _IOW(MAJOR_NUMBER, 0,int32_t*)     // #define "IOCTL_NAME" __IOX("magic number","command number","argument type")
#define RD_VALUE _IOR(MAJOR_NUMBER, 1,int32_t*)     // refer: https://embetronicx.com/tutorials/linux/device-drivers/ioctl-tutorial-in-linux/
/*  optional for creating the device file  
static struct class*  testdevClass  = NULL; // The device-driver class struct pointer
static struct device* testdevDevice = NULL; // The device-driver device struct pointer
*/


/********************************************
 *                prototypes                *
 ********************************************/
static int     testdev_init(void);
static void    testdev_exit(void);
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long    dev_ioctl(struct file *, unsigned int , unsigned long );
static int     dev_release(struct inode *, struct file *);


/* @brief Devices are represented as file structure in the kernel. The file_operations structure from
 * /linux/fs.h lists the callback functions that you wish to associated with your file operations
 * using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .owner = THIS_MODULE,
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .unlocked_ioctl = dev_ioctl,
   .release = dev_release,
};


/* @brief The LKM initialization function
 * The static keyword restricts the visibility of the function to within this C file. The __init
 * macro means that for a built-in driver (not a LKM) the function is only used at initialization
 * time and that it can be discarded and its memory freed up after that point.
 * @return returns 0 if successful
 */
static int __init testdev_init(void)
{
	int8_t ret;
	printk(KERN_INFO "%d %s: Initializing test driver...\n",__LINE__,__func__);
	// allocate the device major number for testdev 
    	ret = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);   
	if (ret<0){
    		printk(KERN_ALERT "%d %s: testdev: failed to register a major number\n",__LINE__,__func__);
		
		return ret;
	}

	printk(KERN_INFO "%d %s: testdev: registered correctly with major number %d\n",__LINE__, __func__, MAJOR_NUMBER);

/*
   	// Register the device class
   	testdevClass = class_create(THIS_MODULE, CLASS_NAME);
   	if (IS_ERR(testdevClass)){                // Check for error and clean up if there is
    		unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
        	printk(KERN_ALERT "%d %s: testdev: failed to register device class\n",__LINE__, __func__);
	
		return PTR_ERR(testdevClass);     // Correct way to return an error on a pointer
   	}

   	printk(KERN_INFO "%d %s: testdev: device class registered correctly\n",__LINE__, __func__);

   	// Register the device driver
   	testdevDevice = device_create(testdevClass, NULL, MKDEV(MAJOR_NUMBER, 0), NULL, DEVICE_NAME);
   	if (IS_ERR(testdevDevice)){                    // Clean up if there is an error
        	class_destroy(testdevClass);           // Repeated code but the alternative is goto statements
        	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
        	printk(KERN_ALERT "%d %s: testdev: Failed to create the device\n",__LINE__, __func__);
      
	    	return PTR_ERR(testdevDevice);
   	}

   	printk(KERN_INFO "%d %s: testdev: device class created correctly\n",__LINE__, __func__); // Made it! device was initialized
*/
	return 0;					//indicates failure if not zero
}

/* @brief The LKM cleanup function
 * Similar to the initialization function, it is static. The __exit macro notifies that if this
 * code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit testdev_exit(void)
{
	//device_destroy(testdevClass, MKDEV(MAJOR_NUMBER, 0));     // remove the device
   	//class_unregister(testdevClass);                           // unregister the device class
   	//class_destroy(testdevClass);                              // remove the device class
   	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);               // unregister the major number
	
	printk(KERN_INFO "%d %s: testdev: driver deregistrartion success\n",__LINE__,__func__);
}


/* @brief The device open function that is called each time the device is opened
 * This will only increment the lock counter in this case.
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){

   	lock++;

   	printk(KERN_INFO "%d %s: testdev: device has been opened %d time(s)\n",__LINE__,__func__, lock);

   	return 0;
}


/* @brief This function is called whenever device is being read from user space i.e. data is
 * being sent from the device to the user. In this case is uses the copy_to_user() function to
 * send the buffer string to the user and captures any errors.
 * @param filep A pointer to a file object (defined in linux/fs.h)
 * @param buffer The pointer to the buffer to which this function writes the data
 * @param len The length of the b
 * @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   	int error_count = 0;
	   
	error_count = copy_to_user(buffer, &message, sizeOfMessage);

   	if (error_count==0){                 // if true then have success
      		printk(KERN_INFO "%d %s: testdev: sent %d characters to the user\n",__LINE__,__func__, sizeOfMessage);
			sizeOfMessage=0;    // clear the position to the start and return 0  
   	}else {

      		printk(KERN_INFO "%d %s: testdev: failed to send %d characters to the user\n",__LINE__,__func__, error_count);
      	
	return -EFAULT;                      // Failed -- return a bad address message (i.e. -14)
   	}

return sizeOfMessage;
}


/* @brief This function is called whenever the device is being written to from user space i.e.
 * data is sent to the device from the user. The data is copied to the message[] array in this
 * LKM using the copy_from_user() function along with the length of the string.
 * @param filep A pointer to a file object
 * @param buffer The buffer to that contains the string to write to the device
 * @param len The length of the array of data that is being passed in the const char buffer
 * @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	sizeOfMessage = len;
	copy_from_user(message, buffer, len);
   	printk(KERN_INFO "%d %s: testdev: received %zu characters from the user\n",__LINE__,__func__, len);
   
	return len;
}


/* @brief This function is called whenever the ioctl function is invoked from user space i.e.
 * user will call ioctl with some mode and argument.
 * @param file <file descriptor>: This the open file on which the ioctl command needs to be executed, which would generally be device files.
 * @param cmd <ioctl command>: ioctl command which is implemented to achieve the desired functionality
 * @param arg <arguments>: The arguments that needs to be passed to the ioctl command.
 */
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        copy_from_user(&mode , (int32_t*)arg, sizeof(mode));
                        printk(KERN_INFO "%d %s: ioctl_op: value = %d\n", __LINE__,__func__, mode);
                        break;
                case RD_VALUE:
                        copy_to_user((int32_t*)arg, &mode, sizeof(mode));
                        break;
        }
        return 0;
}


/* @brief The device release function that is called whenever the device is closed/released by
 * the userspace program
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){

   	printk(KERN_INFO "%d %s: testdev: device successfully closed\n",__LINE__,__func__);
	
	return 0;
}

module_init(testdev_init);
module_exit(testdev_exit);