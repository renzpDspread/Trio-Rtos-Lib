/*
********************************************************************************
*
*   File Name:
*       config.h
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/

#ifdef _CONFIG_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define TRACE_LVL_ERROR             (1)
#define TRACE_LVL_NOTICE            (3)
#define TRACE_LVL_DEFAULT           (5)
#define TRACE_LVL_INFO              (9)
#define TRACE_LVL_DRIVER            (11)
#define TRACE_LVL_TEMP              (13)
#define TRACE_LVL_SYS               (15)
//;
;

#define CFG_TRACE_DBG                       (1)
#define CFG_TRACE_DELAY                     (1)
#define NET_CONTAINER_DBG                   (1)
#define MBEDTLS_SOCK_DBG                    (1)
#define CHTTP_DBG                           (1)

#if (CFG_TRACE_DBG || NET_CONTAINER_DBG|| MBEDTLS_SOCK_DBG||CHTTP_DBG)
extern void Dsp_Lib_Trace(const char *str, ...);
extern void Dsp_Lib_PrintHex(unsigned char *data, unsigned int len);
extern void DelayMs(unsigned short Ms);
#endif

#if CFG_TRACE_DBG
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define TRACE_DELAY(lv, fmt, ...)             {Dsp_Lib_Trace("[%s.%d][%s]",__FILENAME__, __LINE__, __FUNCTION__); \
                                        Dsp_Lib_Trace(fmt, ##__VA_ARGS__);Dsp_Lib_Trace("\r\n");DelayMs(lv);}
//#define TRACE(lv, fmt, ...)             {Dsp_Lib_Trace("[%s.%d][%s]",__FILENAME__, __LINE__, __FUNCTION__); \
                                        Dsp_Lib_Trace(fmt, ##__VA_ARGS__);Dsp_Lib_Trace("\r\n");}

#define TRACE(lv, fmt, ...)             Dsp_Lib_Trace(fmt, ##__VA_ARGS__);
#define TRACE_DUMP(lv, pbuf, cnt)       Dsp_Lib_PrintHex(pbuf, cnt)
#else
#define TRACE(lv, fmt, ...)             ((void)0)
#define TRACE_DELAY(lv, fmt, ...)       ((void)0)
#define TRACE_DUMP(lv, pbuf, cnt)       ((void)0)
#endif
    

/*NET*/
#if NET_CONTAINER_DBG
#define NET_CONTAINER_PRINTF(fmt, ...)      {Dsp_Lib_Trace(fmt, ##__VA_ARGS__);Dsp_Lib_Trace("\r\n");}
#define NET_CONTAINER_DUMP(pbuf, cnt)       Dsp_Lib_PrintHex(pbuf, cnt)
#else
#define NET_CONTAINER_PRINTF(fmt, ...)      ((void)0)
#define NET_CONTAINER_DUMP(pbuf, cnt)       ((void)0)
#endif


/*MBEDTLS*/
#if MBEDTLS_SOCK_DBG
#define MBEDTLS_SOCK_PRINTF(fmt, ...)  {Dsp_Lib_Trace(fmt, ##__VA_ARGS__);Dsp_Lib_Trace("\r\n");}
#define MBEDTLS_SOCK_DUMP(pbuf, cnt)   Dsp_Lib_PrintHex(pbuf, cnt)
#else
#define MBEDTLS_SOCK_PRINTF(fmt, ...)  ((void)0)
#define MBEDTLS_SOCK_DUMP(pbuf, cnt)   ((void)0)
#endif

#define MBEDTLS_INNER_TRACE_LVL        (0)

#define MBEDTLS_SOCK_NUM               (3)

#define MBEDTLS_HEAP_SIZE              (1024*96)

#define MBEDTLS_SESSION                (1)

#define MBEDTLS_CACHE_NUM              (10)


/*CHTTP*/
#if CHTTP_DBG
#define CHTTP_PRINTF(fmt, ...)      {Dsp_Lib_Trace(fmt, ##__VA_ARGS__);Dsp_Lib_Trace("\r\n");}
#define CHTTP_DUMP(pbuf, cnt)       Dsp_Lib_PrintHex((const unsigned char* )pbuf, cnt, 0)
#else
#define CHTTP_PRINTF(fmt, ...)      ((void)0)
#define CHTTP_DUMP(pbuf, cnt)       ((void)0)
#endif



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

#endif  /* #ifndef _CONFIG_H */
