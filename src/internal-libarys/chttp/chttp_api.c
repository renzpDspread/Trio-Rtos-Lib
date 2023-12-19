/*
********************************************************************************
*
*   File Name:
*       chttp_api.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _CHTTP_API_MODULE
#define _CHTTP_API_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chttp_cfg.h"
#include "chttp.h"
#include "chttp_api.h"
#include "chttp_api_platform.h"

#include "net_container.h"
#include "mbedtls_socket.h"
#include "dspapi_tls.h"
#include "config.h"


/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define LONG_HH(val)            ((((unsigned int)(val)) >> 24) & 0xff)
#define LONG_HL(val)            ((((unsigned int)(val)) >> 16) & 0xff)
#define LONG_LH(val)            ((((unsigned int)(val)) >> 8) & 0xff)
#define LONG_LL(val)            (((unsigned int)(val)) & 0xff)

#define SHORT_H(val)            ((((unsigned short)(val)) >> 8) & 0xff)
#define SHORT_L(val)            (((unsigned short)(val)) & 0xff)

#define HTONS(x)                ((unsigned short)((SHORT_L(x) << 8) | SHORT_H(x)))
#define NTOHS(x)                ((unsigned short)((SHORT_L(x) << 8) | SHORT_H(x)))

#define HTTP_LINE_LEN_MAX               (512)
#define HTTP_BODY_BLK_SIZE              (1024)
#define HTTP_BODY_LEN_MAX               (128*1024)

#define HTTP_IP_CACHE_NUM               (5)



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _chttp_api_ip_t
{
    char     host[128];
    char     ip[32];
}chttp_api_ip_t, *pchttp_api_ip_t;

typedef const chttp_api_ip_t     *pcchttp_api_ip_t;

typedef struct _chttp_api_cache_t
{
    chttp_api_ip_t item[HTTP_IP_CACHE_NUM];
    int nums;
}chttp_api_cache_t, *pchttp_api_cache_t;

typedef const chttp_api_cache_t     *pcchttp_api_cache_t;



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/
static chttp_api_cache_t g_chttp_api_cache;



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Functions
+----------------------------------------------------------------------------*/

