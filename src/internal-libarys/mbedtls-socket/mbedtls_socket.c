/*
********************************************************************************
*
*   File Name:
*       mbedtls_socket.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _MBEDTLS_SOCKET_MODULE
#define _MBEDTLS_SOCKET_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "config.h"
#include "mbedtls_socket.h"
#include "dspapi_tls.h"

#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl_cache.h"
#include "mbedtls/ssl.h"

#include "mbedtls_socket_platform.h"

#include "string.h"

// #include "ssl.h"
// #include "ctr_drbg.h"
// #include "ssl_cache.h"
// #include "pk.h"


/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define LONG_HH(val)            (((unsigned int)(val) >> 24) & 0xff)
#define LONG_HL(val)            (((unsigned int)(val) >> 16) & 0xff)
#define LONG_LH(val)            (((unsigned int)(val) >> 8) & 0xff)
#define LONG_LL(val)            ((unsigned int)(val) & 0xff)
#define LONG_H(val)             ((unsigned int)(val) >> 16) & 0xffff)
#define LONG_L(val)             ((unsigned int)(val) & 0xffff)

#define SHORT_H(val)            ((((unsigned short)(val)) >> 8) & 0xff)
#define SHORT_L(val)            (((unsigned short)(val)) & 0xff)

#define HTONS(x)                ((unsigned short)((SHORT_L(x) << 8) | SHORT_H(x)))
#define NTOHS(x)                ((unsigned short)((SHORT_L(x) << 8) | SHORT_H(x)))



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/

typedef struct _mbedtls_fd_t
{
    char session_id[32];
    mbedtls_net_context server_fd;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
#if (MBEDTLS_SESSION && MBEDTLS_CACHE_NUM > 0)
    mbedtls_ssl_session session;
#endif
}mbedtls_fd_t, *pmbedtls_fd_t;

typedef const mbedtls_fd_t     *pcmbedtls_fd_t;

typedef struct _mbedtls_priv_t
{
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clientcert;
    mbedtls_pk_context clientkey;
#if (MBEDTLS_SESSION)
#if (MBEDTLS_CACHE_NUM > 0)
    mbedtls_ssl_cache_context cache;
#else
    mbedtls_ssl_session session;
#endif
#endif
}mbedtls_priv_t, *pmbedtls_priv_t;

typedef const mbedtls_priv_t     *pcmbedtls_priv_t;

typedef struct _mbedtlsfd_set_t
{
    int count;
    int array[MBEDTLS_SOCK_NUM];
}mbedtlsfd_set_t, *pmbedtlsfd_set_t;

typedef const mbedtlsfd_set_t     *pcmbedtlsfd_set_t;



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/
static mbedtls_priv_t g_mbedtls_priv;


/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Functions
+----------------------------------------------------------------------------*/
/*--------------------------------------
|   Function Name:
|       mbedtls_util_inet_ntoa
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int mbedtls_util_inet_ntoa(unsigned int ip, char* pdata, unsigned int len_max)
{
    if(len_max < 16)
        return 0;
    sprintf(pdata, "%u.%u.%u.%u", (unsigned char)LONG_LL(ip), (unsigned char)LONG_LH(ip), (unsigned char)LONG_HL(ip), (unsigned char)LONG_HH(ip));
    return strlen(pdata);
}

/*--------------------------------------
|   Function Name:
|       conf_debug
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static void conf_debug( \
    void *ctx, int level, \
    const char *file, int line, \
    const char *str \
    )
{
    ((void) level);

    MBEDTLS_SOCK_PRINTF("%s:%04d: %s", file, line, str);
}

/*--------------------------------------
|   Function Name:
|       mbedtls_socket
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_socket(void* priv, int domain, int type, int protocol)
{
    pmbedtls_fd_t tlspriv = NULL;
    int ret = -1;

    if(priv == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    tlspriv = mbedtls_calloc(1, sizeof(mbedtls_fd_t));
    do
    {
        memset(tlspriv, 0x00, sizeof(mbedtls_fd_t));
        mbedtls_ssl_init(&tlspriv->ssl);
        mbedtls_ssl_config_init(&tlspriv->conf);
        mbedtls_net_init(&tlspriv->server_fd);
        mbedtls_ssl_set_hostname( &tlspriv->ssl, "localhost" );
        ret = mbedtls_ssl_config_defaults( \
            &tlspriv->conf, \
            MBEDTLS_SSL_IS_CLIENT, \
            MBEDTLS_SSL_TRANSPORT_STREAM, \
            MBEDTLS_SSL_PRESET_DEFAULT \
            );
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
            break;
        }
        mbedtls_ssl_conf_ca_chain( &tlspriv->conf, &g_mbedtls_priv.cacert, NULL );
        mbedtls_ssl_conf_rng( &tlspriv->conf, mbedtls_ctr_drbg_random, &g_mbedtls_priv.ctr_drbg );
        mbedtls_ssl_conf_dbg( &tlspriv->conf, conf_debug, 0 );
        ret = mbedtls_ssl_conf_own_cert(&tlspriv->conf, &g_mbedtls_priv.clientcert, &g_mbedtls_priv.clientkey);
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
            break;
        }
#if (MBEDTLS_CACHE_NUM > 0)
        mbedtls_ssl_session_init(&tlspriv->session);
#endif
    }while(0);
    if(ret < 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        mbedtls_free(tlspriv);
        return -1;
    }
    return (int)tlspriv;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_bind
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_bind(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int addrlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_listen
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_listen(void* priv, int fd, int backlog)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_accept
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_accept(void* priv, int fd, pcmbedtls_sockaddr_in_t cliaddr, int* addrlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_connect(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int addrlen, pmbedtls_hostinfo_t phostinfo)
{
    pmbedtls_fd_t tlspriv = NULL;
    int ret = -1;
    uint32_t flags;
    char ip[128] = {0};
    char port[8];

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    tlspriv = (pmbedtls_fd_t)fd;

    memset(ip, 0x00, sizeof(ip));
    memset(port, 0x00, sizeof(port));
    if (!phostinfo)
    {
        mbedtls_util_inet_ntoa(addr->sin_addr.s_addr, ip, sizeof(ip));
        sprintf(port, "%d", NTOHS(addr->sin_port));
    }
    else
    {
        memcpy(ip, phostinfo->host, strlen(phostinfo->host));
        sprintf(port, "%d", (phostinfo->host_port));
    }


    do
    {
        ret = mbedtls_ssl_setup( &tlspriv->ssl, &tlspriv->conf );
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
            break;
        }
#if (MBEDTLS_SESSION)
#if (MBEDTLS_CACHE_NUM > 0)
        ret = mbedtls_ssl_cache_get( \
            &g_mbedtls_priv.cache, \
            (const unsigned char*)tlspriv->session_id, sizeof(tlspriv->session_id), \
            &tlspriv->session);
        if(ret == 0)
        {
            mbedtls_ssl_session_reset(&tlspriv->ssl);
            ret = mbedtls_ssl_set_session(&tlspriv->ssl, &tlspriv->session);
            if(ret < 0)
            {
                MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
                break;
            }
        }
#else
        mbedtls_ssl_session_reset(&tlspriv->ssl);
        ret = mbedtls_ssl_set_session(&tlspriv->ssl, &g_mbedtls_priv.session);
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
            break;
        }
#endif
#endif
        ret = mbedtls_net_connect( \
            &tlspriv->server_fd, \
            ip, port, \
            MBEDTLS_NET_PROTO_TCP \
            );
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
            break;
        }
        mbedtls_ssl_set_bio( \
            &tlspriv->ssl, &tlspriv->server_fd, \
            mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout \
            );
        while( ( ret = mbedtls_ssl_handshake( &tlspriv->ssl ) ) != 0 )
        {
            if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
            {
                MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
                break;
            }
        }
        if( ( flags = mbedtls_ssl_get_verify_result( &tlspriv->ssl ) ) != 0 )
        {
    #if !defined(MBEDTLS_X509_REMOVE_INFO)
            char vrfy_buf[512];
    #endif
    #if !defined(MBEDTLS_X509_REMOVE_INFO)
            mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );
            MBEDTLS_SOCK_PRINTF("%s\r\n", vrfy_buf );
    #endif
        }
#if (MBEDTLS_SESSION)
#if (MBEDTLS_CACHE_NUM > 0)
        ret = mbedtls_ssl_get_session(&tlspriv->ssl, &tlspriv->session);
        if(ret == 0)
        {
            ret = mbedtls_ssl_cache_set( \
                &g_mbedtls_priv.cache, \
                (const unsigned char*)tlspriv->session_id, sizeof(tlspriv->session_id), \
                &tlspriv->session \
                );
            if(ret < 0)
            {
                MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
                break;
            }
        }
#else
        mbedtls_ssl_session_free(&g_mbedtls_priv.session);
        ret = mbedtls_ssl_get_session(&tlspriv->ssl, &g_mbedtls_priv.session);
        if(ret < 0)
        {
            MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
            break;
        }
#endif
#endif
    }while(0);
#if (MBEDTLS_SESSION && MBEDTLS_CACHE_NUM > 0)
    mbedtls_ssl_session_free(&tlspriv->session);
#endif
    return ret;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_setsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_setsockopt(void* priv, int fd, int level, int optname, void* optval, int optlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_getsockopt
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_getsockopt(void* priv, int fd, int level, int optname, void* optval, int optlen)
//{
//   return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_socketclose
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_socketclose(void* priv, int fd)
{
    pmbedtls_fd_t tlspriv = NULL;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    tlspriv = (pmbedtls_fd_t)fd;

    mbedtls_ssl_close_notify(&tlspriv->ssl);

    mbedtls_net_free(&tlspriv->server_fd);
    mbedtls_ssl_free(&tlspriv->ssl);
    mbedtls_ssl_config_free(&tlspriv->conf);

    mbedtls_free(tlspriv);

    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_errno
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_errno(void* priv, int fd)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_recvfrom
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_recvfrom(void* priv, int fd, char* buf, int len, int flags, pcmbedtls_sockaddr_in_t from, int fromlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_recv(void* priv, int fd, void* buf, int len, int flags)
{
    pmbedtls_fd_t tlspriv = NULL;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    tlspriv = (pmbedtls_fd_t)fd;

    if(flags & MBEDTLS_SOCK_MSG_DONTWAIT)
    {
        mbedtls_ssl_set_bio( \
            &tlspriv->ssl, &tlspriv->server_fd, \
            mbedtls_net_send, mbedtls_net_recv, NULL \
            );
        return mbedtls_ssl_read( &tlspriv->ssl, buf, len );
    }
    else
    {
        mbedtls_ssl_set_bio( \
            &tlspriv->ssl, &tlspriv->server_fd, \
            mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout \
            );
        return mbedtls_ssl_read( &tlspriv->ssl, buf, len );
    }
}

/*--------------------------------------
|   Function Name:
|       mbedtls_sendto
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_sendto(void* priv, int fd, char* buf, int len, int flags, pcmbedtls_sockaddr_in_t to, int tolen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_send(void* priv, int fd, const void* buf, int len, int flags)
{
    pmbedtls_fd_t tlspriv = NULL;
    int ret = -1;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    tlspriv = (pmbedtls_fd_t)fd;

    while( ( ret = mbedtls_ssl_write( &tlspriv->ssl, buf, len ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            break;
        }
    }
    return ret;
}

extern size_t mbedtls_ssl_get_bytes_avail(const mbedtls_ssl_context *ssl);

/*--------------------------------------
|   Function Name:
|       mbedtls_select
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int mbedtls_select(void* priv, pmbedtls_sockfd_set_t rd, pmbedtls_sockfd_set_t wr, pmbedtls_sockfd_set_t ex, int timeoutms)
{
    pmbedtlsfd_set_t fd_set = (pmbedtlsfd_set_t)rd;
    pmbedtls_fd_t tlspriv = NULL;
    int i = 0;
    int count = 0;
    int waitforever = 0;

    if(priv == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(timeoutms == 0)
    {
        waitforever = 1;
    }
    while(waitforever || timeoutms > 0)
    {
        for(i=0;i<fd_set->count;i++)
        {
            tlspriv = (pmbedtls_fd_t)fd_set->array[i];
            if(mbedtls_ssl_get_bytes_avail(&tlspriv->ssl) > 0)
            {
                count++;
            }
            else if(mbedtls_net_avail_timeout(&tlspriv->server_fd, 20) > 0)
            {
                count++;
            }
        }
        if(count > 0)
        {
            break;
        }
        timeoutms -= 20;
    }
    return count;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_fd_zero
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int mbedtls_fd_zero(void* priv, pmbedtls_sockfd_set_t pset)
{
    if(priv == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(pset, 0x00, sizeof(mbedtls_sockfd_set_t));
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_fd_clr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int mbedtls_fd_clr(void* priv, int fd, pmbedtls_sockfd_set_t pset)
{
    pmbedtlsfd_set_t fd_set = (pmbedtlsfd_set_t)pset;
    int i = 0;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    for(i=0;i<fd_set->count;i++)
    {
        if(fd == fd_set->array[i])
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
|       mbedtls_fd_set
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int mbedtls_fd_set(void* priv, int fd, pmbedtls_sockfd_set_t pset)
{
    pmbedtlsfd_set_t fd_set = (pmbedtlsfd_set_t)pset;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(fd_set->count >= MBEDTLS_SOCK_NUM)
    {
        return -1;
    }
    fd_set->array[fd_set->count++] = (int)fd;
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_fd_isset
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int mbedtls_fd_isset(void* priv, int fd, pmbedtls_sockfd_set_t pset)
{
    pmbedtlsfd_set_t fd_set = (pmbedtlsfd_set_t)pset;
    int i = 0;

    if(priv == 0 || fd == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
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
|       mbedtls_getpeername
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_getpeername(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int* addrlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_getsockname
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_getsockname(void* priv, int fd, pcmbedtls_sockaddr_in_t addr, int* addrlen)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_shutdown
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
//static int mbedtls_shutdown(void* priv, int how)
//{
//    return -1;
//}

/*--------------------------------------
|   Function Name:
|       mbedtls_init
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_init(void)
{
    const char *pers = __FUNCTION__;
    int ret = -1;

    mbedtls_platform_set_exit(porting_mbedtls_exit);
    mbedtls_platform_set_time(porting_mbedtls_time);
    mbedtls_platform_set_fprintf(porting_mbedtls_fprintf);
    mbedtls_platform_set_printf(porting_mbedtls_printf);
    mbedtls_platform_set_snprintf(snprintf);
    mbedtls_platform_set_vsnprintf(vsnprintf);
    mbedtls_platform_set_nv_seed(porting_mbedtls_nv_seed_read, porting_mbedtls_nv_seed_write);
    
    #if defined(MBEDTLS_PLATFORM_MEMORY)
    #if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    static unsigned char ptr[MBEDTLS_HEAP_SIZE];
    mbedtls_memory_buffer_alloc_init(ptr, MBEDTLS_HEAP_SIZE);
    #else
    mbedtls_platform_set_calloc_free(porting_mbedtls_calloc, porting_mbedtls_free);
    #endif //defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    #endif //defined(MBEDTLS_PLATFORM_MEMORY)
    #if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( MBEDTLS_INNER_TRACE_LVL );
    #endif

    #if defined(MBEDTLS_THREADING_C) && defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt( \
        porting_mbedtls_mutex_init, \
        porting_mbedtls_mutex_free, \
        porting_mbedtls_mutex_lock, \
        porting_mbedtls_mutex_unlock \
        );
    #endif

    mbedtls_entropy_init(&g_mbedtls_priv.entropy);
    mbedtls_ctr_drbg_init(&g_mbedtls_priv.ctr_drbg);
    mbedtls_x509_crt_init(&g_mbedtls_priv.cacert);
    mbedtls_x509_crt_init(&g_mbedtls_priv.clientcert);
    mbedtls_pk_init(&g_mbedtls_priv.clientkey);
#if (MBEDTLS_SESSION)
#if (MBEDTLS_CACHE_NUM > 0)
    mbedtls_ssl_cache_init(&g_mbedtls_priv.cache);
#else
    mbedtls_ssl_session_init(&g_mbedtls_priv.session);
#endif
#endif

    ret = mbedtls_ctr_drbg_seed( \
        &g_mbedtls_priv.ctr_drbg, \
        mbedtls_entropy_func, &g_mbedtls_priv.entropy, \
        (const unsigned char *) pers, strlen( pers ) \
        );
    if(ret < 0)
    {
        return ret;
    }

    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_uninit
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_uninit(void)
{
    mbedtls_entropy_free(&g_mbedtls_priv.entropy);
    mbedtls_ctr_drbg_free(&g_mbedtls_priv.ctr_drbg);
    mbedtls_x509_crt_free(&g_mbedtls_priv.cacert);
    mbedtls_x509_crt_free(&g_mbedtls_priv.clientcert);
    mbedtls_pk_free(&g_mbedtls_priv.clientkey);
#if (MBEDTLS_SESSION)
#if (MBEDTLS_CACHE_NUM > 0)
    mbedtls_ssl_cache_free(&g_mbedtls_priv.cache);
#else
    mbedtls_ssl_session_init(&g_mbedtls_priv.session);
#endif
#endif
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_cacert
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_cacert(const unsigned char* pdata, int len)
{
    int ret = -1;

    ret = mbedtls_x509_crt_parse( \
        &g_mbedtls_priv.cacert, \
        (const unsigned char *)pdata, len \
        );
    if( ret < 0 )
    {
        return ret;
    }
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_clikey
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_clikey(const unsigned char* pdata, int len)
{
    int ret = -1;

    ret = mbedtls_pk_parse_key( \
        &g_mbedtls_priv.clientkey, \
        (const unsigned char *)pdata, len, \
        NULL, 0, \
        mbedtls_ctr_drbg_random, &g_mbedtls_priv.ctr_drbg \
        );
    if(ret < 0)
    {
        return ret;
    }
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_clicert
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_clicert(const unsigned char* pdata, int len)
{
    int ret = -1;

    ret = mbedtls_x509_crt_parse( \
        &g_mbedtls_priv.clientcert, \
        (const unsigned char *)pdata, len \
        );
    if(ret < 0)
    {
        return ret;
    }
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_vfymode
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_vfymode(int fd, int mode)
{
    pmbedtls_fd_t tlspriv = NULL;

    tlspriv = (pmbedtls_fd_t)fd;

    mbedtls_ssl_conf_authmode( &tlspriv->conf, mode );
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_timeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_timeout(int fd, int timeout)
{
    pmbedtls_fd_t tlspriv = NULL;

    tlspriv = (pmbedtls_fd_t)fd;

    mbedtls_ssl_conf_read_timeout( &tlspriv->conf, timeout );
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_hostname
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_hostname(int fd, const char* phostname)
{
    pmbedtls_fd_t tlspriv = NULL;
    int len = 0;
    int ret = -1;

    tlspriv = (pmbedtls_fd_t)fd;

    ret = mbedtls_ssl_set_hostname( &tlspriv->ssl, phostname );
    if(ret < 0)
    {
        return ret;
    }

    memset(tlspriv->session_id, 0x00, sizeof(tlspriv->session_id));
    len = (strlen(phostname) > sizeof(tlspriv->session_id)) ? sizeof(tlspriv->session_id) : strlen(phostname);
    memcpy(tlspriv->session_id, phostname, len);
    
    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_socket_regist
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_socket_regist(pmbedtls_socket_body_t pdrv)
{
    pdrv->priv = &g_mbedtls_priv;
    pdrv->socket = mbedtls_socket;
    pdrv->connect = mbedtls_connect;
    pdrv->recv = mbedtls_recv;
    pdrv->send = mbedtls_send;
    pdrv->socketclose = mbedtls_socketclose;
    pdrv->select = mbedtls_select;
    pdrv->fd_zero = mbedtls_fd_zero;
    pdrv->fd_clr = mbedtls_fd_clr;
    pdrv->fd_Set = mbedtls_fd_set;
    pdrv->fd_isset = mbedtls_fd_isset;
    pdrv->getaddrinfo = NULL;
    pdrv->freeaddrinfo = NULL;

    return 0;
}
