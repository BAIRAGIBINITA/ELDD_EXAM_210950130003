#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
//int8_t write_buf[1024];
//int8_t read_buf[1024]; 
int main()
{
        int fd;
        int32_t value, number;
        printf("****************User space IOCTL application*****************\n"); 
        printf("\nOpening Driver\n");
        fd = open("/dev/Real_Linux_Device_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }       
        
        
        printf("Enter the Value to send\n");
        scanf("%d",&number);
        printf("Writing Value to Driver\n");
        ioctl(fd, WR_VALUE, (int32_t*) &number); 
 
        printf("Reading Value from Driver\n");
        ioctl(fd, RD_VALUE, (int32_t*) &value);
        printf("Value is %d\n", value);
 
        printf("Closing Driver\n");
        close(fd);
}
