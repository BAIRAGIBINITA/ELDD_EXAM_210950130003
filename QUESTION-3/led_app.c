#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
#define WR_VALUE _IOW('a','a',u_int8_t *)	//define the ioctl command
#define RD_VALUE _IOR('a','b',u_int8_t *)
int main()
{
        int fd;
        u_int8_t  value, number;
        printf("****************User space IOCTL application*****************\n"); 
        printf("\nOpening Driver\n");
        fd = open("/dev/led_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        printf("Enter the Value to send\n");
        scanf("%hhd",&number);
        printf("Writing Value to Driver\n");
        ioctl(fd, WR_VALUE, (u_int8_t *) &number); 
 
        printf("Reading Value from Driver\n");
        ioctl(fd, RD_VALUE, (u_int8_t *) &value);
        printf("Value is %d\n", value);
 
        printf("Closing Driver\n");
        close(fd);
}
