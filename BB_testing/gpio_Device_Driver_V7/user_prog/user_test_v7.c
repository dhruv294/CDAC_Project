#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
int fd;
char time[20] = "10";
int period;
fd = open("/dev/GPIOLED",O_WRONLY);
if(fd < 0)
{
	printf("couldn't opened file \n");
	_exit(0);
	}
write(fd,time,sizeof(time));


printf("driver blinking time is: %d\n",period);


}
