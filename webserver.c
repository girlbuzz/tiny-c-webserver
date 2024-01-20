#include "syscall-macros.h"

/*
	The first section of this struct is used to capture the result of fstat
	The second section is all the variables for the rest of the program
	Saves like 48 bytes
*/
struct everything {
	unsigned long int st_dev;
	unsigned long int st_ino;
	unsigned long int st_nlink;
	unsigned int st_mode;
	unsigned int st_uid;
	unsigned int st_gid;
	int __pad0;
	unsigned long int st_rdev;
	long int st_size;
	long int st_blksize;
	long int st_blocks;
	long int st_atime;
	unsigned long int st_atimensec;
	long int st_mtime;
	unsigned long int st_mtimensec;
	long int st_ctime;
	unsigned long int st_ctimensec;
	unsigned long int __glibc_reserved[3];
	
	int server_sock_fd;
	struct sockaddr_in server_addr;
	int bind_res;
	int client_sock_fd;
	int bytes_received;
	char recvbuf[1024];
	char* recvbufptr;
	int file_fd;
	char* brk_start;
	char* brk_end;
	int i;
	int file_bytes_read;
	int sendto_res;
};

#define PORT 8081

void _start() {
	struct everything prgm; /* Hack to save a few bytes, read comment at struct definition */

	/* Create socket */
	asm_socket(prgm.server_sock_fd, AF_INET, SOCK_STREAM, 0);

	/* Config */
	prgm.server_addr.sin_family = AF_INET;
	prgm.server_addr.sin_addr.s_addr = INADDR_ANY;
	prgm.server_addr.sin_port = ((PORT & 0xFF) << 8) | ((PORT & 0xFF00) >> 8); /* Swap the 1st and 2nd byte of port */

	/* Bind sock to port */
	asm_bind(prgm.bind_res, prgm.server_sock_fd, &prgm.server_addr, sizeof(prgm.server_addr));

	/* Listen for connections */
	asm_listen(prgm.server_sock_fd, 16);

	while (1) {
		/* Accept new connection */
		asm_accept(prgm.client_sock_fd, prgm.server_sock_fd);

		/* Receive their request */
		asm_recvfrom(prgm.bytes_received, prgm.client_sock_fd, prgm.recvbuf, 1024, 0);

		/* Check if GET */
		if (prgm.recvbuf[0] == 'G') { /* This is the most minimal way to possibly check, no other method starts with a 'G' so theres no need to check the rest of the chars. */
			/* Parse filename from request */
			prgm.recvbufptr = (char*)&prgm.recvbuf + 5;
			do {
				prgm.recvbufptr++;
			} while(*prgm.recvbufptr != ' ');
			do {
				*prgm.recvbufptr++ = '\0';
			} while(prgm.recvbufptr - (char*)&prgm.recvbuf < 1023);

			/* Open the file */
			asm_open(prgm.file_fd, prgm.recvbuf+5, O_RDONLY, 0644);
			/* Send 404 page if open fails - assuming the 404 file will always exist*/
			if(prgm.file_fd < 0){
				asm_open(prgm.file_fd, "e", O_RDONLY, 0644);
			}

			/* Get file size */
			asm_fstat(prgm.file_fd, &prgm);

			/* Allocate space for file on heap */
			asm_brk(prgm.brk_start, 0);
			asm_brk(prgm.brk_end, prgm.brk_start + (prgm.st_size * sizeof(char)) + 18);

			/* Write HTTP/1.1 200 OK into heap */
			prgm.i = 0;
			while (prgm.i < 18) {
				*(prgm.brk_start+prgm.i) = "HTTP/1.1 200 OK\n\n"[prgm.i++];
			} /* Changing this to a do-while wont save any bytes */

			/* Read file into heap */
			asm_read(prgm.file_bytes_read, prgm.file_fd, prgm.brk_start+17, prgm.st_size);
			/* asm_close(prgm.file_fd); REAL MEN DONT CLOSE FILES HAHAHAHAHA */

			prgm.file_bytes_read += 18;

			/* Send the content in heap to the client */
			asm_sendto(prgm.sendto_res, prgm.client_sock_fd, prgm.brk_start, prgm.file_bytes_read, 0);

			/* Free heap memory */
			asm_brk(prgm.brk_end, prgm.brk_start);

			/* Close the socket 
			 * Be cheeky and actually not close it to save some bytes? ;)
			 * asm_close(prgm.client_sock_fd);
			*/
		}
	}
}
