#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
int fd1,fd2;
char ledstatus[50];
char Buttonstatus[100];
int period;
fd1 = open("/dev/GPIOLED",O_RDONLY);
if(fd1 < 0)
{
	printf("couldn't opened file \n");
	_exit(0);
	}
fd2 = open("/dev/GPIOButton",O_RDONLY);
if(fd2 < 0)
{
	printf("couldn't opened GPIO Button file \n");
	_exit(0);
	}
read(fd1,ledstatus,sizeof(ledstatus));
read(fd2,Buttonstatus,sizeof(Buttonstatus));


printf("%s\n",ledstatus);
printf("%s\n",Buttonstatus);

close(fd1);
close(fd2);

}
