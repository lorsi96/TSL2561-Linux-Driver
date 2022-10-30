obj-m = hello_world.o
KVERSION = 5.15.74-sunxi64
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
