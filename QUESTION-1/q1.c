/*
1.	Verify User Read/Write operations using IOCTL functionality.
a)	Write a Simple character driver with Open , Read ,Write , Close and Ioctl Functionality.
b)	Initialize Structure in Kernel space which records kernel buffer data , size of the buffer and Recent activity(Read = 0 /Write = 1).
	  struct stats
		{
                                  int size;
                                  char buff[];
                                  int r_w;
			};
c)	Write 3 separate user programs for Read, Write and Ioctl in User application.
d)	Check the status of driver using Ioctl program with command GETSTATS, which should return structure(struct stats) from kernel space.
e)	User should run write application first with any string and check stats using Ioctl program and then run Read application and check stats using Ioctl program and verify the string and recent activity(r_w).
*/


#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
int32_t value = 0;
#define mem_size        1024           //Memory Size
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev RLDCdev_cdev;
uint8_t *kernel_buffer;
/*
** Function Prototypes
*/
static int      __init Real_Linux_Device_driver_init(void);
static void     __exit Real_Linux_Device_driver_exit(void);
static int      RLDCdev_open(struct inode *inode, struct file *file);
static int      RLDCdev_release(struct inode *inode, struct file *file);
static ssize_t  RLDCdev_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  RLDCdev_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long     etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
/*
** File Operations structure
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = RLDCdev_read,
        .write          = RLDCdev_write,
        .open           = RLDCdev_open,
        .release        = RLDCdev_release,
};
 
/*
** This function will be called when we open the Device file
*/
static int RLDCdev_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int RLDCdev_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t RLDCdev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        /*Copy the data from the kernel space to the user-space
        This function is used to Copy a block of data into userspace (Copy data from kernel space to user space).
        */
        
        if( copy_to_user(buf, kernel_buffer, mem_size) )
        {
                pr_err("Data Read : Err!\n");
        }
        pr_info("Data Read : Done!\n");
        return mem_size;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t RLDCdev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        /*Copy the data to kernel space from the user-space
        This function is used to Copy a block of data from user space (Copy data from user space to kernel space).
        */
        
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        pr_info("Data Write : Done!\n");
        return len;
}


static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}
 
/*
** Module Init function
*/
static int __init Real_Linux_Device_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "Real_Linux_Device_Driver")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&RLDCdev_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&RLDCdev_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"Real_Linux_Device_class")) == NULL){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"Real_Linux_Device_device")) == NULL){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        
        /*Creating Physical memory in kernel,kmalloc function is used to allocate the memory in kernel space.
        size– how many bytes of memory are required.
	flags– the type of memory to allocate,GFP_KERNEL – Allocate normal kernel ram. May sleep..*/
	
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0)
        {
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        /*after creating physical memory in kernel we are writing the hello world sting as a default string in that memory*/
        strcpy(kernel_buffer, "Hello_World");
        
        pr_info("Real Linux Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
/*
** Module exit function
*/
static void __exit Real_Linux_Device_driver_exit(void)
{
  	kfree(kernel_buffer);//we are freeing kernel memory,This is used to free the previously allocated memory.
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&RLDCdev_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Real Linux Device Driver Remove...Done!!!\n");
}
 
module_init(Real_Linux_Device_driver_init);
module_exit(Real_Linux_Device_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("BINITA");
MODULE_DESCRIPTION("Real Linux Device Driver");

