	obj-m := q1.o


	KERNELDIR = /lib/modules/$(shell uname -r)/build
	
	PWD=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o app1 app1.c
	gcc -o app2 app2.c
	gcc -o app3 app3.c
	
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm app1
	rm app2
	rm app3

