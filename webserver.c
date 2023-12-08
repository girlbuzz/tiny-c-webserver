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

#define AF_INET     2	/* Internet IP Protocol 	*/
#define SOCK_STREAM 1   /* stream (connection) socket	*/
#define INADDR_ANY  0
#define O_RDONLY    0

#define PORT 8088

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
    asm_bind(bind_res, server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    /* Listen for connections */
    asm_listen(server_sock_fd, 16);

    while(1){
        /* Accept new connection */
        int client_sock_fd;
        asm_accept(client_sock_fd, server_sock_fd);

        /* TODO - use fork to multithread */

        /* Receive their request */
        int bytes_received;
        char recvbuf[1024];
        asm_recvfrom(bytes_received, client_sock_fd, recvbuf, 1024, 0);

        /* Check if GET */
        if (recvbuf[0] == 'G' && recvbuf[1] == 'E' && recvbuf[2] == 'T') {
            /* Parse filename from request */
            char filename[1024]; /* 1024 should be enough, but 4096 would be 100% enough, as its the path size limit in linux */
            int fn_iter;
            for (fn_iter = 5; recvbuf[fn_iter] != ' ' && fn_iter < bytes_received; fn_iter++) {
                filename[fn_iter - 5] = recvbuf[fn_iter];
            }
            filename[fn_iter - 5] = '\0'; // Null-terminate the filename

            /* Open the file */
            int file_fd;
            asm_open(file_fd, filename, O_RDONLY, 0644);
            /* Send 404 page if open fails - assuming the 404.html file will always exist*/
            if(file_fd < 0){
                asm_open(file_fd, "404.html", O_RDONLY, 0644);
            }

            /* Read file content - TODO use heap for large files */
            char file_buf[1024];
            int file_bytes_read;
            asm_read(file_bytes_read, file_fd, file_buf, sizeof(file_buf));
            asm_close(file_fd);

            /* Send the file content to the client */
            int sendto_res;
            asm_sendto(sendto_res, client_sock_fd, file_buf, file_bytes_read, 0);
        }

        /* Close the socket */
        asm_close(client_sock_fd);
    }
}
