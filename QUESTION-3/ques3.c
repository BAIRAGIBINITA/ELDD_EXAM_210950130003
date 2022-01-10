/*
3.	LED Operation Using GPIOs and Kernel Timers.
a)	Write a character driver using ioctl functionality, kernel timer.
b)	Use GPIO_23 to interface led on raspberry pi and blink the led periodically using kernel timer.
c)	Use ioctl functionality to change the periodicity of led blinking
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/timer.h>
#include <linux/jiffies.h>

//Timer Variable
#define TIMEOUT 5000    //milliseconds
static struct timer_list etx_timer;
static unsigned int count = 0;

#define WR_VALUE _IOW('a','a',u_int8_t *)
#define RD_VALUE _IOR('a','b',u_int8_t *)
uint8_t  value = 0;
uint8_t gpio_state = 0;
//LED is connected to this GPIO
#define GPIO_23 (23)
 
dev_t dev = 0;	//to hold the device numbers
static struct class *dev_class;
static struct cdev led_cdev;
 
static int __init led_driver_init(void);
static void __exit led_driver_exit(void); 
 
/*************** Driver functions **********************/
static int led_open(struct inode *inode, struct file *file);
static int led_release(struct inode *inode, struct file *file);
static ssize_t led_read(struct file *filp,char __user *buf, size_t len,loff_t * off);
static ssize_t led_write(struct file *filp,const char *buf, size_t len, loff_t * off);
static long     LED_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
/******************************************************/
 
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = led_read,
  .write          = led_write,
  .open           = led_open,
  .unlocked_ioctl = LED_ioctl,
  .release        = led_release,
};

//Timer Callback function. This will be called when timer expires
void timer_callback(struct timer_list * data)
{
    /* do your timer stuff here */
    pr_info("Timer Callback function Called [%d]\n",count++);
    
    /*
       Re-enable timer. Because this function will be called only first time. 
       If we re-enable this will work like periodic timer. 
    */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
    }
/*
** This function will be called when we open the Device file
*/ 
static int led_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}
/*
** This function will be called when we close the Device file
*/
static int led_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}
/*
** This function will be called when we read the Device file

This is used to retrieve data from the device. A null pointer in this position causes the read system call to fail with -EINVAL (“Invalid argument”). A non-negative return value represents the number of bytes successfully read (the return value is a “signed size” type, usually the native integer type for the target platform)
*/ 
static ssize_t led_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
  pr_info("Read Function\n");
        return 0;
}
/*
** This function will be called when we write the Device file

It is used to sends the data to the device. If NULL -EINVAL is returned to the program calling the write system call. The return value, if non-negative, represents the number of bytes successfully written.
*/ 
static ssize_t led_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
  pr_info("Write function\n");
        return len;
}

static long LED_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
		//uint8_t led_buf[10] = {0};
		//uint8_t gpio_state = 0;
         switch(cmd) {
                case WR_VALUE:
                        
  
  			if( copy_from_user( &value,(u_int8_t *) arg, sizeof(value)) > 0) 
			  {
			    pr_err("ERROR: Not all the bytes have been copied from user\n");
			  }
			  
			  pr_info("Write Function : GPIO_21 Set = %c\n", value);
			  
			  if (value=='1') 
			  {
			    //set the GPIO value to HIGH
			    gpio_set_value(GPIO_23, 1);
			  } else if (value=='0') 
			  {
			    //set the GPIO value to LOW
			    gpio_set_value(GPIO_23, 0);
			  } else 
			  {
			    pr_err("Unknown command : Please provide either 1 or 0 \n");
			  }			  
			  return sizeof(value);
                        break; 
                case RD_VALUE:
                        
  
			  //reading GPIO value
			  value = gpio_get_value(GPIO_23);
			  
			  //write to user
			  //len = 1;
			  if( copy_to_user((u_int8_t *) arg, &value, sizeof(value)) > 0) 
			  {
			    pr_err("ERROR: Not all the bytes have been copied to user\n");
			  }
			  
			  pr_info("Read function : GPIO_23 = %d \n", value);
			  
			  return 0;            
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}
/*
** Module Init function
*/ 
static int __init led_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "led_Dev")) <0)
  {
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
  /*Creating cdev structure,cdev structure is set up with file_operations and owner*/
  cdev_init(&led_cdev,&fops);
 
  /*Adding character device to the system*/
  /*After a call to cdev_add(), your device is immediately alive. All functions you defined (through the file_operations structure) can be called.*/
  if((cdev_add(&led_cdev,dev,1)) < 0)
  {
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }
 
  /*Creating struct class*/
 if((dev_class = class_create(THIS_MODULE,"led_class")) == NULL)
  {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
 
  /*Creating device*/
  if((device_create(dev_class,NULL,dev,NULL,"led_device")) == NULL)
  {
    pr_err( "Cannot create the Device \n");
    goto r_device;
  }
  
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_23) == false)
  {
    pr_err("GPIO %d is not valid\n", GPIO_23);
    goto r_device;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_23,"GPIO_23") < 0)
  {
    pr_err("ERROR: GPIO %d request\n", GPIO_23);
    goto r_gpio;
  }
  
  //configure the GPIO as output
  gpio_direction_output(GPIO_23, 0);
  
  /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio21/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  gpio_export(GPIO_23, false);
  
  timer_setup(&etx_timer, timer_callback, 0);       //If you face some issues and using older kernel version, then you can try setup_timer API(Change Callback function's argument to unsingned long instead of struct timer_list *.
 
    /* setup timer interval to based on TIMEOUT Macro */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
 
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
  
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
 
r_gpio:
  gpio_free(GPIO_23);
r_device:
  device_destroy(dev_class,dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&led_cdev);
r_unreg:
  unregister_chrdev_region(dev,1);
  
  return -1;
}
/*
** Module exit function
*/ 
static void __exit led_driver_exit(void)
{
  del_timer(&etx_timer);
  gpio_unexport(GPIO_23);
  gpio_free(GPIO_23);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&led_cdev);	//To remove a char device from the system
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(led_driver_init);
module_exit(led_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("BINITA");
MODULE_DESCRIPTION("GPIO LED Driver");

