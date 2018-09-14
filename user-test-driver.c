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
#include <inttypes.h>
#include <sys/ioctl.h>
#include "test-driver.h"

#define WR_VALUE _IOW(MAJOR_NUMBER, 0, int32_t *)
#define RD_VALUE _IOR(MAJOR_NUMBER, 1, int32_t *)

static char buffer[BUFFER_LENGTH]; // The buffer buffer from the LKM

int16_t reads( const char* prompt, char * buff, uint16_t len )
{
        int idx = 0;
        char c;
        if( buff == NULL ) return -1;
        memset(buff, 0, len);
        printf(prompt);
        while( (c = getchar() )!= '\n' && idx < len )
        {
                buff[idx++] = c;
        }
return idx;
}

int testrdwr(int fd)
{
    int ret;

    printf("Type in a short string to send to the kernel module:\n");
    fflush(stdin);
    scanf(" %[^\n]%*c", buffer); // Read in a string (with spaces)
    printf("Writing message to the device [%s].\n", buffer);

    ret = write(fd, buffer, strlen(buffer)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }

    printf("Press ENTER to read back from the device...\n");
    getchar();

    printf("Reading from the device...\n");
    ret = read(fd, buffer, BUFFER_LENGTH); // Read the response from the LKM
    if (ret < 0)
    {
        perror("Failed to read the message from the device.");
        return errno;
    }

    printf("The bufferd message is: [%s]\n", buffer);
    return 0;
}

int main()
{

    int fd;
    int choice;
    int32_t value, number;

    printf("*****************************************\n");
    printf("*                test menu              *\n");
    printf("*****************************************\n");
    printf("1. Test device read-write\n");
    printf("2. Test ioctl operation\n");
    printf("Enter your choice\n");

    scanf("%d", &choice);
    fd = open("testdev", O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }

    switch (choice)
    {
    case 1:
        testrdwr(fd);
        break;

    case 2:
        printf("Enter the Value to send\n");
        scanf("%" SCNd32, &number);
        printf("Writing Value to Driver\n");
        ioctl(fd, WR_VALUE, (int32_t *)&number);

        printf("Reading Value from Driver\n");
        ioctl(fd, RD_VALUE, (int32_t *)&value);
        printf("Value is %d\n", value);
        break;

    default:
        printf("Wrong input\n");
    }

    printf("End of the program\n");
    close(fd);
    return 0;
}
