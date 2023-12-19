/*
********************************************************************************
*
*   File Name:
*       mbedtls_socket.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _MBEDTLS_SOCKET_H
#define _MBEDTLS_SOCKET_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _MBEDTLS_SOCKET_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define MBEDTLS_DOMAIN_AF_UNSPEC        (0)
#define MBEDTLS_DOMAIN_AF_INET          (2)
#define MBEDTLS_DOMAIN_AF_INET6         (3)
#define MBEDTLS_DOMAIN_AF_INET_DUAL46   (4)
#define MBEDTLS_DOMAIN_AF_NONIP         (11)

#define MBEDTLS_TYPE_SOCK_STREAM        (1)
#define MBEDTLS_TYPE_SOCK_DGRAM         (2)
#define MBEDTLS_TYPE_SOCK_RAW           (3)
#define MBEDTLS_TYPE_SOCK_NONIP         (4)

#define MBEDTLS_PROTOCOL_DEFAULT        (0)
#define MBEDTLS_PROTOCOL_TCP            (1)
#define MBEDTLS_PROTOCOL_UDP            (2)
#define MBEDTLS_PROTOCOL_TLS            (3)

#define MBEDTLS_SHUTDOWN_RD             (0)
#define MBEDTLS_SHUTDOWN_WR             (1)
#define MBEDTLS_SHUTDOWN_RDWR           (2)

#define MBEDTLS_SOCK_MSG_DONTWAIT       (0x20)



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/
#if 0    
enum
{
    SSL_VERIFY_START = 0,

    SSL_VERIFY_NONE = SSL_VERIFY_START,
    SSL_VERIFY_OPTIONAL,
    SSL_VERIFY_REQUIRED,
    SSL_VERIFY_UNSET,

    SSL_VERIFY_NUMS
};
#endif


/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _mbedtls_sock_in_addr_t
{
    unsigned int     s_addr;
}mbedtls_sock_in_addr_t, *pmbedtls_sock_in_addr_t;

typedef const mbedtls_sock_in_addr_t     *pcmbedtls_sock_in_addr_t;

typedef struct _mbedtls_sockaddr_in_t
{
    unsigned short sin_family;
    unsigned short sin_port;
    mbedtls_sock_in_addr_t sin_addr;
    char sin_zero[8];
}mbedtls_sockaddr_in_t, *pmbedtls_sockaddr_in_t;

typedef const mbedtls_sockaddr_in_t *pcmbedtls_sockaddr_in_t;

typedef struct _mbedtls_hostinfo_t
{
    unsigned short host_port;
    unsigned char *host;
}mbedtls_hostinfo_t, *pmbedtls_hostinfo_t;

typedef const mbedtls_hostinfo_t     *pcmbedtls_hostinfo_t;

typedef struct _mbedtls_sockfd_set_t
{
    unsigned int fds_bits[32];
}mbedtls_sockfd_set_t, *pmbedtls_sockfd_set_t;

typedef const mbedtls_sockfd_set_t     *pcmbedtls_sockfd_set_t;

typedef struct _mbedtls_addrinfo_t
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    int ai_addrlen;
    pmbedtls_sockaddr_in_t ai_addr;
    char* ai_canonname;
    struct _mbedtls_addrinfo_t *ai_next;
}mbedtls_addrinfo_t, *pmbedtls_addrinfo_t;

typedef const mbedtls_addrinfo_t     *pcmbedtls_addrinfo_t;

typedef struct _mbedtls_socket_body_t
{
    void* priv;
    
    int (*socket)(void* priv, int domain, int type, int protocol);
    int (*bind)(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int addrlen);
    int (*listen)(void* priv, int fd, int backlog);
    int (*accept)(void* priv, int fd, pcmbedtls_sockaddr_in_t cliaddr, int* addrlen);
    int (*connect)(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int addrlen, pmbedtls_hostinfo_t phostinfo);
    int (*setsockopt)(void* priv, int fd, int level, int optname, void* optval, int optlen);
    int (*getsockopt)(void* priv, int fd, int level, int optname, void* optval, int optlen);
    int (*socketclose)(void* priv, int fd);
    int (*error)(void* priv, int fd);
    int (*recvfrom)(void* priv, int fd, char* buf, int len, int flags, pcmbedtls_sockaddr_in_t from, int fromlen);
    int (*recv)(void* priv, int fd, void* buf, int len, int flags);
    int (*sendto)(void* priv, int fd, char* buf, int len, int flags, pcmbedtls_sockaddr_in_t to, int tolen);
    int (*send)(void* priv, int fd, const void* buf, int len, int flags);
    int (*select)(void* priv, pmbedtls_sockfd_set_t rd, pmbedtls_sockfd_set_t wr, pmbedtls_sockfd_set_t ex, int timeoutms);
    int (*fd_zero)(void* priv, pmbedtls_sockfd_set_t pset);
    int (*fd_clr)(void* priv, int fd, pmbedtls_sockfd_set_t pset);
    int (*fd_Set)(void* priv, int fd, pmbedtls_sockfd_set_t pset);
    int (*fd_isset)(void* priv, int fd, pmbedtls_sockfd_set_t pset);
    int (*getpeername)(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int* addrlen);
    int (*getsockname)(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int* addrlen);
    int (*shutdown)(void* priv, int how);
    int (*getaddrinfo)(void* priv, char* node, char* service, pcmbedtls_addrinfo_t hints, pmbedtls_addrinfo_t* res);
    int (*freeaddrinfo)(pmbedtls_addrinfo_t res);
    char* (*gai_strerror)(int errcode);
} mbedtls_socket_body_t, *pmbedtls_socket_body_t;

typedef const mbedtls_socket_body_t     *pcmbedtls_socket_body_t;

/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL int mbedtls_init(void);
GLOBAL int mbedtls_uninit(void);
GLOBAL int mbedtls_cacert(const unsigned char* pdata, int len);
GLOBAL int mbedtls_clikey(const unsigned char* pdata, int len);
GLOBAL int mbedtls_clicert(const unsigned char* pdata, int len);
GLOBAL int mbedtls_vfymode(int fd, int mode);
GLOBAL int mbedtls_timeout(int fd, int timeout);
GLOBAL int mbedtls_hostname(int fd, const char* phostname);
GLOBAL int mbedtls_socket_regist(pmbedtls_socket_body_t pdrv);

GLOBAL int mbedtls_socket(void *priv, int domain, int type, int protocol);
GLOBAL int mbedtls_connect(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int addrlen, pmbedtls_hostinfo_t phostinfo);
GLOBAL int mbedtls_socketclose(void* priv, int fd);
GLOBAL int mbedtls_recv(void* priv, int fd, void* buf, int len, int flags);
GLOBAL int mbedtls_send(void* priv, int fd, const void* buf, int len, int flags);
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

#endif  /* #ifndef _MBEDTLS_SOCKET_H */
