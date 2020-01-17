CC_ARM=arm-linux-gnueabihf-gcc
CSRC=user_test_v7.c
TARGET=user_v7
all:
	$(CC_ARM) -o  ${TARGET}.out ${CSRC}

install:
	scp ${TARGET}.out debian@192.168.0.1:/home/debian/test_led/user

.PHONY: all install
