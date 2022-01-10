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



