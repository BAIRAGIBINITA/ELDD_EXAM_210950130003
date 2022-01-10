# ELDD_EXAM_210950130003
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



2.	Kernel Synchronization Mechanisms for Multiple devices.
a)	Write a character driver for Multiple devices and create 2 device numbers.Use semaphore to protect the Critical section (write activity) and Use wait-queues to avoid consecutive write operations.
b)	Maintain a Kernel buffer of 50 bytes to  Read and Write data.
c)	Put write function to sleep using wait-queues after successful write operation before releasing semaphore.
d)	Wake-up sleep(wait-queues) operation only in Read function.
e)	Write 2 separate programs for Read and Write for each device.

3.	LED Operation Using GPIOs and Kernel Timers.
a)	Write a character driver using ioctl functionality, kernel timer.
b)	Use GPIO_23 to interface led on raspberry pi and blink the led periodically using kernel timer.
c)	Use ioctl functionality to change the periodicity of led blinking

