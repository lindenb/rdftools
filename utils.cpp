
#include "utils.hh"

ssize_t safeRead(int fd, void *buf, size_t count)
	{
	ssize_t n= ::read(fd,buf,count);
	}
	
ssize_t safeWrite(int fd, const void *buf, size_t count)
	{
	ssize_t n= ::write(fd,buf,count);
	}