/*--------------------------------------
|   Function Name:
|       chttp_url_scheme
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_url_scheme(const char* url, char** scheme)
{
    char* bgn = 0;
    char* end = 0;

    if(url == 0 || scheme == 0)
    {
        return 0;
    }
    *scheme = 0;
    bgn = (char*)url;
    end = bgn;
    while(*end != ':' && *end != '\0')
    {
        end++;
    }
    *scheme = bgn;
    return end - bgn;
}

/*--------------------------------------
|   Function Name:
|       chttp_url_host
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_url_host(const char* url, char** host)
{
    char* bgn = 0;
    char* end = 0;

    if(url == 0 || host == 0)
    {
        return 0;
    }
    *host = 0;
    bgn = strstr(url, "//");
    if(bgn == 0)
    {
        return 0;
    }
    bgn += 2;
    end = bgn;
    while(*end != '/' && *end != ':' && *end != '\0')
    {
        end++;
    }
    *host = bgn;
    return end - bgn;
}

/*--------------------------------------
|   Function Name:
|       chttp_url_port
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_url_port(const char* url, char** port)
{
    char* bgn = 0;
    char* end = 0;

    if(url == 0 || port == 0)
    {
        return 0;
    }
    *port = 0;
    bgn = strstr(url, "//");
    if(bgn == 0)
    {
        return 0;
    }
    bgn += 2;
    if(strstr(bgn, ":") == 0 || (unsigned int)strstr(bgn, ":") > (unsigned int)strstr(bgn, "/"))
    {
        return 0;
    }
    bgn = strstr(bgn, ":");
    bgn += 1;
    end = bgn;
    while(*end != '/' && *end != '\0')
    {
        end++;
    }
    *port = bgn;
    return end - bgn;
}

/*--------------------------------------
|   Function Name:
|       chttp_url_path
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_url_path(const char* url, char** path)
{
    char* bgn = 0;
    char* end = 0;

    if(url == 0 || path == 0)
    {
        return 0;
    }
    *path = 0;
    bgn = strstr(url, "//");
    if(bgn == 0)
    {
        return 0;
    }
    bgn += 2;
    while(*bgn != '/' && *bgn != '\0')
    {
        bgn++;
    }
    if(*bgn == '\0')
    {
        return 0;
    }
    end = bgn;
    while(*end != '\0')
    {
        end++;
    }
    *path = bgn;
    return end - bgn;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_inet_addr
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_api_inet_addr(char* str)
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
|       chttp_api_inet_ntoa
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static unsigned int chttp_api_inet_ntoa(unsigned int ip, char* pdata, unsigned int len_max)
{
    if(len_max < 16)
        return 0;
    sprintf(pdata, "%u.%u.%u.%u", \
        (unsigned char )LONG_LL(ip), (unsigned char )LONG_LH(ip), (unsigned char )LONG_HL(ip), (unsigned char )LONG_HH(ip));
    return strlen(pdata);
}

static int chttp_api_ip_cache_init(void)
{
    memset(&g_chttp_api_cache, 0x00, sizeof(chttp_api_cache_t));

    return CHTTP_ERR_OK;
}

static int chttp_api_ip_cache_update(const char* host, const char* ip)
{
    int i = 0;

    for(i=0;i<g_chttp_api_cache.nums;i++)
    {
        if(strcmp(host, g_chttp_api_cache.item[i].host) == 0)
        {
            memset(g_chttp_api_cache.item[i].ip, 0x00, 32);
            strcpy(g_chttp_api_cache.item[i].ip, ip);
            return CHTTP_ERR_OK;
        }
    }

    if(g_chttp_api_cache.nums == HTTP_IP_CACHE_NUM)
    {
        g_chttp_api_cache.nums = 0;
    }
    memset(g_chttp_api_cache.item[g_chttp_api_cache.nums].host, 0x00, 128);
    strcpy(g_chttp_api_cache.item[g_chttp_api_cache.nums].host, host);
    memset(g_chttp_api_cache.item[g_chttp_api_cache.nums].ip, 0x00, 32);
    strcpy(g_chttp_api_cache.item[g_chttp_api_cache.nums].ip, ip);
    g_chttp_api_cache.nums++;
    return CHTTP_ERR_OK;
}

static int chttp_api_ip_cache_read(const char* host, char* ip)
{
    int i = 0;

    for(i=0;i<g_chttp_api_cache.nums;i++)
    {
        if(strcmp(host, g_chttp_api_cache.item[i].host) == 0)
        {
            CHTTP_PRINTF("found cache record: %s|%s\r\n", g_chttp_api_cache.item[i].host, g_chttp_api_cache.item[i].ip);
            sprintf(ip, "%s", g_chttp_api_cache.item[i].ip);
            return CHTTP_ERR_OK;
        }
    }
    return CHTTP_ERR_FAIL;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_send
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_send(void* pmsg, int fd)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int len = 0;

    if(pmsg == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    ret = chttp_format(pmsg, &pdata, &len);
    if(ret < 0)
    {
        return ret;
    }
    return (nc_send(fd, pdata, len, 0) == len) ? CHTTP_ERR_OK : CHTTP_ERR_FAIL;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_request_body_length
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_request_body_length(void* pmsg, int* plen)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int cnt = 0;
    char* bgn = 0;

    *plen = 0;
    ret = chttp_header_find(pmsg, "Content-Length", &pdata, &cnt);
    if(ret < 0)
    {
        return ret;
    }
    bgn = strstr((char*)pdata, ": ");
    bgn += 2;
    *plen = atoi(bgn);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_request_is_json
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_request_is_json(void* pmsg)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int cnt = 0;
    char* bgn = 0;

    ret = chttp_header_find(pmsg, "Content-Type", &pdata, &cnt);
    if(ret < 0)
    {
        return ret;
    }
    bgn = strstr((char*)pdata, ": ");
    bgn += 2;
    bgn = strstr(bgn, "application/json");
    if(bgn == 0)
    {
        return CHTTP_ERR_FAIL;
    }
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_read_line
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_read_line(void* pmsg, int fd, int timeout)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int cnt = 0;
    sockfd_set_t fd_set;
    int counter = 0;

    if(pmsg == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    nc_fd_zero(&fd_set);
    nc_fd_set(fd, &fd_set);
    pdata = http_malloc(HTTP_LINE_LEN_MAX);
    memset(pdata, 0x00, HTTP_LINE_LEN_MAX);
    while(1)
    {
        if(cnt == HTTP_LINE_LEN_MAX)
        {
            break;
        }
        counter = nc_select(&fd_set, 0, 0, timeout);
        if(counter <= 0)
        {
            break;
        }
        if(!nc_fd_isset(fd, &fd_set))
        {
            break;
        }
        if(nc_recv(fd, &pdata[cnt], 1, SOCK_MSG_DONTWAIT) != 1)
        {
            break;
        }
        cnt++;
        if(pdata[cnt - 1] != '\n')
        {
            continue;
        }
        cnt -= 2;
        ret = chttp_line_set(pmsg, pdata, cnt);
        if(ret < 0)
        {
            break;
        }
        break;
    }
    http_free(pdata);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_read_header
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_read_header(void* pmsg, int fd, int timeout)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int cnt = 0;
    sockfd_set_t fd_set;
    int counter = 0;

    if(pmsg == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    nc_fd_zero(&fd_set);
    nc_fd_set(fd, &fd_set);
    pdata = http_malloc(HTTP_LINE_LEN_MAX);
    memset(pdata, 0x00, HTTP_LINE_LEN_MAX);
    while(1)
    {
        if(cnt == HTTP_LINE_LEN_MAX)
        {
            break;
        }
        counter = nc_select(&fd_set, 0, 0, timeout);
        if(counter <= 0)
        {
            break;
        }
        if(!nc_fd_isset(fd, &fd_set))
        {
            break;
        }
        if(nc_recv(fd, &pdata[cnt], 1, SOCK_MSG_DONTWAIT) != 1)
        {
            break;
        }
        cnt++;
        if(pdata[cnt - 1] != '\n')
        {
            continue;
        }
        if(cnt == 2 && memcmp(pdata, "\r\n", 2) == 0)
        {
            ret = CHTTP_ERR_OK;
            break;
        }
        cnt -= 2;
        ret = chttp_header_add(pmsg, pdata, cnt);
        if(ret < 0)
        {
            break;
        }
        memset(pdata, 0x00, HTTP_LINE_LEN_MAX);
        cnt = 0;
    }
    http_free(pdata);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_read_body
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_read_body(void* pmsg, int fd, int timeout)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int cnt = 0;
    char* pback = 0;
    int len = 0;
    sockfd_set_t fd_set;
    int counter = 0;
    int bjson = 0;
    int bjsonbgn = 0;
    int charcounter = 0;

    if(pmsg == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    ret = chttp_api_request_body_length(pmsg, &len);
    if(ret < 0)
    {
        ret = chttp_api_request_is_json(pmsg);
        if(ret < 0)
        {
            return ret;
        }
        bjson = 1;
    }
    if(len == 0 && bjson != 1)
    {
        return ret;
    }
    nc_fd_zero(&fd_set);
    nc_fd_set(fd, &fd_set);
    if(bjson == 1)
    {
        len = HTTP_BODY_BLK_SIZE;
    }
    pdata = http_malloc(len);
    while(1)
    {
        if(bjson == 1)
        {
            if(cnt == len)
            {
                pback = pdata;
                pdata = http_malloc(len + HTTP_BODY_BLK_SIZE);
                memcpy(pdata, pback, len);
                len += HTTP_BODY_BLK_SIZE;
                http_free(pback);
            }
        }
        counter = nc_select(&fd_set, 0, 0, timeout);
        if(counter <= 0)
        {
            break;
        }
        if(!nc_fd_isset(fd, &fd_set))
        {
            break;
        }
        if(nc_recv(fd, &pdata[cnt], 1, SOCK_MSG_DONTWAIT) != 1)
        {
            break;
        }
        cnt++;
        if(bjson == 1)
        {
            if(bjsonbgn == 0)
            {
                if(pdata[cnt - 1] == '{')
                {
                    bjsonbgn = 1;
                }
                else
                {
                    continue;
                }
            }
            if(pdata[cnt - 1] == '{')
            {
                charcounter++;
            }
            else if(pdata[cnt - 1] == '}')
            {
                charcounter--;
            }
            if(charcounter != 0)
            {
                continue;
            }
        }
        else
        {
            if(cnt < len)
            {
                continue;
            }
        }
        ret = chttp_body_set(pmsg, pdata, cnt);
        if(ret < 0)
        {
            break;
        }
        break;
    }
    http_free(pdata);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_recv
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
static int chttp_api_recv(void* pmsg, int fd, int timeout)
{
    int ret = CHTTP_ERR_PARAM;
    char* pdata = 0;
    int len = 0;

    if(pmsg == 0)
    {
        return CHTTP_ERR_PARAM;
    }
    ret = chttp_api_read_line(pmsg, fd, timeout);
    if(ret < 0)
    {
        return ret;
    }
    ret = chttp_api_read_header(pmsg, fd, timeout);
    if(ret < 0)
    {
        return ret;
    }
    ret = chttp_api_read_body(pmsg, fd, timeout);
    if(ret < 0)
    {
        return ret;
    }
    ret = chttp_format(pmsg, &pdata, &len);
    if(ret < 0)
    {
        return ret;
    }
    //CHTTP_DUMP(pdata, len);
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_init
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_api_init(void)
{
    mbedtls_init();
    
    chttp_init_hooks(http_malloc, http_free, 0);

    chttp_api_ip_cache_init();
    
    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_connect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_api_connect(const char* host, const char* port, int protocol, int timeout, int* fd)
{
    int ret = CHTTP_ERR_PARAM;
    int socket_id = 0;
    addrinfo_t addrinfo;
    paddrinfo_t addr_list = 0, cur = 0;
    char ip[32];
    sockaddr_in_t addr;

    CHTTP_PRINTF("chttp connect %s:%s\r\n", host, port);

    ret = chttp_api_ip_cache_read(host, ip);
    do
    {
        if(ret != CHTTP_ERR_OK)
        {
            CHTTP_PRINTF("chttp cache not found\r\n");
            break;
        }
        socket_id = nc_socket(DOMAIN_AF_INET, TYPE_SOCK_STREAM, protocol);
        if(socket_id < 0)
        {
            CHTTP_PRINTF("chttp cache ip socket error: %d\r\n", ret);
            break;
        }
        if(protocol == PROTOCOL_TLS)
        {
            ret = mbedtls_vfymode(nc_ifd(socket_id), SSL_VERIFY_OPTIONAL);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp cache ip mbedtls_vfymode error: %d\r\n", ret);
                nc_socketclose(socket_id);
                break;
            }
            ret = mbedtls_timeout(nc_ifd(socket_id), timeout);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp cache ip mbedtls_timeout error: %d\r\n", ret);
                nc_socketclose(socket_id);
                break;
            }
            ret = mbedtls_hostname(nc_ifd(socket_id), host);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp cache ip mbedtls_hostname error: %d\r\n", ret);
                nc_socketclose(socket_id);
                break;
            }
        }
        addr.sin_family = DOMAIN_AF_INET;
        addr.sin_port = HTONS(atoi(port));
        addr.sin_addr.s_addr = chttp_api_inet_addr((char*)ip);
        ret = nc_connect((int)socket_id, &addr, sizeof(sockaddr_in_t));
        if(ret < 0)
        {
            CHTTP_PRINTF("chttp cache ip connect error: %d\r\n", ret);
            nc_socketclose(socket_id);
            break;
        }
        CHTTP_PRINTF("chttp cache ip connect success\r\n");
        *fd = socket_id;
        return CHTTP_ERR_OK;
    }while(0);

    memset(&addrinfo, 0x00, sizeof(addrinfo_t));
    addrinfo.ai_flags = 0;
    addrinfo.ai_family = DOMAIN_AF_INET;
    addrinfo.ai_socktype = TYPE_SOCK_STREAM;
    addrinfo.ai_protocol = protocol;
    ret = nc_getaddrinfo((char*)host, (char*)port, &addrinfo, &addr_list);
    if (ret != 0)
    {
        CHTTP_PRINTF("chttp getaddrinfo error: %d\r\n", ret);
        return ret;
    }
    ret = -1;
    for (cur = addr_list; cur != 0; cur = cur->ai_next)
    {
        socket_id = nc_socket(cur->ai_family, cur->ai_socktype, protocol);
        if (socket_id < 0)
        {
            CHTTP_PRINTF("chttp socket error: %d\r\n", socket_id);
            ret = -1;
            continue;
        }
        if(protocol == PROTOCOL_TLS)
        {
            ret = mbedtls_vfymode(nc_ifd(socket_id), SSL_VERIFY_OPTIONAL);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp mbedtls_vfymode error: %d\r\n", ret);
                nc_socketclose(socket_id);
                return ret;
            }
            ret = mbedtls_timeout(nc_ifd(socket_id), timeout);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp mbedtls_timeout error: %d\r\n", ret);
                nc_socketclose(socket_id);
                return ret;
            }
            ret = mbedtls_hostname(nc_ifd(socket_id), host);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp mbedtls_hostname error: %d\r\n", ret);
                nc_socketclose(socket_id);
                return ret;
            }
        }
        ret = nc_connect(socket_id, cur->ai_addr, cur->ai_addrlen);
        if (ret == 0)
        {
            memset(ip, 0x00, sizeof(ip));
            chttp_api_inet_ntoa(cur->ai_addr->sin_addr.s_addr, ip, sizeof(ip));
            ret = chttp_api_ip_cache_update(host, ip);
            if(ret < 0)
            {
                CHTTP_PRINTF("chttp cache update error: %d\r\n", ret);
                nc_socketclose(socket_id);
                break;
            }
            CHTTP_PRINTF("chttp cache update success\r\n");
            break;
        }
        CHTTP_PRINTF("chttp connect error: %d\r\n", ret);
        nc_socketclose(socket_id);
    }
    nc_freeaddrinfo(addr_list);
    if (ret == 0)
    {
        CHTTP_PRINTF("chttp connect success\r\n");
        *fd = socket_id;
    }
    return ret;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_disconnect
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_api_disconnect(int fd)
{
    nc_socketclose(fd);

    return CHTTP_ERR_OK;
}

/*--------------------------------------
|   Function Name:
|       chttp_api_api
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
int chttp_api(int fd, const char* url, const char* method, const char* version, void* wr, void* rd, int timeout)
{
    int ret = CHTTP_ERR_PARAM;

    int socket_id = 0;

    char* scheme = 0;
    int scheme_len = 0;
    char* host = 0;
    int host_len = 0;
    char* port = 0;
    int port_len = 0;
    char* path = 0;
    int path_len = 0;
    char* temp = 0;
    int temp_len = 0;

    char* header = 0;
    int header_len = 0;
    char* body = 0;
    int body_len = 0;

    int protocol = PROTOCOL_DEFAULT;

    char* host_buf = 0;
    char* port_buf = 0;

    if(wr == 0 || rd == 0)
    {
        CHTTP_PRINTF("chttp api param error\r\n");
        return CHTTP_ERR_PARAM;
    }
    scheme_len = chttp_url_scheme(url, &scheme);
    host_len = chttp_url_host(url, &host);
    port_len = chttp_url_port(url, &port);
    path_len = chttp_url_path(url, &path);
    if(     \
            (scheme_len == 5 && memcmp(scheme, "HTTPS", 5) == 0) \
        ||  (scheme_len == 5 && memcmp(scheme, "https", 5) == 0) \
        )
    {
        protocol = PROTOCOL_TLS;
    }
    else if(     \
            (scheme_len == 4 && memcmp(scheme, "HTTP", 4) == 0) \
        ||  (scheme_len == 4 && memcmp(scheme, "http", 4) == 0) \
        )
    {
        protocol = PROTOCOL_TCP;
    }
    else
    {
        protocol = PROTOCOL_DEFAULT;
    }
    if(port_len == 0)
    {
        if(protocol == PROTOCOL_TLS)
        {
            port = "443";
            port_len = 3;
        }
        else
        {
            port = "80";
            port_len = 2;
        }
    }
    ret = chttp_line_get(wr, &temp, &temp_len);
    if(ret < 0)
    {
        temp = http_malloc(256);
        memset(temp, 0x00, 256);
        temp_len = 0;
        memcpy(&temp[temp_len], method, strlen(method));
        temp_len += strlen(method);
        temp[temp_len++] = ' ';
        memcpy(&temp[temp_len], path, path_len);
        temp_len += path_len;
        temp[temp_len++] = ' ';
        memcpy(&temp[temp_len], version, strlen(version));
        temp_len += strlen(version);
        chttp_line_set(wr, (const char*)temp, temp_len);
        http_free(temp);
    }
    ret = chttp_header_find(wr, "Content-Length", &header, &header_len);
    if(ret < 0)
    {
        ret = chttp_body_get(wr, &body, &body_len);
        if(ret == CHTTP_ERR_OK)
        {
            temp = http_malloc(256);
            memset(temp, 0x00, 256);
            sprintf(temp, "Content-Length: %d", body_len);
            chttp_header_add(wr, (const char*)temp, strlen(temp));
            http_free(temp);
        }
    }
    if(fd <= 0)
    {
        host_buf = http_malloc(host_len + 1);
        port_buf = http_malloc(port_len + 1);
        memset(host_buf, 0x00, host_len + 1);
        memset(port_buf, 0x00, port_len + 1);
        memcpy(host_buf, host, host_len);
        memcpy(port_buf, port, port_len);
        ret = chttp_api_connect(host_buf, port_buf, protocol, timeout, &socket_id);
        http_free(host_buf);
        http_free(port_buf);
        if(ret < 0)
        {
            CHTTP_PRINTF("chttp connect error: %d\r\n", ret);
            return ret;
        }
    }
    else
    {
        socket_id = fd;
    }
    ret = chttp_api_send(wr, socket_id);
    if(ret < 0)
    {
        CHTTP_PRINTF("chttp send error: %d\r\n", ret);
        return ret;
    }
    ret = chttp_api_recv(rd, socket_id, timeout);
    if(ret < 0)
    {
        CHTTP_PRINTF("chttp recv error: %d\r\n", ret);
        return ret;
    }
    if(fd <= 0)
    {
        ret = chttp_api_disconnect(socket_id);
        if(ret < 0)
        {
            CHTTP_PRINTF("chttp disconnect error: %d\r\n", ret);
            return ret;
        }
    }
    return ret;
}
