	obj-m := ques3.o


	KERNELDIR = /lib/modules/$(shell uname -r)/build
	
	PWD=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o led_app led_app.c
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm led_app

