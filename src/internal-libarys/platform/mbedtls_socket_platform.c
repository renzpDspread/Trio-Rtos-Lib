/*
********************************************************************************
*
*   File Name:
*       mbedtls_socket_platform.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _MBEDTLS_SOCKET_PLATFORM_MODULE
#define _MBEDTLS_SOCKET_PLATFORM_C

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

#include "mbedtls_socket_platform.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <liteos_api.h>
#include "liteMapi.h"





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
|       mbedtls_util_inet_addr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int mbedtls_util_inet_addr(char* str)
{
    unsigned int lhost = 0;
    char *plong = (char *)&lhost;
    char *p = (char *)str;
    while (p)
    {
        *plong++ = atoi(p);
        p = strchr(p, '.');
        if (p)
            ++p;
    }
    return lhost;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_exit
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void porting_mbedtls_exit(int status)
{
	
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_exit
|   Description: determine whether it is a leap year
|   Parameters:
|   Returns:
+-------------------------------------*/
int isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return 1;
    }
    return 0;
}
/*--------------------------------------
|   Function Name:
|       porting_mbedtls_exit
|   Description: calculate the number of days in a given month
|   Parameters:
|   Returns:
+-------------------------------------*/
int getDaysInMonth(int month, int year) {
    int days;
    switch (month) {
        case 2:
            days = isLeapYear(year) ? 29 : 28;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            days = 30;
            break;
        default:
            days = 31;
            break;
    }
    return days;
}
/*--------------------------------------
|   Function Name:
|       porting_mbedtls_exit
|   Description: convert year, month, day, hour, minute and second to Greenwich Mean Time seconds
|   Parameters:
|   Returns:
+-------------------------------------*/
int convertToGMT(int year, int month, int day, int hour, int minute, int second) {
    int totalSeconds = 0;

    // Calculate the number of seconds corresponding to the year
    for (int i = 1970; i < year; i++) {
        totalSeconds += isLeapYear(i) ? 366 * 24 * 60 * 60 : 365 * 24 * 60 * 60;
    }

    // Calculate the number of seconds corresponding to the month
    for (int i = 1; i < month; i++) {
        totalSeconds += getDaysInMonth(i, year) * 24 * 60 * 60;
    }

    // Calculate the number of seconds corresponding to the day
    totalSeconds += (day - 1) * 24 * 60 * 60;

    // Calculate the number of seconds corresponding to hours, minutes and seconds
    totalSeconds += hour * 60 * 60 + minute * 60 + second;

    return totalSeconds;
}
/*--------------------------------------
|   Function Name:
|       porting_mbedtls_time
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
mbedtls_time_t porting_mbedtls_time(mbedtls_time_t * timer)
{
    unsigned char tbuf[16] = { 0 };
    memset(tbuf, 0, sizeof(tbuf));

    GetTime(tbuf);

    int t = convertToGMT((2000 + tbuf[0]), tbuf[1], tbuf[2], tbuf[3], tbuf[4], tbuf[5]);
    //return (mbedtls_time_t)time(0);
    return t;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_fprintf
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int porting_mbedtls_fprintf( FILE * stream, const char * format, ... )
{
    return 0;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_printf
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int porting_mbedtls_printf( const char * format, ... )
{
    return 0;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_nv_seed_read
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int porting_mbedtls_nv_seed_read(unsigned char * buf, size_t buf_len)
{
    int i = 0;
    unsigned char randbuf[8 + 1] = {0};

    for(i=0;i<buf_len;i++)
    {
        PCIGetRandom(randbuf);
        buf[i] = randbuf[0];
    }
	return 0;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_nv_seed_write
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int porting_mbedtls_nv_seed_write(unsigned char * buf, size_t buf_len)
{
	return 0;
}

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_calloc
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
#if defined(MBEDTLS_PLATFORM_MEMORY) && !defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
void *porting_mbedtls_calloc( size_t n, size_t size )
{
    void* ptr = 0;

    ptr = malloc(n*size);
    if(ptr != 0)
    {
        memset(ptr, 0x00, n*size);
    }
    if(ptr == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
    }
    return ptr;
}
#endif //defined(MBEDTLS_PLATFORM_MEMORY) && !defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)

/*--------------------------------------
|   Function Name:
|       porting_mbedtls_free
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
#if defined(MBEDTLS_PLATFORM_MEMORY) && !defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
void porting_mbedtls_free( void *ptr )
{
    if(ptr == 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);
    }
    if(ptr != 0)
    {
        free(ptr);
    }
}
#endif //defined(MBEDTLS_PLATFORM_MEMORY) && !defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)

/*--------------------------------------
|   Function Name:
|       mbedtls_net_init
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void mbedtls_net_init( mbedtls_net_context *ctx )
{


}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
    int ret = 0;
    int socket_id = 0;
    struct net_sockaddr addr;
    int tm = 4, timeout = 3000;

    //MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);

    ret = NetSocket(NET_AF_INET, NET_SOCK_STREAM, 0);
    if(ret < 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    socket_id = ret;
    ctx->private_fd = ret;

    ret = Netioctl(socket_id, CMD_TO_SET, 0);
    if (ret<0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    MBEDTLS_SOCK_PRINTF("%s,%d try to connecting host = %s:%d\r\n", __FUNCTION__, __LINE__, host, atoi(port));

    SockAddrSet(&addr, (char*)host, atoi(port));
    ret = NetConnect(socket_id, &addr, sizeof(struct net_sockaddr));
    if(ret < 0)
    {
        MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    TimerSet(tm, timeout/100);
    while (1)
    {        
        if (0 == TimerCheck(tm))
        {
             MBEDTLS_SOCK_PRINTF("%s,%d connect host timeout\r\n", __FUNCTION__, __LINE__);
            return -1;
        }

        ret = Netioctl(socket_id, CMD_EVENT_GET, 0);
        if (ret & SOCK_EVENT_CONN)
        {
            ctx->private_fd = socket_id;
            MBEDTLS_SOCK_PRINTF("%s,%d connect host success\r\n", __FUNCTION__, __LINE__);
            return 0;
        }
    }

    return 0;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_free
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void mbedtls_net_free( mbedtls_net_context *ctx )
{
    int ret = -1;
    ret = NetCloseSocket((int)ctx->private_fd);
    if(ret<0)    MBEDTLS_SOCK_PRINTF("%s,%d  NetCloseSocket = %d\r\n", __FUNCTION__, __LINE__, ret);
}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    mbedtls_net_context * pctx = ctx;
    int ret = -1;

    ret = NetSend((int)pctx->private_fd, (char *)buf, len, 0);
    MBEDTLS_SOCK_PRINTF("TLS SEND(%d):\r\n", ret);
    if (ret < 0)
    {
        MBEDTLS_SOCK_PRINTF("Net_State =%08x\r\n", Netioctl((int)pctx->private_fd, CMD_EVENT_GET, 0));
        //MBEDTLS_SOCK_DUMP((unsigned char*)buf, len);
    }
    else
    {
    //    MBEDTLS_SOCK_DUMP(buf, ret);
    }

    return ret;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    mbedtls_net_context * pctx = ctx;
    int ret = -1;

    ret = NetRecv((int)pctx->private_fd, buf, len, 0);
    MBEDTLS_SOCK_PRINTF("TLS RECV(%d):\r\n", ret);
    //if(ret > 0)
    //{
    //    MBEDTLS_SOCK_DUMP(buf, ret);
    //}
    return ret;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_recv_timeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout )
{
    int ret = -1;
    int tm = 1;
    mbedtls_net_context *pctx = ctx;    
    
    //MBEDTLS_SOCK_PRINTF("%s timeout = %d socket_id = %d\r\n", __FUNCTION__, timeout, (int)pctx->private_fd);

    TimerSet(tm, timeout / 100);

    while (1)
    {
        if (0 == TimerCheck(tm))
        {
            MBEDTLS_SOCK_PRINTF("%s timeout\r\n", __FUNCTION__);
            return MBEDTLS_ERR_SSL_TIMEOUT;
        }        

        ret = NetRecv((int)pctx->private_fd, buf, len, 0);
        if (ret <= 0)
        {
            if (ret < 0)  MBEDTLS_SOCK_PRINTF("%s,%d ret = %d\r\n", __FUNCTION__, __LINE__, ret);
            DelayMs(10);
            continue;
        }
        break;   

    }

    MBEDTLS_SOCK_PRINTF("%s rece len = %d\r\n",  __FUNCTION__, ret);

    return ret;
}

/*--------------------------------------
|   Function Name:
|       mbedtls_net_avail_timeout
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int mbedtls_net_avail_timeout( void *ctx, uint32_t timeout)
{
    int ret = -1;
    mbedtls_net_context *pctx = ctx;
    
    ret = Netioctl((int)pctx->private_fd, CMD_EVENT_GET, 0);
    if (ret>=0 & SOCK_EVENT_CONN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
