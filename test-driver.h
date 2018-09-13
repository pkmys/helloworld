#ifndef _test_driver_h
#define _test_driver_h
#endif

#define DEVICE_NAME "testdev"   // The device will appear at /dev/testdev using this message
#define CLASS_NAME  "test"      // The device class -- this is a character device driver
#define MOD_AUTHOR "Pawan Kumar <jmppawanhit@gmail.com>"
#define MOD_DESC "Hello world driver"
#define MOD_SUPPORT "test device"
#define MAJOR_NUMBER 240
#define BUFFER_LENGTH 256               // The buffer length (crude but fine) 