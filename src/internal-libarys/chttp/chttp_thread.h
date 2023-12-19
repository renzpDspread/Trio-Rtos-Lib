/*
********************************************************************************
*
*   File Name:
*       chttp_thread.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _CHTTP_THREAD_H
#define _CHTTP_THREAD_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _CHTTP_THREAD_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef int (*chttp_thread_cb_init_t)(int ret);
typedef int (*chttp_thread_cb_connect_t)(int ret, int fd);
typedef int (*chttp_thread_cb_disconnect_t)(int ret);
typedef int (*chttp_thread_cb_api_t)(int ret, void* wr, void* rd);




/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL int chttp_thread_process(void);
GLOBAL int chttp_thread_init( \
    chttp_thread_cb_init_t cb);
GLOBAL int chttp_thread_connect( \
    const char* host, const char* port, int protocol, int timeout, int* fd, \
    chttp_thread_cb_connect_t cb);
GLOBAL int chttp_thread_disconnect(int fd, \
    chttp_thread_cb_disconnect_t cb);
GLOBAL int chttp_thread_api(int fd, const char* url, const char* method, const char* version, \
    void* wr, void* rd, int timeout, \
    chttp_thread_cb_api_t cb);

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

#endif  /* #ifndef _CHTTP_THREAD_H */
