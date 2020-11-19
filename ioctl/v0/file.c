//compile with arm-linux-gnueabi-gcc file.c -o file.out
#include<unistd.h>
#include<fcntl.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define IOC_MAGIC ‘p’ 
#define MY_IOCTL_LEN    _IO(IOC_MAGIC, 1) 
#define MY_IOCTL_AVAIL	_IO(IOC_MAGIC, 2) 
#define MY_IOCTL_RESET 	_IO(IOC_MAGIC, 3)

int main()
{
	int fd,ret;
	fd = open("/dev/psample", O_RDWR);  
if(fd<0) 
{   
 	perror("open");    
	exit(1);  
}
 ret=ioctl(fd, MY_IOCTL_LEN);  
if(ret<0)   
{    
 	perror("ioctl");     
	exit(3);  
}  
ret=ioctl(fd, MY_IOCTL_AVAIL); 
if(ret<0)  
 {    
 perror("ioctl");    
 exit(3); 
 }  
ret=ioctl(fd, MY_IOCTL_RESET); 
if(ret<0) {    
perror("ioctl");     
exit(3); 
 } 
close(fd);

}
		


		








