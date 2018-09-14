/* @Module: user-test-driver.c  
 * @Author: Pawan Kumar
 * @Email: jmppawanhit@gmail.com
 * @License: GPL
 * @Domain: Kernel Module Programming
 * @Description: Common header for user module and kernel module
 * @Copyright: Copyright (C) 2018 pkmys@github.com
 */

#ifndef _test_driver_h
#define _test_driver_h
#endif

#define DEVICE_NAME "testdev" // The device will appear at /dev/testdev using this message
#define CLASS_NAME "test"     // The device class -- this is a character device driver
#define MOD_AUTHOR "Pawan Kumar <jmppawanhit@gmail.com>"
#define MOD_DESC "Hello world driver"
#define MOD_SUPPORT "test device"
#define MAJOR_NUMBER 240
#define BUFFER_LENGTH 256 // The buffer length (crude but fine)