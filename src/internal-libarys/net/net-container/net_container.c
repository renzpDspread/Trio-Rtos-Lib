/*
********************************************************************************
*
*   File Name:
*       net_container.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _NET_CONTAINER_MODULE
#define _NET_CONTAINER_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "config.h"
#include "net_container.h"
#include "net_container_platform.h"

#include <string.h>
#include <stdlib.h>



/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _socket_priv_t
{
    int fd;
    int sw;
    socket_body_t api;
}socket_priv_t, *psocket_priv_t;

typedef const socket_priv_t     *pcsocket_priv_t;

typedef struct _socketfd_set_priv_t
{
    int count;
    int array[SOCKET_NUM];
}socketfd_set_priv_t, *psocketfd_set_priv_t;

typedef const socketfd_set_priv_t     *pcsocketfd_set_priv_t;

static psocket_hostinfo_t g_phostinfo=NULL;

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
|       nc_socket
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_socket(int domain, int type, int protocol)
{
    #if 1
    psocket_priv_t priv = 0;
    int i = 0;
    int ret = 0;

    priv = net_container_alloc(sizeof(socket_priv_t));
    
    ret = net_container_require(i, protocol, &priv->api);
    if(ret < 0)
    {
        return -1;
    }

    if (priv->api.socket != NULL)
    {
        priv->fd = priv->api.socket(priv->api.priv, domain, type, protocol);
        if (priv->fd < 0)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    if (ret >= 0 && priv->fd >= 0)
    {
        priv->sw = i;
        return (int)priv;
    }

    net_container_free(priv);
    NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
    return -1;
    #else
    psocket_priv_t priv = 0;
    int i = 0;
    int ret = 0;

    priv = net_container_alloc(sizeof(socket_priv_t));
    for(i=SOCKET_SW_START;i<SOCKET_SW_NUMS;i++)
    {
        ret = net_container_require(i, protocol, &priv->api);
        if(ret < 0)
        {
            continue;
        }
        priv->fd = priv->api.socket(priv->api.priv, domain, type, protocol);
        if(priv->fd < 0)
        {
            continue;
        }
        break;
    }
    if(ret >= 0 && priv->fd >= 0)
    {
        priv->sw = i;x
        return (int)priv;
    }
    net_container_free(priv);
    NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
    return -1;
    #endif
}

/*--------------------------------------
|   Function Name:
|       nc_bind
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_bind(int fd, pcsockaddr_in_t addr, int addrlen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.bind == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.bind(priv->api.priv, priv->fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       nc_listen
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_listen(int fd, int backlog)
{
    psocket_priv_t priv = NULL;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.listen == NULL)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.listen(priv->api.priv, priv->fd, backlog);
}

/*--------------------------------------
|   Function Name:
|       nc_accept
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_accept(int fd, pcsockaddr_in_t cliaddr, int* addrlen)
{
    psocket_priv_t priv = NULL;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.accept == NULL)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.accept(priv->api.priv, priv->fd, cliaddr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       nc_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_connect(int fd, pcsockaddr_in_t addr, int addrlen)
{
    psocket_priv_t priv = NULL;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;
    if(priv->api.connect == NULL)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.connect(priv->api.priv, priv->fd, addr, addrlen, g_phostinfo);
}

/*--------------------------------------
|   Function Name:
|       nc_setsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_setsockopt(int fd, int level, int optname, void* optval, int optlen)
{
    psocket_priv_t priv = NULL;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.setsockopt == NULL)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.setsockopt(priv->api.priv, priv->fd, level, optname, optval, optlen);
}

/*--------------------------------------
|   Function Name:
|       nc_getsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_getsockopt(int fd, int level, int optname, void* optval, int optlen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.getsockopt == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.getsockopt(priv->api.priv, priv->fd, level, optname, optval, optlen);
}

/*--------------------------------------
|   Function Name:
|       nc_socketclose
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_socketclose(int fd)
{
    psocket_priv_t priv = 0;
    int ret = -1;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;
    do
    {
        if(priv->api.socketclose == 0)
        {
            break;
        }
        ret = priv->api.socketclose(priv->api.priv, priv->fd);
    }while(0);
    net_container_free(priv);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       fw_errno
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_errno(int fd)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;
    
    if(priv->api.error == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.error(priv->api.priv, priv->fd);
}

/*--------------------------------------
|   Function Name:
|       fw_recvfrom
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_recvfrom(int fd, char* buf, int len, int flags, pcsockaddr_in_t from, int fromlen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.recvfrom == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.recvfrom(priv->api.priv, priv->fd, buf, len, flags, from, fromlen);
}

/*--------------------------------------
|   Function Name:
|       nc_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_recv(int fd, void* buf, int len, int flags)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.recv == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.recv(priv->api.priv, priv->fd, buf, len, flags);
}

/*--------------------------------------
|   Function Name:
|       nc_sendto
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_sendto(int fd, char* buf, int len, int flags, pcsockaddr_in_t to, int tolen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.sendto == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.sendto(priv->api.priv, priv->fd, buf, len, flags, to, tolen);
}

/*--------------------------------------
|   Function Name:
|       nc_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_send(int fd, const void* buf, int len, int flags)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    priv = (psocket_priv_t)fd;

    if(priv->api.send == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    return priv->api.send(priv->api.priv, priv->fd, buf, len, flags);
}

/*--------------------------------------
|   Function Name:
|       nc_select
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_select(psockfd_set_t rd, psockfd_set_t wr, psockfd_set_t ex, int timeoutms)
{
    psocketfd_set_priv_t fd_set = (psocketfd_set_priv_t)rd;
    psocket_priv_t priv = 0;
    sockfd_set_t fd_set_swi[SOCKET_SW_NUMS];
    sockfd_set_t fd_set_fds[SOCKET_SW_NUMS];
    int i = 0;
    int sel = 0;
    int count = 0;
    int waitforever = 0;

    if(timeoutms == 0)
    {
        waitforever = 1;
    }
    for(i=0;i<SOCKET_SW_NUMS;i++)
    {
        nc_fd_zero(&fd_set_swi[i]);
        nc_fd_zero(&fd_set_fds[i]);
    }
    for(i=0;i<fd_set->count;i++)
    {
        priv = (psocket_priv_t)fd_set->array[i];
        nc_fd_set((int)priv, &fd_set_swi[priv->sw]);
        nc_fd_set((int)priv->fd, &fd_set_fds[priv->sw]);
    }
    count = 0;
    while(waitforever || timeoutms > 0)
    {
        for(i=0;i<SOCKET_SW_NUMS;i++)
        {
            fd_set = (psocketfd_set_priv_t)&fd_set_swi[i];
            if(fd_set->count > 0)
            {
                priv = (psocket_priv_t)fd_set->array[0];
                sel = priv->api.select(priv->api.priv, (psockfd_set_t)&fd_set_fds[i], 0, 0, 10);
                if(sel < 0)
                {
                    NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
                    return sel;
                }
                else
                {
                    count += sel;
                }
                timeoutms -= 10;
            }
        }
        if(count > 0)
        {
            break;
        }
    }
    return count;
}

/*--------------------------------------
|   Function Name:
|       nc_fd_zero
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_fd_zero(psockfd_set_t pset)
{
    memset(pset, 0x00, sizeof(sockfd_set_t));
    return 0;
}

/*--------------------------------------
|   Function Name:
|       nc_fd_clr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_fd_clr(int fd, psockfd_set_t pset)
{
    psocketfd_set_priv_t fd_set = (psocketfd_set_priv_t)pset;
    int i = 0;

    for(i=0;i<fd_set->count;i++)
    {
        if((int)fd == fd_set->array[i])
        {
            for(;i<fd_set->count;i++)
            {
                fd_set->array[i] = fd_set->array[i + 1];
            }
            fd_set->count--;
            return 0;
        }
    }
    return -1;
}

/*--------------------------------------
|   Function Name:
|       nc_fd_set
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_fd_set(int fd, psockfd_set_t pset)
{
    psocketfd_set_priv_t fd_set = (psocketfd_set_priv_t)pset;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d fd = %d\r\n", __FUNCTION__, __LINE__, fd);
        return -1;
    }

    if(fd_set->count >= SOCKET_NUM)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    fd_set->array[fd_set->count++] = (int)fd;
    return 0;
}

/*--------------------------------------
|   Function Name:
|       nc_fd_isset
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_fd_isset(int fd, psockfd_set_t pset)
{
    psocketfd_set_priv_t fd_set = (psocketfd_set_priv_t)pset;
    int i = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    for(i=0;i<fd_set->count;i++)
    {
        if((int)fd == fd_set->array[i])
        {
            return 1;
        }
    }
    return 0;
}

/*--------------------------------------
|   Function Name:
|       nc_getpeername
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_getpeername(int fd, pcsockaddr_in_t addr, int* addrlen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.getpeername == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return 0;
    }
    return priv->api.getpeername(priv->api.priv, priv->fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       nc_getsockname
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_getsockname(int fd, pcsockaddr_in_t addr, int* addrlen)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    if(priv->api.getsockname == 0)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return 0;
    }
    return priv->api.getsockname(priv->api.priv, priv->fd, addr, addrlen);
}

/*--------------------------------------
|   Function Name:
|       nc_shutdown
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_shutdown(int how)
{
    NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
    return -1;
}


/*--------------------------------------
|   Function Name:
|       nc_sethostinfo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int nc_sethostinfo(char* host, char* port)
{
    int len = strlen(host) + 1;
    if (!g_phostinfo)
        g_phostinfo = net_container_alloc(sizeof(socket_hostinfo_t));

    if (!g_phostinfo)
         return -1;
        
    if (!g_phostinfo->host)
    {
        net_container_free(g_phostinfo->host);
    }

    g_phostinfo->host = net_container_alloc(len);
    if (!g_phostinfo->host)
        return -1;
    
    memset(g_phostinfo->host, 0, len);
    memcpy(g_phostinfo->host, host, strlen(host));
    g_phostinfo->host_port = atoi(port);

    return 0;
}


psocket_priv_t g_priv = NULL;
/*--------------------------------------
|   Function Name:
|       nc_getaddrinfo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_getaddrinfo(char* node, char* service, pcaddrinfo_t hints, paddrinfo_t* res)
{
    psocket_priv_t priv = NULL;
    int ret = 0;

    memset((char *)priv, 0, sizeof(socket_priv_t));

    priv = net_container_alloc(sizeof(socket_priv_t));
    ret = net_container_require(0, PROTOCOL_DEFAULT, &priv->api);
    
    if (ret >= 0 && priv->api.getaddrinfo)
    {
        TRACE_DELAY(2500, "");
        ret = priv->api.getaddrinfo(priv->api.priv, node, service, hints, res);
    }
    else
    {
        ret = nc_sethostinfo(node, service);
    }
    //TRACE_DELAY(2500, "");

    net_container_free(priv);

    if (ret < 0)
    {
        return -1;
    }    

    return ret;
}

/*--------------------------------------
|   Function Name:
|       nc_freeaddrinfo
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_freeaddrinfo(paddrinfo_t res)
{
    paddrinfo_t this = 0;
    paddrinfo_t next = 0;

    next = res;
    while(next != 0)
    {
        this = next;
        next = next->ai_next;
        if(this->ai_addr != 0)
        {
            net_container_free(this->ai_addr);
        }
        net_container_free(this);
    }
    return 0;
}

/*--------------------------------------
|   Function Name:
|       nc_gai_strerror
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
char* nc_gai_strerror(int errcode)
{
    return "error";
}

/*--------------------------------------
|   Function Name:
|       nc_ifd
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int nc_ifd(int fd)
{
    psocket_priv_t priv = 0;

    if(fd < 0 && fd > -65536)
    {
        NET_CONTAINER_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    priv = (psocket_priv_t)fd;

    return priv->fd;
}
