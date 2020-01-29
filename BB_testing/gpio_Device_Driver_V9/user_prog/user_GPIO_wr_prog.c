#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int fd1,fd2,cnt;
	char GPIOOUTstatus[50];
	char GPIOINstatus[100];
	int period;
	if(argc != 3){
		printf("Prameter error: syntax( <.exec file> , <INPUT GPIO no.>, <OUPUT GPIO no.> )\n");
		_exit(0);
	}
	fd1 = open("/dev/GPIOOUT",O_RDWR);
	if(fd1 < 0)
	{
		printf("couldn't opened file \n");
		_exit(1);
	}
	cnt = write(fd1, argv[1], 10);

	fd2 = open("/dev/GPIOIN",O_RDWR);

	if(fd2 < 0)
	{
		printf("couldn't opened GPIO Button file \n");
		_exit(1);
	}
	cnt = write(fd2, argv[2], 10);

	close(fd1);
	close(fd2);

}

