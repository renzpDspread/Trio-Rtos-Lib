/*
********************************************************************************
*
*   File Name:
*       dspapi_socket.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _DSPAPI_SOCKET_MODULE
#define _DSPAPI_SOCKET_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "net_container.h"



/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Functions
+----------------------------------------------------------------------------*/
/*--------------------------------------
|   Function Name:
|       dsp_socket
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_socket(int domain, int type, int protocol)
{
    return nc_socket(domain, type, protocol);
}

/*--------------------------------------
|   Function Name:
|       dsp_bind
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_bind(int fd, pcsockaddr_in_t addr, int addrlen)
{
    return nc_bind(fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_listen
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_listen(int fd, int backlog)
{
    return nc_listen(fd, backlog);
}

/*--------------------------------------
|   Function Name:
|       dsp_accept
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_accept(int fd, pcsockaddr_in_t cliaddr, int* addrlen)
{
    return nc_accept(fd, cliaddr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_connect(int fd, pcsockaddr_in_t addr, int addrlen)
{
    return nc_connect(fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_setsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_setsockopt(int fd, int level, int optname, void* optval, int optlen)
{
    return nc_setsockopt(fd, level, optname, optval, optlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_getsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_getsockopt(int fd, int level, int optname, void* optval, int optlen)
{
    return nc_getsockopt(fd, level, optname, optval, optlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_socketclose
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_socketclose(int fd)
{
    return nc_socketclose(fd);
}

/*--------------------------------------
|   Function Name:
|       dsp_errno
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_errno(int fd)
{
    return nc_errno(fd);
}

/*--------------------------------------
|   Function Name:
|       dsp_recvfrom
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_recvfrom(int fd, char* buf, int len, int flags, pcsockaddr_in_t from, int fromlen)
{
    return nc_recvfrom(fd, buf, len, flags, from, fromlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_recv(int fd, void* buf, int len, int flags)
{
    return nc_recv(fd, buf, len, flags);
}

/*--------------------------------------
|   Function Name:
|       dsp_sendto
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_sendto(int fd, char* buf, int len, int flags, pcsockaddr_in_t to, int tolen)
{
    return nc_sendto(fd, buf, len, flags, to, tolen);
}

/*--------------------------------------
|   Function Name:
|       dsp_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_send(int fd, const void* buf, int len, int flags)
{
    return nc_send(fd, buf, len, flags);
}

/*--------------------------------------
|   Function Name:
|       dsp_select
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_select(psockfd_set_t rd, psockfd_set_t wr, psockfd_set_t ex, int timeoutms)
{
    return nc_select(rd, wr, ex, timeoutms);
}

/*--------------------------------------
|   Function Name:
|       dsp_fd_zero
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_fd_zero(psockfd_set_t pset)
{
    return nc_fd_zero(pset);
}

/*--------------------------------------
|   Function Name:
|       dsp_fd_clr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_fd_clr(int fd, psockfd_set_t pset)
{
    return nc_fd_clr(fd, pset);
}

/*--------------------------------------
|   Function Name:
|       dsp_fd_set
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_fd_set(int fd, psockfd_set_t pset)
{
    return nc_fd_set(fd, pset);
}

/*--------------------------------------
|   Function Name:
|       dsp_fd_isset
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_fd_isset(int fd, psockfd_set_t pset)
{
    return nc_fd_isset(fd, pset);
}

/*--------------------------------------
|   Function Name:
|       dsp_getpeername
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_getpeername(int fd, pcsockaddr_in_t addr, int* addrlen)
{
    return nc_getpeername(fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_getsockname
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_getsockname(int fd, pcsockaddr_in_t addr, int* addrlen)
{
    return nc_getsockname(fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       dsp_shutdown
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_shutdown(int how)
{
    return nc_shutdown(how);
}

/*--------------------------------------
|   Function Name:
|       dsp_getaddrinfo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_getaddrinfo(char* node, char* service, pcaddrinfo_t hints, paddrinfo_t* res)
{
    return nc_getaddrinfo(node, service, hints, res);
}

/*--------------------------------------
|   Function Name:
|       dsp_freeaddrinfo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_freeaddrinfo(paddrinfo_t res)
{
    return nc_freeaddrinfo(res);
}

/*--------------------------------------
|   Function Name:
|       dsp_gai_strerror
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
char* dsp_gai_strerror(int errcode)
{
    return nc_gai_strerror(errcode);
}

/*--------------------------------------
|   Function Name:
|       dsp_ifd
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_ifd(int fd)
{
    return nc_ifd(fd);
}
