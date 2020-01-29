CC_ARM=arm-linux-gnueabihf-gcc
TARGET=user_GPIO_rd_prog
CSRC=$(TARGET).c
all:
	$(CC_ARM) -o  ${TARGET}.out ${CSRC}

install:
	scp ${TARGET}.out debian@192.168.0.1:/home/debian/test_led/user

.PHONY: all install
