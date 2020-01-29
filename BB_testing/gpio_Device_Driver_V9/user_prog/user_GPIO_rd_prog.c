#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int fd1,fd2,cnt;
	char GPIOINstatus[50];
	char GPIOOUTstatus[100];
	int period;
	fd1 = open("/dev/GPIOOUT",O_RDONLY);
	if(fd1 < 0)
	{
		printf("couldn't opened file \n");
		_exit(1);
	}
	read(fd1,GPIOOUTstatus,sizeof(GPIOOUTstatus));

	fd2 = open("/dev/GPIOIN",O_RDONLY);

	if(fd2 < 0)
	{
		printf("couldn't opened GPIO Button file \n");
		_exit(1);
	}

	read(fd2,GPIOINstatus,sizeof(GPIOINstatus));


	printf("status of GPIO OUT is: %s\n",GPIOOUTstatus);
	printf("status of GPIO IN is: %s\n",GPIOINstatus);

	close(fd1);
	close(fd2);

}

