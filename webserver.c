#include "syscall-macros.h"

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

/*
    This is really not very portable, will likely break across different systems!
*/
struct stat{
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
};

#define AF_INET     2	/* Internet IP Protocol 	*/
#define SOCK_STREAM 1   /* stream (connection) socket	*/
#define INADDR_ANY  0
#define O_RDONLY    0

#define PORT 8080

void _start(){
    /* Create socket */
    int server_sock_fd;
    asm_socket(server_sock_fd, AF_INET, SOCK_STREAM, 0);
    
    /* Config */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = ((PORT & 0xFF) << 8) | ((PORT & 0xFF00) >> 8); /* Swap the 1st and 2nd byte of port */

    /* Bind sock to port */
    int bind_res;
    asm_bind(bind_res, server_sock_fd, &server_addr, sizeof(server_addr));

    /* Listen for connections */
    asm_listen(server_sock_fd, 16);

    while(1){
        /* Accept new connection */
        int client_sock_fd;
        asm_accept(client_sock_fd, server_sock_fd);

        /* Receive their request */
        int bytes_received;
        char recvbuf[1024]; /* Everything past the first 1024 bytes is ignored*/
        asm_recvfrom(bytes_received, client_sock_fd, recvbuf, 1024, 0);

        /* Check if GET */
        if (recvbuf[0] == 'G') { /* This is the most minimal way to possibly check, no other method starts with a 'G' so theres no need to check the rest of the chars. */
            /* Parse filename from request */
            char filename[1024]; /* No point going bigger since recvbuf has a size of 1024 too */
            int fn_iter = 5;
            while(recvbuf[fn_iter] != ' ' && fn_iter < bytes_received){
                filename[fn_iter - 5] = recvbuf[fn_iter++];
            }
            filename[fn_iter - 5] = '\0';

            /* Open the file */
            int file_fd;
            asm_open(file_fd, filename, O_RDONLY, 0644);
            /* Send 404 page if open fails - assuming the 404 file will always exist*/
            if(file_fd < 0){
                asm_open(file_fd, "404", O_RDONLY, 0644);
            }

            /* Get file size */
            struct stat stat_res;
            asm_fstat(file_fd, &stat_res);

            /* Allocate space for file on heap */
            char* brk_start;
            asm_brk(brk_start, 0);
            char* brk_end;
            asm_brk(brk_end, brk_start + (stat_res.st_size * sizeof(char)) + 18);

            /* Write HTTP/1.1 200 OK into heap */
            const char* ok = "HTTP/1.1 200 OK\n\n";
            int i = 0;
            while(i<18){
                *(brk_start+i) = ok[i++];
            }

            /* Read file into heap */
            int file_bytes_read;
            asm_read(file_bytes_read, file_fd, brk_start+17, stat_res.st_size);
            asm_close(file_fd);

            file_bytes_read += 18;

            /* Send the content in heap to the client */
            int sendto_res;
            asm_sendto(sendto_res, client_sock_fd, brk_start, file_bytes_read, 0);
            
            /* Free heap memory */
            asm_brk(brk_end, brk_start);
        }

        /* Close the socket */
        asm_close(client_sock_fd);
    }
}
