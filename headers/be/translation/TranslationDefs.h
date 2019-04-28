/********************************************************************************
/
/      File:           TranslationDefs.h
/
/      Description:    Miscellaneous basic definitions for the Translation Kit
/
/      Copyright 1998, Be Incorporated, All Rights Reserved.
/      Copyright 1995-1997, Jon Watte
/
********************************************************************************/

#if !defined(_TRANSLATION_DEFS_H)
#define _TRANSLATION_DEFS_H

#include <BeBuild.h>
#include <SupportDefs.h>
#include <TranslationErrors.h>


typedef unsigned long translator_id;


/*	when you export this struct, end with an empty	*/
/*	record that has 0 for "type"	*/


/*	These are defines, because they reflect the state at which the app was compiled	*/
#define B_TRANSLATION_CURRENT_VERSION	B_BEOS_VERSION
#define B_TRANSLATION_MIN_VERSION		161

extern const char * B_TRANSLATOR_MIME_TYPE;

struct translation_format {
	uint32		type;				/* B_ASCII_TYPE, ...*/
	uint32		group;				/* B_TRANSLATOR_BITMAP, B_TRANSLATOR_TEXT, ...*/
	float		quality;			/* format quality 0.0-1.0*/
	float		capability;			/* translator capability 0.0-1.0*/
	char		MIME[251];			/* MIME string*/
	char		name[251];			/* only descriptive	*/
};


/*	This struct is different from the format struct for a reason:	*/
/*	to separate the notion of formats from the notion of translations	*/

struct translator_info {			/* Info about a specific translation*/
	uint32			type;			/* B_ASCII_TYPE, ...*/
	translator_id	translator;		/* Filled in by BTranslationRoster*/
	uint32			group;			/* B_TRANSLATOR_BITMAP, B_TRANSLATOR_TEXT, ...*/
	float			quality;		/* Quality of format in group 0.0-1.0*/
	float			capability;		/* How much of the format do we do? 0.0-1.0*/
	char			name[251];
	char			MIME[251];
};

#endif /* _TRANSLATION_DEFS_H */
