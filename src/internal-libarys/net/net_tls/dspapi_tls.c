/*
********************************************************************************
*
*   File Name:
*       dspapi_tls.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _DSPAPI_TLS_MODULE
#define _DSPAPI_TLS_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "config.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "mbedtls_socket.h"
#include "dspapi_tls.h"
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
typedef struct _tls_socket_priv_t
{
    int fd;
    int sw;
    mbedtls_socket_body_t api;
}tls_socket_priv_t, *ptls_socket_priv_t;


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
|       dsp_tls_cacert
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_init(void)
{
    int ret = -1;

    ret = mbedtls_init();

    return ret;
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_cacert
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_cacert(const unsigned char* pdata, int len)
{
    return mbedtls_cacert(pdata, len);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_clikey
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_clikey(const unsigned char* pdata, int len)
{
    return mbedtls_clikey(pdata, len);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_clicert
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_clicert(const unsigned char* pdata, int len)
{
    return mbedtls_clicert(pdata, len);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_vfymode
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_vfymode(int fd, int mode)
{
    ptls_socket_priv_t priv_tls = (ptls_socket_priv_t)fd;

    return mbedtls_vfymode(priv_tls->fd, mode);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_timeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_timeout(int fd, int timeout)
{
    ptls_socket_priv_t priv_tls = (ptls_socket_priv_t)fd;  

    return mbedtls_timeout(priv_tls->fd, timeout);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_hostname
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_hostname(int fd, const char* phostname)
{
    ptls_socket_priv_t priv_tls = (ptls_socket_priv_t)fd;

    return mbedtls_hostname(priv_tls->fd, phostname);
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_socket
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_socket(void)
{
    int ret = -1;
    ptls_socket_priv_t tls_drv = 0;

    tls_drv = malloc(sizeof(tls_socket_priv_t));
    ret = mbedtls_socket_regist(&tls_drv->api);
    
    if (tls_drv->api.socket == NULL || tls_drv->api.priv == NULL)
    {
        TRACE("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    tls_drv->fd = tls_drv->api.socket(tls_drv->api.priv, 0, 0, 0);
    if (ret >= 0 && tls_drv->fd >= 0)
    {
        return (int)tls_drv;
    }

    TRACE("%s,%d\r\n", __FUNCTION__, __LINE__);

    return 0;
    
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_connect(int fd, char* host_or_ip, char* port)
{
    ptls_socket_priv_t priv_tls = NULL;
    int len = strlen(host_or_ip) + 1;
    int ret = -1;

    mbedtls_hostinfo_t hostinfo;

    hostinfo.host = malloc(len);
    if (!hostinfo.host)
        return -1;
    
    memset(hostinfo.host, 0, len);
    memcpy(hostinfo.host, host_or_ip, strlen(host_or_ip));
    hostinfo.host_port = atoi(port);

    priv_tls = (ptls_socket_priv_t)fd;
    if (priv_tls->api.connect == NULL)
    {
        return -1;
    }
    ret = priv_tls->api.connect(priv_tls->api.priv, priv_tls->fd, NULL, 0, &hostinfo);
    free(hostinfo.host);

    return ret;
}



/*--------------------------------------
|   Function Name:
|       dsp_tls_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_send(int fd, const void* buf, int len, int flags)
{
    ptls_socket_priv_t priv_tls = NULL;
    int ret = -1;

    priv_tls = (ptls_socket_priv_t)fd;
    
    ret = priv_tls->api.send(priv_tls->api.priv, priv_tls->fd, buf, len, flags);

    return ret;
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_recv(int fd, void* buf, int len, int flags)
{
    ptls_socket_priv_t priv_tls = NULL;
    int ret = -1;

    priv_tls = (ptls_socket_priv_t)fd;
    ret = priv_tls->api.recv(priv_tls->api.priv, priv_tls->fd, buf, len, flags);

    return ret;
}

/*--------------------------------------
|   Function Name:
|       dsp_tls_socketclose
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int dsp_tls_socketclose(int fd)
{
    int ret = -1;
    ptls_socket_priv_t priv_tls = NULL;

    priv_tls = (ptls_socket_priv_t)fd;
    ret = priv_tls->api.socketclose(priv_tls->api.priv, priv_tls->fd);

    return ret;
}
