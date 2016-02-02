/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */
#define  __NEWLIB_STUBS
#include <stdint.h>
#include "newlib_stubs.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>

#ifdef TARGET_F407
#include "stm32f4xx.h"
#include "usbd_cdc_vcp.h"

volatile uint8_t stdin_buffer[STDIN_BUFFER_SIZE];
volatile uint16_t stdin_buffer_in=0;
volatile uint16_t stdin_buffer_len=0;
volatile uint16_t stdin_buffer_out=0;
#endif

#ifdef TARGET_F091
#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#endif

#undef errno
extern int errno;

/*
 environ
 A pointer to a list of environment variables and their values. 
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status) {
    _write(1, "exit", 4);
    while (1) {
        ;
    }
}

int _close(int file) {
    return -1;
}
/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
/*int _execve(char *name, char **argv, char **env) {
    errno = ENOMEM;
    return -1;
}*/
/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */

/*int _fork() {
    errno = EAGAIN;
    return -1;
}*/
/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int _getpid() {
    return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file) {
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}


/*
 kill
 Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig) {
    errno = EINVAL;
    return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */

/*int _link(char *old, char *new) {
    errno = EMLINK;
    return -1;
}*/

/*
 lseek
 Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir) {
    return 0;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */

static char *heap_end = 0;
caddr_t _sbrk(int incr) {

    extern char _ebss; // Defined by the linker
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_ebss;
    }
    prev_heap_end = heap_end;

char * stack = (char*) __get_MSP();
     if (heap_end + incr >  stack)
     {
         _write (STDERR_FILENO, "Heap and stack collision\n", 25);
         errno = ENOMEM;
         return  (caddr_t) -1;
         //abort ();
     }

    heap_end += incr;
    return (caddr_t) prev_heap_end;

}

/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */


int _read(int file, char *ptr, int len)
{
	int n = len;

	switch (file) {
	case STDIN_FILENO:
		while (len--) {
#ifdef TARGET_F407
			while (!stdin_buffer_len)
				;
			if (stdin_buffer_out >= STDIN_BUFFER_SIZE)
				stdin_buffer_out = 0;
			*ptr++ = stdin_buffer[stdin_buffer_out++];
			stdin_buffer_len--;
#endif
#ifdef TARGET_F091
			while (!USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
				;
			*ptr++ = (int)USART_ReceiveData(USART2);
#endif
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return n;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

/*int _stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}*/

/*
 times
 Timing information for current process. Minimal implementation:
 */

/*clock_t _times(struct tms *buf) {
    return -1;
}*/

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
/*int _unlink(char *name) {
    errno = ENOENT;
    return -1;
}*/

/*
 wait
 Wait for a child process. Minimal implementation:
 */
/*int _wait(int *status) {
    errno = ECHILD;
    return -1;
}*/

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
	int n = len;

	switch (file) {
	case STDOUT_FILENO: /*stdout*/
	case STDERR_FILENO: /* stderr */
		while (len--) {
#ifdef TARGET_F407
			VCP_DataTx(ptr++);
#endif
#ifdef TARGET_F091
			while (!USART_GetFlagStatus(USART2, USART_FLAG_TXE))
				;
			USART_SendData(USART2, *ptr++);
#endif
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return n;
}
