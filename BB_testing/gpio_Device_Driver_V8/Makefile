Target := test_v8
obj-m := ${Target}.o
KDIR :=/home/dhruvshn/BeagleBoneBlack/Linux_Porting/kernel/linux-4.4

all:
	make ARCH=arm CROSS_COMPILE=${CC} -C $(KDIR) M=`pwd` modules

install:
	scp ${Target}.ko debian@192.168.0.1:/home/debian/test_led

clean:
	make -C $(KDIR) M=`pwd` clean


