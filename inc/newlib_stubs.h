#include <sys/unistd.h>

#define STDIN_BUFFER_SIZE 1024

#ifndef __NEWLIB_STUBS
extern volatile uint8_t stdin_buffer[];
extern volatile uint16_t stdin_buffer_in;
extern volatile uint16_t stdin_buffer_len;
#endif
