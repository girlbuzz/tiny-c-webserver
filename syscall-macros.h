/*
    "a": %rax
    "b": %rbx
    "c": %rcx
    "d": %rdx
    "D": %rdi
    "S": %rsi
    "d": %rdx
*/

#define asm_read(fd, buf, size) asm volatile("xor %%rax, %%rax;" \
                                             "syscall" \
                                             : \
                                             :"D"(fd),"S"(buf),"d"(size) \
) // rax == 0 == read syscall (we save a couple bytes using xor ;) ) // rdi == fd // rsi == buf ptr // rdx == buf size

#define asm_write(fd, buf, size) asm volatile("syscall" \
                                              : \
                                              :"a"(1),"D"(fd),"S"(buf),"d"(size) \
) // rax == 1 == write syscall // rdi == 1 == stdout fd // rsi == buf ptr // rdx == buf size

#define asm_open(fd, fn, flags, mode) asm volatile("syscall" \
                                                   :"=r"(fd) \
                                                   :"a"(2),"D"(fn),"S"(flags),"d"(mode) \
) // rax == 2 == open syscall // rdi == filename ptr // rsi == flags // rdx == mode

#define asm_close(fd) asm volatile("syscall" \
                                   : \
                                   :"a"(3), "D"(fd) \
) // rax == 3 == close syscall // rdi == fd

#define asm_brk(res, brk) asm volatile("syscall" \
                                       : "=a" (res) \
                                       : "a"(12),"D"(brk) \
)

#define asm_socket(sockfd, domain, type, protocol) asm volatile("syscall" \
                                                                :"=r"(sockfd) \
                                                                :"a"(41),"D"(domain),"S"(type),"d"(protocol) \
) // rax == 41 == socket syscall // rdi == domain // rsi == type // rdx == protocol


// Disregard addr, dont need the data, is nullable
#define asm_accept(resfd, sockfd) asm volatile("xor %%rsi, %%rsi;" \
                                               "xor %%rdx, %%rdx;" \
                                               "syscall" \
                                               :"=r"(resfd) \
                                               :"a"(43),"D"(sockfd) \
)

#define asm_sendto(res, sockfd, buf, buflen, flags) asm volatile("syscall" \
                                                                 :"=r"(res) \
                                                                 :"a"(44),"D"(sockfd),"S"(buf),"d"(buflen),"r"(flags) \
)

// Disregard src addr, is nullable
#define asm_recvfrom(res, sockfd, buf, buflen, flags) asm volatile("syscall" \
                                                                   :"=r"(res) \
                                                                   :"a"(45),"D"(sockfd),"S"(buf),"d"(buflen),"r"(flags) \
)

#define asm_bind(res, sockfd, addr, addrlen) asm volatile("syscall" \
                                                          :"=r"(res) \
                                                          :"a"(49),"D"(sockfd),"S"(addr),"d"(addrlen) \
)

#define asm_listen(sockfd, max_connections) asm volatile("syscall" \
                                                         : \
                                                         :"a"(50),"D"(sockfd),"S"(max_connections) \
)

#define asm_exit() asm volatile("xor %%rdi,%%rdi;" \
                                "syscall" \
                                : \
                                :"a"(60) \
) // rax == 60 == exit syscall // rdi == 0 == no err exit code
