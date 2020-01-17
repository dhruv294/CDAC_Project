#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
int fd;
char time[20];
int period;
fd = open("/dev/LedGPIO",O_RDONLY);
if(fd < 0)
{
	printf("couldn't opened file \n");
	_exit(0);
	}
read(fd,time,sizeof(time));
period = atoi(time);

printf("driver blinking time is: %d\n",period);


}
