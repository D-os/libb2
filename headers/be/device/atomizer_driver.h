/*******************************************************************************
/
/	File:			atomizer_driver.h
/
/	Description:	User-space atomizer driver API
/
/	Copyright 1999, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _ATOMIZER_DRIVER_H_
#define _ATOMIZER_DRIVER_H_

#include <Drivers.h>
#include "atomizer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	B_ATOMIZER_FIND_OR_MAKE_ATOMIZER = B_DEVICE_OP_CODES_END + 1,
	B_ATOMIZER_DELETE_ATOMIZER,
	B_ATOMIZER_ATOMIZE,
	B_ATOMIZER_STRING_FOR_TOKEN,
	B_ATOMIZER_GET_NEXT_ATOMIZER_INFO,
	B_ATOMIZER_GET_NEXT_ATOM	
};

#define ATOMIZER_PRIVATE_DATA_MAGIC	'ATOM' /* a private driver rev, of sorts */

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	const char	*name;	/* name of atomizer to search for/create */
	const void	*atomizer;	/* a token representing the atomizer */
} atomizer_find_or_make_atomizer;

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	const void	*atomizer;	/* a token representing the atomizer to delete */
} atomizer_delete_atomizer;

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	const void	*atomizer;	/* a token representing the atomizer */
	const char	*string;	/* string to atomize */
	int			create;		/* non-zero if token should be created if not already there */
	const void	*atom;		/* token representing atomized string */
} atomizer_atomize;

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	const void	*atomizer;	/* a token representing the atomizer */
	const void	*atom;		/* token representing atomized string */
	char		*string;	/* buffer to copy the string to */
	uint32		max_size;	/* maximum number of bytes to copy into buffer */
	uint32		length;		/* actuall length of the string */
} atomizer_string_for_token;

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	void		**cookie;	/* a token for keeping track of the list of atomizers */
	atomizer_info	*info;	/* pointer to info about the atomizer */
} atomizer_get_next_atomizer_info;

typedef struct {
	uint32		magic;	/* magic number to make sure the caller groks us */
	const void	*atomizer;	/* a token representing the atomizer */
	uint32		*cookie;	/* a token for keeping track of the list of atoms */
	const void	*atom;		/* the results */
} atomizer_get_next_token;

#ifdef __cplusplus
}
#endif

#endif
