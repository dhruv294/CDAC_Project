#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
int fd;
char time[50];
int period;
fd = open("/dev/GPIOLED",O_RDONLY);
if(fd < 0)
{
	printf("couldn't opened file \n");
	_exit(0);
	}
read(fd,time,sizeof(time));


printf("%s\n",time);


}
