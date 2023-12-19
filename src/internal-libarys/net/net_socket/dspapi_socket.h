/*
********************************************************************************
*
*   File Name:
*       dspapi_socket.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _DSPAPI_SOCKET_H
#define _DSPAPI_SOCKET_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _DSPAPI_SOCKET_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define DSP_DOMAIN_AF_UNSPEC        (0)
#define DSP_DOMAIN_AF_INET          (2)
#define DSP_DOMAIN_AF_INET6         (3)
#define DSP_DOMAIN_AF_INET_DUAL46   (4)
#define DSP_DOMAIN_AF_NONIP         (11)

#define DSP_TYPE_SOCK_STREAM        (1)
#define DSP_TYPE_SOCK_DGRAM         (2)
#define DSP_TYPE_SOCK_RAW           (3)
#define DSP_TYPE_SOCK_NONIP         (4)

#define DSP_PROTOCOL_DEFAULT        (0)
#define DSP_PROTOCOL_TCP            (1)
#define DSP_PROTOCOL_UDP            (2)
#define DSP_PROTOCOL_TLS            (3)

#define DSP_SHUTDOWN_RD             (0)
#define DSP_SHUTDOWN_WR             (1)
#define DSP_SHUTDOWN_RDWR           (2)

#define DSP_SOCK_MSG_DONTWAIT       (0x20)



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

typedef const sockaddr_in_t     *pcsockaddr_in_t;

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



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL int dsp_socket(int domain, int type, int protocol);
GLOBAL int dsp_bind(int fd, pcsockaddr_in_t addr, int addrlen);
GLOBAL int dsp_listen(int fd, int backlog);
GLOBAL int dsp_accept(int fd, pcsockaddr_in_t cliaddr, int* addrlen);
GLOBAL int dsp_connect(int fd, pcsockaddr_in_t addr, int addrlen);
GLOBAL int dsp_setsockopt(int fd, int level, int optname, void* optval, int optlen);
GLOBAL int dsp_getsockopt(int fd, int level, int optname, void* optval, int optlen);
GLOBAL int dsp_socketclose(int fd);
GLOBAL int dsp_errno(int fd);
GLOBAL int dsp_recvfrom(int fd, char* buf, int len, int flags, pcsockaddr_in_t from, int fromlen);
GLOBAL int dsp_recv(int fd, void* buf, int len, int flags);
GLOBAL int dsp_sendto(int fd, char* buf, int len, int flags, pcsockaddr_in_t to, int tolen);
GLOBAL int dsp_send(int fd, const void* buf, int len, int flags);
GLOBAL int dsp_select(psockfd_set_t rd, psockfd_set_t wr, psockfd_set_t ex, int timeoutms);
GLOBAL int dsp_fd_zero(psockfd_set_t pset);
GLOBAL int dsp_fd_clr(int fd, psockfd_set_t pset);
GLOBAL int dsp_fd_set(int fd, psockfd_set_t pset);
GLOBAL int dsp_fd_isset(int fd, psockfd_set_t pset);
GLOBAL int dsp_getpeername(int fd, pcsockaddr_in_t addr, int* addrlen);
GLOBAL int dsp_getsockname(int fd, pcsockaddr_in_t addr, int* addrlen);
GLOBAL int dsp_shutdown(int how);
GLOBAL int dsp_getaddrinfo(char* node, char* service, pcaddrinfo_t hints, paddrinfo_t* res);
GLOBAL int dsp_freeaddrinfo(paddrinfo_t res);
GLOBAL char* dsp_gai_strerror(int errcode);

GLOBAL int dsp_ifd(int fd);

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

#endif  /* #ifndef _DSPAPI_SOCKET_H */
