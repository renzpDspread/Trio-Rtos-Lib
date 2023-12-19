/*
********************************************************************************
*
*   File Name:
*       trace.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _TRACE_MODULE
#define _TRACE_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>



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
|   Functions
+----------------------------------------------------------------------------*/

/*--------------------------------------
|   Function Name:
|       trace
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void trace(const char* fmt, ...)
{
    int len = 0;
    va_list vas;

    if(g_trace_register.cb == NULL)
    {
        return;
    }

	if(g_trace_register.suspend)
	{
		return;
	}

    private_trace_lock();

    memset(g_fmt_str, 0x00, sizeof(g_fmt_str));

    va_start(vas, fmt);

    len = vsnprintf(g_fmt_str, sizeof(g_fmt_str), fmt, vas);

    va_end(vas);

    len = strlen(g_fmt_str);

    for (int i = 0; i < len; i++) 
    {
        private_parse_trace_char(g_fmt_str[i]);
    }

    private_trace_unlock();
}


/*--------------------------------------
|   Function Name:
|       trace_dump
|   Description:
|   Parameters:
|   Returns:
+-------------------------------------*/
void trace_dump(const unsigned char* dat, int len, int rts)
{
	unsigned int times = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	char temp[16];

    if(g_trace_register.cb == NULL)
    {
        return;
    }

	if(g_trace_register.suspend)
	{
		return;
	}
	
    private_trace_lock();

	for (i = 0; i < len; i += 16)
	{
		private_printf("%08X", i);

		times = rts;
		while (times--)
		{
			private_printf("|   ");
		}

		private_printf("  ");

		memset(temp, 0x00, 16);

		if ((len - i) < 16)
		{
			for (j = 0; j < (len - i) % 16; j++)
			{
				if ((j % 4) == 0)
				{
					private_printf(" ");
				}
				private_printf("%02X ", dat[i + j]);

				if (dat[i + j] < ' ' || dat[i + j] > '~')
				{
					temp[j] = '.';
				}
				else
				{
					temp[j] = dat[i + j];
				}
			}

			for (; j < 16; j++)
			{
				if ((j % 4) == 0)
				{
					private_printf(" ");
				}
				private_printf("   ");
			}
		}
		else
		{
			for (j = 0; j < 16; j++)
			{
				if ((j % 4) == 0)
				{
					private_printf(" ");
				}
				private_printf("%02X ", dat[i + j]);

				if (dat[i + j] < ' ' || dat[i + j] > '~')
				{
					temp[j] = '.';
				}
				else
				{
					temp[j] = dat[i + j];
				}
			}
		}

		private_printf(" - ");

		if ((len - i) < 16)
		{
			private_trace_ascii(temp, (len - i) % 16);
		}
		else
		{
			private_trace_ascii(temp, 16);
		}
	}

    private_trace_unlock();
}
