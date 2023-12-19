/*
********************************************************************************
*
*   File Name:
*       net_container.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _NET_CONTAINER_H
#define _NET_CONTAINER_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _NET_CONTAINER_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define DOMAIN_AF_UNSPEC        (0)
#define DOMAIN_AF_INET          (2)
#define DOMAIN_AF_INET6         (3)
#define DOMAIN_AF_INET_DUAL46   (4)
#define DOMAIN_AF_NONIP         (11)

#define TYPE_SOCK_STREAM        (1)
#define TYPE_SOCK_DGRAM         (2)
#define TYPE_SOCK_RAW           (3)
#define TYPE_SOCK_NONIP         (4)

#define PROTOCOL_DEFAULT        (0)
#define PROTOCOL_TCP            (1)
#define PROTOCOL_UDP            (2)
#define PROTOCOL_TLS            (3)

#define SHUTDOWN_RD             (0)
#define SHUTDOWN_WR             (1)
#define SHUTDOWN_RDWR           (2)

#define SOCK_MSG_DONTWAIT       (0x20)



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _sock_in_addr_t
{
    unsigned int     s_addr;
}sock_in_addr_t, *psock_in_addr_t;

typedef const sock_in_addr_t     *pcsock_in_addr_t;

typedef struct _sockaddr_in_t
{
    unsigned short sin_family;
    unsigned short sin_port;
    sock_in_addr_t sin_addr;
    char sin_zero[8];
}sockaddr_in_t, *psockaddr_in_t;

typedef const sockaddr_in_t *pcsockaddr_in_t;


typedef struct _socket_hostinfo_t
{
    unsigned short host_port;
    unsigned char *host;
}socket_hostinfo_t, *psocket_hostinfo_t;

typedef const socket_hostinfo_t *pcsocket_hostinfo_t;


typedef struct _sockfd_set_t
{
    unsigned int fds_bits[32];
}sockfd_set_t, *psockfd_set_t;

typedef const sockfd_set_t     *pcsockfd_set_t;

typedef struct _addrinfo_t
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    int ai_addrlen;
    psockaddr_in_t ai_addr;
    char* ai_canonname;
    struct _addrinfo_t *ai_next;
}addrinfo_t, *paddrinfo_t;

typedef const addrinfo_t     *pcaddrinfo_t;

typedef struct _socket_body_t
{
    void* priv;
    
    int (*socket)(void* priv, int domain, int type, int protocol);
    int (*bind)(void* priv, int fd, pcsockaddr_in_t addr, int addrlen);
    int (*listen)(void* priv, int fd, int backlog);
    int (*accept)(void* priv, int fd, pcsockaddr_in_t cliaddr, int* addrlen);
    int (*connect)(void *priv, int fd, pcsockaddr_in_t addr, int addrlen, psocket_hostinfo_t phostinfo);
    int (*setsockopt)(void *priv, int fd, int level, int optname, void *optval, int optlen);
    int (*getsockopt)(void* priv, int fd, int level, int optname, void* optval, int optlen);
    int (*socketclose)(void* priv, int fd);
    int (*error)(void* priv, int fd);
    int (*recvfrom)(void* priv, int fd, char* buf, int len, int flags, pcsockaddr_in_t from, int fromlen);
    int (*recv)(void* priv, int fd, void* buf, int len, int flags);
    int (*sendto)(void* priv, int fd, char* buf, int len, int flags, pcsockaddr_in_t to, int tolen);
    int (*send)(void* priv, int fd, const void* buf, int len, int flags);
    int (*select)(void* priv, psockfd_set_t rd, psockfd_set_t wr, psockfd_set_t ex, int timeoutms);
    int (*fd_zero)(void* priv, psockfd_set_t pset);
    int (*fd_clr)(void* priv, int fd, psockfd_set_t pset);
    int (*fd_set)(void* priv, int fd, psockfd_set_t pset);
    int (*fd_isset)(void* priv, int fd, psockfd_set_t pset);
    int (*getpeername)(void* priv, int fd, pcsockaddr_in_t addr, int* addrlen);
    int (*getsockname)(void* priv, int fd, pcsockaddr_in_t addr, int* addrlen);
    int (*shutdown)(void* priv, int how);
    int (*getaddrinfo)(void* priv, char* node, char* service, pcaddrinfo_t hints, paddrinfo_t* res);
    int (*freeaddrinfo)(paddrinfo_t res);
    char* (*gai_strerror)(int errcode);
} socket_body_t, *psocket_body_t;

typedef const socket_body_t     *pcsocket_body_t;



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL int nc_socket(int domain, int type, int protocol);
GLOBAL int nc_bind(int fd, pcsockaddr_in_t addr, int addrlen);
GLOBAL int nc_listen(int fd, int backlog);
GLOBAL int nc_accept(int fd, pcsockaddr_in_t cliaddr, int* addrlen);
GLOBAL int nc_connect(int fd, pcsockaddr_in_t addr, int addrlen);
GLOBAL int nc_setsockopt(int fd, int level, int optname, void* optval, int optlen);
GLOBAL int nc_getsockopt(int fd, int level, int optname, void* optval, int optlen);
GLOBAL int nc_socketclose(int fd);
GLOBAL int nc_errno(int fd);
GLOBAL int nc_recvfrom(int fd, char* buf, int len, int flags, pcsockaddr_in_t from, int fromlen);
GLOBAL int nc_recv(int fd, void* buf, int len, int flags);
GLOBAL int nc_sendto(int fd, char* buf, int len, int flags, pcsockaddr_in_t to, int tolen);
GLOBAL int nc_send(int fd, const void* buf, int len, int flags);
GLOBAL int nc_select(psockfd_set_t rd, psockfd_set_t wr, psockfd_set_t ex, int timeoutms);
GLOBAL int nc_fd_zero(psockfd_set_t pset);
GLOBAL int nc_fd_clr(int fd, psockfd_set_t pset);
GLOBAL int nc_fd_set(int fd, psockfd_set_t pset);
GLOBAL int nc_fd_isset(int fd, psockfd_set_t pset);
GLOBAL int nc_getpeername(int fd, pcsockaddr_in_t addr, int* addrlen);
GLOBAL int nc_getsockname(int fd, pcsockaddr_in_t addr, int* addrlen);
GLOBAL int nc_shutdown(int how);
GLOBAL int nc_getaddrinfo(char* node, char* service, pcaddrinfo_t hints, paddrinfo_t* res);
GLOBAL int nc_freeaddrinfo(paddrinfo_t res);
GLOBAL char* nc_gai_strerror(int errcode);

GLOBAL int nc_ifd(int fd);

/*
*******************************************************************************
*   End of File
*******************************************************************************
*/

#undef GLOBAL

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif  /* #ifndef _NET_CONTAINER_H */
