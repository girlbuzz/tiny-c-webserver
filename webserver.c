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

#define AF_INET		2	/* Internet IP Protocol 	*/
#define SOCK_STREAM	1   /* stream (connection) socket	*/
#define	INADDR_ANY  ((unsigned long int) 0x00000000)

#define O_RDONLY 00000000

/*
unsigned short htons(unsigned short hostshort) {
    // Check the endianness of the system
    // If it's little-endian, swap the bytes
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((hostshort & 0xFF) << 8) | ((hostshort & 0xFF00) >> 8);
    #else
        return hostshort;
    #endif
}
*/

#define PORT 8088

void _start(){
    /* Create sock */
    int server_sock_fd;
    asm_socket(server_sock_fd, AF_INET, SOCK_STREAM, 0);
    
    /* Config */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = ((PORT & 0xFF) << 8) | ((PORT & 0xFF00) >> 8);

    /* Bind sock to port */
    int bind_res;
    asm_bind(bind_res, server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(bind_res != 0){
        asm_exit();
    }

    /* Listen for connections */
    asm_listen(server_sock_fd, 16);

    while(1){
        /* Accept new connection */
        int client_sock_fd;
        asm_accept(client_sock_fd, server_sock_fd);
        if(client_sock_fd < 0){
            asm_exit();
        }

        /* Receive their request */
        int bytes_received = -1;
        while(bytes_received != 0){
            char recvbuf[256];
            asm_recvfrom(bytes_received, client_sock_fd, recvbuf, 256, 0);
            asm_write(1, recvbuf, 256);

            /* Check if request is GET */
            if(recvbuf[0] == 'G' && recvbuf[1] == 'E' && recvbuf[2] == 'T'){

                /* is GET, reply with contents of index.html file */
                int index_fd;
                asm_open(index_fd, "index.html", O_RDONLY, 0644);

                char indexbuf[1024];
                asm_read(index_fd, indexbuf, 1024);

                int sendto_res;
                asm_sendto(sendto_res, client_sock_fd, indexbuf, 1024, 0);

                if(sendto_res <= 0){
                    asm_exit();
                }
            }
        }

        asm_close(client_sock_fd);
    }
}