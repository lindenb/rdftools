#ifndef UTILS_HH
#define UTILS_HH
#include <unistd.h>
#include <cstdlib>


#define EXIT_FAILURE std::exit(EXIT_FAILURE)

/** IO functions  */
ssize_t safeRead(int fd, void *buf, size_t count);
ssize_t safeWrite(int fd, const void *buf, size_t count);



#endif

