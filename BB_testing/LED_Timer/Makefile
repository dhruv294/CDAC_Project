obj-m := test_timer.o
KDIR :=/home/dhruvshn/BeagleBoneBlack/Linux_Porting/kernel/linux-4.4

all:
	make ARCH=arm CROSS_COMPILE=${CC} -C $(KDIR) M=`pwd` modules

clean:
	make -C $(KDIR) M=`pwd` clean


