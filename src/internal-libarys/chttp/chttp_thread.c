/*
********************************************************************************
*
*   File Name:
*       chttp_thread.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _CHTTP_THREAD_MODULE
#define _CHTTP_THREAD_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "chttp.h"
#include "chttp_api.h"
#include "chttp_api_platform.h"
#include "chttp_thread.h"
#include "chttp_cfg.h"



/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/
enum
{
    CHTTP_THREAD_ID_INIT = 0,
    CHTTP_THREAD_ID_CONNECT,
    CHTTP_THREAD_ID_DISCONNECT,
    CHTTP_THREAD_ID_API
};



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _chttp_thread_ctx_init_t
{
    chttp_thread_cb_init_t cb;
}chttp_thread_ctx_init_t, *pchttp_thread_ctx_init_t;

typedef const chttp_thread_ctx_init_t     *pcchttp_thread_ctx_init_t;

typedef struct _chttp_thread_ctx_connect_t
{
    char host[128];
    char port[8];
    int protocol;
    int timeout;
    chttp_thread_cb_connect_t cb;
}chttp_thread_ctx_connect_t, *pchttp_thread_ctx_connect_t;

typedef const chttp_thread_ctx_connect_t     *pcchttp_thread_ctx_connect_t;

typedef struct _chttp_thread_ctx_disconnect_t
{
    int fd;
    chttp_thread_cb_disconnect_t cb;
}chttp_thread_ctx_disconnect_t, *pchttp_thread_ctx_disconnect_t;

typedef const chttp_thread_ctx_disconnect_t     *pcchttp_thread_ctx_disconnect_t;

typedef struct _chttp_thread_ctx_api_t
{
    int fd;
    char url[256];
    char method[16];
    char version[16];
    void* wr;
    void* rd;
    int timeout;
    chttp_thread_cb_api_t cb;
}chttp_thread_ctx_api_t, *pchttp_thread_ctx_api_t;

typedef const chttp_thread_ctx_api_t     *pcchttp_thread_ctx_api_t;

typedef struct _chttp_thread_ctx_t
{
    int id;
    void* ctx;
}chttp_thread_ctx_t, *pchttp_thread_ctx_t;

typedef const chttp_thread_ctx_t     *pcchttp_thread_ctx_t;



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
|       chttp_thread_init_internal
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_init_internal(chttp_thread_ctx_init_t* ctx)
{
    int ret = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    ret = http_message_init();
    if(ret == CHTTP_ERR_OK)
    {
        ret = chttp_api_init();
    }

    if(ctx->cb != 0)
    {
        ctx->cb(ret);
    }

    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_connect_internal
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_connect_internal(chttp_thread_ctx_connect_t* ctx)
{
    int ret = 0;
    int fd = -1;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    ret = chttp_api_connect(ctx->host, ctx->port, ctx->protocol, ctx->timeout, &fd);

    if(ctx->cb != 0)
    {
        ctx->cb(ret, fd);
    }

    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_disconnect_internal
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_disconnect_internal(chttp_thread_ctx_disconnect_t* ctx)
{
    int ret = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    ret = chttp_api_disconnect(ctx->fd);

    if(ctx->cb != 0)
    {
        ctx->cb(ret);
    }

    return 0;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_api_internal
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_api_internal(chttp_thread_ctx_api_t* ctx)
{
    int ret = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    ret = chttp_api(ctx->fd, ctx->url, ctx->method, ctx->version, ctx->wr, ctx->rd, ctx->timeout);

    if(ctx->cb != 0)
    {
        ctx->cb(ret, ctx->wr, ctx->rd);
    }

    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_process
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_process(void)
{
    chttp_thread_ctx_t* msg;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    while(1)
    {
        http_message_recv((void**)&msg);

        CHTTP_PRINTF("msg->id = %d\r\n", msg->id);

        switch(msg->id)
        {
        case CHTTP_THREAD_ID_INIT:
            chttp_thread_init_internal(msg->ctx);
            break;
        case CHTTP_THREAD_ID_CONNECT:
            chttp_thread_connect_internal(msg->ctx);
            break;
        case CHTTP_THREAD_ID_DISCONNECT:
            chttp_thread_disconnect_internal(msg->ctx);
            break;
        case CHTTP_THREAD_ID_API:
            chttp_thread_api_internal(msg->ctx);
            break;
        default:

            break;
        }

        http_free(msg->ctx);
        http_free(msg);
    }
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_init
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_init( \
    chttp_thread_cb_init_t cb)
{
    chttp_thread_ctx_t* pctx = 0;
    chttp_thread_ctx_init_t* pinit = 0;
    int ret = -1;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    if(cb == 0)
    {
        ret = http_message_init();
        if(ret == CHTTP_ERR_OK)
        {
            ret = chttp_api_init();
        }
        return ret;
    }
    pctx = http_malloc(sizeof(chttp_thread_ctx_t));
    pctx->id = CHTTP_THREAD_ID_INIT;
    pctx->ctx = http_malloc(sizeof(chttp_thread_ctx_init_t));
    pinit = pctx->ctx;
    memset(pinit, 0x00, sizeof(chttp_thread_ctx_init_t));
    pinit->cb = cb;
    http_message_send(pctx);

    return 0;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_connect( \
    const char* host, const char* port, int protocol, int timeout, int* fd, \
    chttp_thread_cb_connect_t cb)
{
    chttp_thread_ctx_t* pctx = 0;
    chttp_thread_ctx_connect_t* pconnect = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    if(cb == 0)
    {
        return chttp_api_connect(host, port, protocol, timeout, fd);
    }
    pctx = http_malloc(sizeof(chttp_thread_ctx_t));
    pctx->id = CHTTP_THREAD_ID_CONNECT;
    pctx->ctx = http_malloc(sizeof(chttp_thread_ctx_connect_t));
    pconnect = pctx->ctx;
    memset(pconnect, 0x00, sizeof(chttp_thread_ctx_connect_t));
    strcpy(pconnect->host, host);
    strcpy(pconnect->port, port);
    pconnect->protocol = protocol;
    pconnect->timeout = timeout;
    pconnect->cb = cb;
    http_message_send(pctx);

    return 0;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_disconnect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_disconnect(int fd, \
    chttp_thread_cb_disconnect_t cb)
{
    chttp_thread_ctx_t* pctx = 0;
    chttp_thread_ctx_disconnect_t* pdisconnect = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    if(cb == 0)
    {
        return chttp_api_disconnect(fd);
    }
    pctx = http_malloc(sizeof(chttp_thread_ctx_t));
    pctx->id = CHTTP_THREAD_ID_DISCONNECT;
    pctx->ctx = http_malloc(sizeof(chttp_thread_ctx_disconnect_t));
    pdisconnect = pctx->ctx;
    memset(pdisconnect, 0x00, sizeof(chttp_thread_ctx_disconnect_t));
    pdisconnect->fd = fd;
    pdisconnect->cb = cb;
    http_message_send(pctx);

    return 0;
}

/*--------------------------------------
|   Function Name:
|       chttp_thread_api
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_thread_api(int fd, const char* url, const char* method, const char* version, \
    void* wr, void* rd, int timeout, \
    chttp_thread_cb_api_t cb)
{
    chttp_thread_ctx_t* pctx = 0;
    chttp_thread_ctx_api_t* papi = 0;
    CHTTP_PRINTF("%s,%d\r\n", __FUNCTION__, __LINE__);

    if(cb == 0)
    {
        return chttp_api(fd, url, method, version, wr, rd, timeout);
    }
    pctx = http_malloc(sizeof(chttp_thread_ctx_t));
    pctx->id = CHTTP_THREAD_ID_API;
    pctx->ctx = http_malloc(sizeof(chttp_thread_ctx_api_t));
    papi = pctx->ctx;
    memset(papi, 0x00, sizeof(chttp_thread_ctx_api_t));
    papi->fd = fd;
    strcpy(papi->url, url);
    strcpy(papi->method, method);
    strcpy(papi->version, version);
    papi->wr = wr;
    papi->rd = rd;
    papi->timeout = timeout;
    papi->cb = cb;
    http_message_send(pctx);

    return 0;
}

