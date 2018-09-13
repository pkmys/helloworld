/* @Module: user-test-driver.c  
 * @Author: Pawan Kumar
 * @Email: jmppawanhit@gmail.com
 * @License: GPL
 * @Domain: Kernel Module Programming
 * @Description: Demonstrating the simple  user  test driver. This  is  the
 *               demonstration code for implementation of character driver in
 *               user module.
 * @Copyright: Copyright (C) 2018 pkmys
 */

/******************************************
 *               includes                 *
 ******************************************/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define BUFFER_LENGTH 256               // The buffer length (crude but fine) 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

static char receive[BUFFER_LENGTH];     // The receive buffer from the LKM
static char stringToSend[BUFFER_LENGTH];

int testrdwr(int fd)
{
    int ret;

    printf("Type in a short string to send to the kernel module:\n");
    fflush(stdin);
    scanf("%[^\n]%*c", stringToSend);                // Read in a string (with spaces)
    printf("Writing message to the device [%s].\n", stringToSend);

    ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
    if (ret < 0){
        perror("Failed to write the message to the device.");
        return errno;
    }

    printf("Press ENTER to read back from the device...\n");
    getchar();

    printf("Reading from the device...\n");
    ret = read(fd, receive, BUFFER_LENGTH);       // Read the response from the LKM
    if (ret < 0){
        perror("Failed to read the message from the device.");
        return errno;
    }

    printf("The received message is: [%s]\n", receive);
    return 0;
}
 
int main(){

   int fd;
   int choice;
   int32_t value, number;

   fd = open("testdev", O_RDWR);        // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
start:
   printf("*****************************************\n");
   printf("*                test menu              *\n");
   printf("*****************************************\n");
   printf("1. Test device read-write\n");
   printf("2. Test ioctl operation\n");
   printf("Enter your choice\n");
  
   scanf("%d",&choice);

    switch (choice) {
    case 1: {
        testrdwr(fd);
        break;
    }
    case 2: {
        printf("Enter the Value to send\n");
        scanf("%d",&number);
        printf("Writing Value to Driver\n");
        ioctl(fd, WR_VALUE, (int32_t*)&number); 
 
        printf("Reading Value from Driver\n");
        ioctl(fd, RD_VALUE, (int32_t*)&value);
        printf("Value is %d\n", value);     
        break;
    }
    default:
        printf("Wrong input\n");
        goto start;
    }
   
   printf("End of the program\n");
   close(fd);
   return 0;
}