#if the KERNELRELEASE is defined, we've been invoked from the kernel build system and can use its language
ifneq ($(KERNELRELEASE),)
	obj-m := hello_world_dri.o

# otherwise we were called directly ftom the command line; invoke the kernel build system
else
	KERNELDIR ?= /lib/modules/`uname -r`/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

endif

clean:
	find . -maxdepth 1  -name 'hello_world_dri*' ! -name '*.ko' ! -name '*.c' -delete
	rm -f *mod.c
	rm -f [mM]odule*

