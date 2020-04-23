obj-m := chardev_orga.o

all :
	make -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(shell pwd) modules

clean :
	make -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(shell pwd) clean
