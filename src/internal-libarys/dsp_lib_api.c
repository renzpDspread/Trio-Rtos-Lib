
/*
********************************************************************************
*
*   File Name:
*       dsp_lib_api.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <liteos_api.h>
#include "liteMapi.h"

/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/
#define DSP_LIB_V1 (1)
#define DSP_LIB_V2 (0)

#define MAX_STRING_LENGTH (512)

/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/
static int g_enable_log = 0;


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

extern void mbedtls_version_get_string(char *string);

int dsp_lib_ver(char *inbuf, int inBufMaxLen)
{
    char buf[32] = { 0 };
    char buf2[32] = { 0 };
    memset(buf, 0, sizeof(buf));
    memset(buf2, 0, sizeof(buf2));

    mbedtls_version_get_string(buf);

    sprintf(buf2, "%s.%s", LIB_VER);

    memcpy(inbuf, buf2, inBufMaxLen);

    return inBufMaxLen;
}

void dsp_lib_enable_log(int isEnable)
{
	g_enable_log = isEnable;
}

void Dsp_Lib_Trace(const char *fmt, ...)
{
	va_list varg;
	int ret;
	char strBuff[MAX_STRING_LENGTH];

	if (g_enable_log == 0)
		return;

	memset(strBuff, 0, sizeof(strBuff));

    va_start( varg, fmt );                  
    ret = vsprintf(strBuff,  fmt,  varg); 
    va_end( varg );
	if(ret >= sizeof(strBuff)){
		strBuff[sizeof(strBuff)-1] = 0;
	}
	
	usb_debug(strBuff);
}

void Dsp_Lib_PrintHex(unsigned char *data, uint len)
{
    uint i;
    char hex[80];
	char disp[20];

	if (len <= 0 || g_enable_log == 0)
		return;

	memset(hex, 0, sizeof(hex));
	memset(disp, 0, sizeof(disp));
    for(i=0; i<len; ){
		sprintf(hex + strlen(hex), "%02X ", data[i]);
		disp[i%16] = data[i];
		if(data[i] < 0x20 || data[i] > 0x7E ) {
			disp[i%16] = '.';
		}
        i++;
        if(!(i%16)) {
			strcat(hex, " : ");
			strcat(hex, disp);
			strcat(hex, "\r\n");
            usb_debug(hex);
            //DelayMs(10);
			memset(hex, 0, sizeof(hex));
			memset(disp, 0, sizeof(disp));
			//usb_debug("\r\n======i=%d===len=%d=\r\n", i, len);
		}
		
	}
	strcat(hex, " : ");
	strcat(hex, disp);
	strcat(hex, "\r\n");
    usb_debug(hex);
    //DelayMs(10);
}