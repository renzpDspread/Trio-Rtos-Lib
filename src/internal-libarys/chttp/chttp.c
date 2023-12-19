/*
********************************************************************************
*
*   File Name:
*       chttp.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _CHTTP_MODULE
#define _CHTTP_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include <string.h>

#include "chttp.h"



/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define alloc_mem(size) \
    g_chttp_hooks_priv.alloc(size)
#define free_mem(addr) \
    g_chttp_hooks_priv.free(addr)
#define realloc_mem(addr, size) \
    g_chttp_hooks_priv.realloc(addr, size)



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _chttp_hooks_t
{
    void* (*alloc)(int );
    void (*free)(void* );
    void* (*realloc)(void* , int );
} chttp_hooks_priv_t, *pchttp_hooks_priv_t;

typedef const chttp_hooks_priv_t   *pcchttp_hooks_priv_t;

typedef struct _http_line_priv_t
{
    char* pdata;
    int len;
} http_line_priv_t, *phttp_line_priv_t;

typedef const http_line_priv_t   *pchttp_line_priv_t;

typedef struct _http_header_priv_t
{
    char* pdata;
    int len;
    struct _http_header_priv_t* next;
} http_header_priv_t, *phttp_header_priv_t;

typedef const http_header_priv_t   *pchttp_header_priv_t;

typedef struct _http_body_priv_t
{
    char* pdata;
    int len;
} http_body_priv_t, *phttp_body_priv_t;

typedef const http_body_priv_t   *pchttp_body_priv_t;

typedef struct _http_priv_t
{
    http_line_priv_t line;
    phttp_header_priv_t header;
    http_body_priv_t body;
    phttp_header_priv_t pseek;
    char* format;
} http_priv_t, *phttp_priv_t;

typedef const http_priv_t   *pchttp_priv_t;



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/
static chttp_hooks_priv_t g_chttp_hooks_priv;



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Functions
+----------------------------------------------------------------------------*/
/*--------------------------------------
|   Function Name:
|       chttp_init_hooks
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_init_hooks(void* (*alloc)(int ), void (*free)(void*), void* (*realloc)(void*, int))
{
    g_chttp_hooks_priv.alloc = alloc;
    g_chttp_hooks_priv.free = free;
    g_chttp_hooks_priv.realloc = realloc;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_new
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_new(void** ppctl)
{
    phttp_priv_t priv = 0;

    if(ppctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = alloc_mem(sizeof(http_priv_t));
    if(priv == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memset(priv, 0x00, sizeof(http_priv_t));
    *ppctl = (void*)priv;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_del
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_del(void* pctl)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t pthis = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->line.pdata != 0)
    {
        free_mem(priv->line.pdata);
    }
    header = priv->header;
    while(header != 0)
    {
        if(header->pdata != 0)
        {
            free_mem(header->pdata);
        }
        pthis = header;
        header = header->next;
        free_mem(pthis);
    }
    if(priv->body.pdata != 0)
    {
        free_mem(priv->body.pdata);
    }
    if(priv->format != 0)
    {
        free_mem(priv->format);
    }
    free_mem(priv);
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_line_set
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_line_set(void* pctl, const char* pval, int len)
{
    phttp_priv_t priv = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->line.pdata != 0)
    {
        free_mem(priv->line.pdata);
    }
    priv->line.pdata = alloc_mem(len);
    if(priv->line.pdata == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memcpy(priv->line.pdata, pval, len);
    priv->line.len = len;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_line_get
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_line_get(void* pctl, char** ppval, int* plen)
{
    phttp_priv_t priv = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    if(ppval == 0 || plen == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->line.pdata == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    *ppval = priv->line.pdata;
    *plen = priv->line.len;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_header_clr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_header_clr(void* pctl)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t pthis = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    header = priv->header;
    while(header != 0)
    {
        if(header->pdata != 0)
        {
            free_mem(header->pdata);
        }
        pthis = header;
        header = header->next;
        free_mem(pthis);
    }
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_header_add
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_header_add(void* pctl, const char* pval, int len)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t pthis = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    if(pval == 0 || len == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    pthis = priv->header;
    header = priv->header;
    while(header != 0)
    {
        pthis = header;
        header = header->next;
    }
    header = alloc_mem(sizeof(http_header_priv_t));
    if(header == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    header->pdata = alloc_mem(len);
    if(header->pdata == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memcpy(header->pdata, pval, len);
    header->len = len;
    header->next = 0;
    if(pthis != 0)
    {
        pthis->next = header;
    }
    else
    {
        priv->header = header;
    }
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_header_first
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_header_first(void* pctl, char** ppval, int* plen)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    if(ppval == 0 || plen == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    header = priv->header;
    if(header == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    *ppval = header->pdata;
    *plen = header->len;
    priv->pseek = header->next;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_header_next
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_header_next(void* pctl, char** ppval, int* plen)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    if(ppval == 0 || plen == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    header = priv->pseek;
    if(header == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    *ppval = header->pdata;
    *plen = header->len;
    priv->pseek = header->next;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_header_find
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_header_find(void* pctl, const char* pkey, char** ppval, int* plen)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t header = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    if(ppval == 0 || plen == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    header = priv->header;
    while(header != 0)
    {
        if(header->pdata == 0)
        {
            header = header->next;
            continue;
        }
        if(memcmp(header->pdata, pkey, strlen(pkey)) != 0)
        {
            header = header->next;
            continue;
        }
        *ppval = header->pdata;
        *plen = header->len;
        return CHTTP_ERR_OK;
    }
    return CHTTP_ERR_NOT_FOUND;
}

/*--------------------------------------
|   Function Name:
|       chttp_body_set
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_body_set(void* pctl, const char* pval, int len)
{
    phttp_priv_t priv = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->body.pdata != 0)
    {
        free_mem(priv->body.pdata);
    }
    priv->body.pdata = alloc_mem(len);
    if(priv->body.pdata == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memcpy(priv->body.pdata, pval, len);
    priv->body.len = len;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_body_append
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_body_append(void* pctl, const char* pval, int len)
{
    phttp_priv_t priv = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    priv->body.pdata = realloc_mem(priv->body.pdata, priv->body.len + len);
    if(priv->body.pdata == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memcpy(&priv->body.pdata[priv->body.len], pval, len);
    priv->body.len += len;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_body_get
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_body_get(void* pctl, char** ppval, int* plen)
{
    phttp_priv_t priv = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->body.pdata == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    *ppval = priv->body.pdata;
    *plen = priv->body.len;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_parse
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_parse(void* pctl, const char* pval, int len)
{
    int err = 0;
    //phttp_priv_t priv = 0;
    char* bgn = 0;
    char* end = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    //priv = (phttp_priv_t)pctl;
    bgn = (char*)pval;
    end = strstr(bgn, "\r\n");
    err = chttp_line_set(pctl, bgn, end - bgn);
    if(err != CHTTP_ERR_OK)
    {
        return err;
    }
    bgn = end + 2;
    while(strcmp(bgn, "\r\n") != 0)
    {
        end = strstr(bgn, "\r\n");
        if(0 == (end - bgn))
        {
            break;
        }
        err = chttp_header_add(pctl, bgn, end - bgn);
        if(err != CHTTP_ERR_OK)
        {
            return err;
        }
        bgn = end + 2;
    }
    bgn += 2;
    end = (char*)pval + len;
    err = chttp_body_set(pctl, bgn, end - bgn);
    if(err != CHTTP_ERR_OK)
    {
        return err;
    }
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_len
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_len(void* pctl, int* plen)
{
    phttp_priv_t priv = 0;
    phttp_header_priv_t header = 0;
    int cnt = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->line.pdata == 0 && priv->line.len <= 0)
    {
        return CHTTP_ERR_PARAM;
    }
    cnt += priv->line.len;
    cnt += 2;
    header = priv->header;
    while(header != 0)
    {
        if(header->pdata != 0 && header->len > 0)
        {
            cnt += header->len;
            cnt += 2;
        }
        header = header->next;
    }
    cnt += 2;
    if(priv->body.pdata != 0 && priv->body.len > 0)
    {
        cnt += priv->body.len;
    }
    *plen = cnt;
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_format
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_format(void* pctl, char** ppval, int* plen)
{
    int err = 0;
    phttp_priv_t priv = 0;
    phttp_header_priv_t header = 0;
    int cnt = 0;
    int buflen = 0;

    if(pctl == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    priv = (phttp_priv_t)pctl;
    if(priv->line.pdata == 0 || priv->line.len <= 0)
    {
        return CHTTP_ERR_PARAM;
    }
    err = chttp_len(pctl, &buflen);
    if(err != CHTTP_ERR_OK)
    {
        return err;
    }
    if(priv->format != 0)
    {
        free_mem(priv->format);
    }
    priv->format = alloc_mem(buflen);
    if(priv->format == 0)
    {
        return CHTTP_ERR_ALLOC;
    }
    memcpy(&priv->format[cnt], priv->line.pdata, priv->line.len);
    cnt += priv->line.len;
    memcpy(&priv->format[cnt], "\r\n", 2);
    cnt += 2;
    header = priv->header;
    while(header != 0)
    {
        if(header->pdata != 0 && header->len > 0)
        {
            memcpy(&priv->format[cnt], header->pdata, header->len);
            cnt += header->len;
            memcpy(&priv->format[cnt], "\r\n", 2);
            cnt += 2;
        }
        header = header->next;
    }
    memcpy(&priv->format[cnt], "\r\n", 2);
    cnt += 2;
    if(priv->body.pdata != 0 && priv->body.len > 0)
    {
        memcpy(&priv->format[cnt], priv->body.pdata, priv->body.len);
        cnt += priv->body.len;
    }
    *ppval = priv->format;
    *plen = cnt;
    return CHTTP_ERR_OK;
}
