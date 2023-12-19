/*
********************************************************************************
*
*   File Name:
*       dymem.c
*   Author:
*       SW R&D Department
*   Version:
*       V1.0
*   Description:
*
*
********************************************************************************
*/

#define _DYMEM_MODULE
#define _DYMEM_C

/*-----------------------------------------------------------------------------
|   Includes
+----------------------------------------------------------------------------*/
#include "dymem.h"



/*-----------------------------------------------------------------------------
|   Macros
+----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
|   Enumerations
+----------------------------------------------------------------------------*/
#define MAGIC1    0x55AA55AA
#define MAGIC2    0xAA55AA55



/*-----------------------------------------------------------------------------
|   Typedefs
+----------------------------------------------------------------------------*/
typedef struct _dymem_priv_t dymem_priv_t;

struct _dymem_priv_t
{
	unsigned int magic1;
	unsigned int free;
	unsigned int size;
	dymem_priv_t* last;
	dymem_priv_t* next;
	unsigned int magic2;
};



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
|	Function Name:
|		dymem_copy
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
static void dymem_copy(char* dst, char* src, unsigned int size)
{
	unsigned int i = 0;

	for(i=0;i<size;i++)
	{
		dst[i] = src[i];
	}
}

/*--------------------------------------
|	Function Name:
|		dymem_generate
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
void* dymem_generate(char* buffer, unsigned int size)
{
	dymem_priv_t* dymem_priv = 0;

	if ((unsigned int)buffer % 4 != 0) 
	{
		buffer = buffer + 4 - (unsigned int)buffer % 4;
		size = size - 4 + (unsigned int)buffer % 4;
	}

	size = ((size % 4) == 0) ? (size) : (size - 4 + size % 4);

	if (size <= sizeof(dymem_priv_t)) 
	{
		return 0;
	}

	dymem_priv = (dymem_priv_t*)buffer;

	dymem_priv->magic1 = MAGIC1;
	dymem_priv->size = size - sizeof(dymem_priv_t);
	dymem_priv->free = 1;
	dymem_priv->last = 0;
	dymem_priv->next = 0;
	dymem_priv->magic2 = MAGIC2;

	return (void*)dymem_priv;
}

/*--------------------------------------
|	Function Name:
|		dymem_alloc
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
void* dymem_alloc(void* priv, unsigned int size)
{
	dymem_priv_t* dymem_priv = 0;
	dymem_priv_t* dymem_priv_child = 0;

	size = (size >= 16) ? size : 16;
	size = ((size % 4) == 0) ? (size) : (size + 4 - size % 4);

	dymem_priv = (dymem_priv_t*)priv;

	while (dymem_priv != 0) 
	{
		if (dymem_priv->magic1 != MAGIC1 \
			|| dymem_priv->magic2 != MAGIC2 \
			)
		{
			return 0;
		}

		if (dymem_priv->free == 1) 
		{
			if (dymem_priv->size > size + sizeof(dymem_priv_t)) 
			{
				dymem_priv_child = (dymem_priv_t*)((unsigned int)dymem_priv + sizeof(dymem_priv_t) + size);
				dymem_priv_child->magic1 = MAGIC1;
				dymem_priv_child->free = 1;
				dymem_priv_child->size = dymem_priv->size - size - sizeof(dymem_priv_t);
				dymem_priv_child->last = dymem_priv;
				dymem_priv_child->next = dymem_priv->next;
				dymem_priv_child->magic2 = MAGIC2;
				dymem_priv->free = 0;
				dymem_priv->size = size;
				dymem_priv->next = dymem_priv_child;
				return (void*)((unsigned int)dymem_priv + sizeof(dymem_priv_t));
			}
		}

		dymem_priv = dymem_priv->next;
	}

	return 0;
}

/*--------------------------------------
|	Function Name:
|		dymem_realloc
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
void* dymem_realloc(void* priv, void* buffer, unsigned int size)
{
	dymem_priv_t* dymem_priv = 0;
	void* buffer_new = 0;

	buffer_new = dymem_alloc(priv, size);

	dymem_priv = (dymem_priv_t*)((unsigned int)buffer - sizeof(dymem_priv_t));
	if(dymem_priv->size > size)
	{
		dymem_copy(buffer_new, buffer, size);
	}
	else
	{
		dymem_copy(buffer_new, buffer, dymem_priv->size);
	}

	dymem_free(priv, buffer);

	return buffer_new;
}

/*--------------------------------------
|	Function Name:
|		dymem_free
|	Description:
|	Parameters:
|	Returns:
+-------------------------------------*/
void dymem_free(void* priv, void* buffer)
{
	dymem_priv_t* dymem_priv = 0;

	dymem_priv = (dymem_priv_t*)((unsigned int)buffer - sizeof(dymem_priv_t));

	dymem_priv->free = 1;

	dymem_priv = (dymem_priv_t*)priv;

	while (dymem_priv != 0)
	{
		if (dymem_priv->magic1 != MAGIC1 \
			|| dymem_priv->magic2 != MAGIC2 \
			)
		{
			return;
		}

		if (dymem_priv->free == 1 && dymem_priv->next != 0 && dymem_priv->next->free == 1)
		{
			dymem_priv->size = dymem_priv->size + sizeof(dymem_priv_t) + dymem_priv->next->size;
			dymem_priv->next = dymem_priv->next->next;
			continue;
		}

		dymem_priv = dymem_priv->next;
	}
}
