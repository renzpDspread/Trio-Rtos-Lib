/*
********************************************************************************
*
*   File Name:
*       mbedtls_socket_platform.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _MBEDTLS_SOCKET_PLATFORM_H
#define _MBEDTLS_SOCKET_PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _MBEDTLS_SOCKET_PLATFORM_C
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



/*-----------------------------------------------------------------------------
|   Variables
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Constants
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   prototypes
+----------------------------------------------------------------------------*/
GLOBAL void porting_mbedtls_exit(int status);
GLOBAL mbedtls_time_t porting_mbedtls_time(mbedtls_time_t * timer);
GLOBAL int porting_mbedtls_fprintf( FILE * stream, const char * format, ... );
GLOBAL int porting_mbedtls_printf( const char * format, ... );
GLOBAL int porting_mbedtls_nv_seed_read(unsigned char * buf, size_t buf_len);
GLOBAL int porting_mbedtls_nv_seed_write(unsigned char * buf, size_t buf_len);
GLOBAL void *porting_mbedtls_calloc( size_t n, size_t size );
GLOBAL void porting_mbedtls_free( void *ptr );
GLOBAL void mbedtls_net_init( mbedtls_net_context *ctx );
GLOBAL int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto );
GLOBAL void mbedtls_net_free( mbedtls_net_context *ctx );
GLOBAL int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len );
GLOBAL int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len );
GLOBAL int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout );
GLOBAL int mbedtls_net_avail_timeout( void *ctx, uint32_t timeout);

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

#endif  /* #ifndef _MBEDTLS_SOCKET_PLATFORM_H */
