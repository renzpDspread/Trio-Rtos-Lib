/*
********************************************************************************
*
*   File Name:
*       dspapi_tls.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _DSPAPI_TLS_H
#define _DSPAPI_TLS_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _DSPAPI_TLS_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
typedef enum
{
    SSL_VERIFY_START = 0,

    SSL_VERIFY_NONE = SSL_VERIFY_START,
    SSL_VERIFY_OPTIONAL,
    SSL_VERIFY_REQUIRED,
    SSL_VERIFY_UNSET,

    SSL_VERIFY_NUMS
}SSL_VER_TYPE;



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _tls_sock_in_addr_t
{
    unsigned int     s_addr;
}tls_sock_in_addr_t, *p_tls_sock_in_addr_t;

typedef const tls_sock_in_addr_t     *pctls_sock_in_addr_t;

typedef struct _tls_sockaddr_in_t
{
    unsigned short sin_family;
    unsigned short sin_port;
    tls_sock_in_addr_t sin_addr;
    char sin_zero[8];
}tls_sockaddr_in_t, *ptls_sockaddr_in_t;

typedef const tls_sockaddr_in_t     *pctls_sockaddr_in_t;

/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL int dsp_tls_init(void);
GLOBAL int dsp_tls_cacert(const unsigned char *pdata, int len);
GLOBAL int dsp_tls_clikey(const unsigned char* pdata, int len);
GLOBAL int dsp_tls_clicert(const unsigned char* pdata, int len);
GLOBAL int dsp_tls_vfymode(int fd, int mode);
GLOBAL int dsp_tls_timeout(int fd, int timeout);
GLOBAL int dsp_tls_hostname(int fd, const char *phostname);

GLOBAL int dsp_tls_socket(void);
GLOBAL int dsp_tls_connect(int fd, char* host_or_ip, char* port);
GLOBAL int dsp_tls_send(int fd, const void* buf, int len, int flags);
GLOBAL int dsp_tls_recv(int fd, void* buf, int len, int flags);
GLOBAL int dsp_tls_socketclose(int fd);
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

#endif  /* #ifndef _DSPAPI_TLS_H */
