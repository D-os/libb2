/********************************************************************************
/
/      File:           TranslationUtils.h
/
/      Description:    Utility functions for using Translation Kit
/
/      Copyright 1998, Be Incorporated, All Rights Reserved.
/      Copyright 1995-1997, Jon Watte
/
********************************************************************************/

#if !defined(_TRANSLATION_UTILS_H)
#define _TRANSLATION_UTILS_H

#include <TranslationDefs.h>
#include <SupportDefs.h>


class BBitmap;
class BTranslatorRoster;
class BPositionIO;
class BMenu;


/* This class is a little different from many other classes. */
/* You don't create an instance of it; you just call its various */
/* static member functions for utility-like operations. */

class BTranslationUtils
{
		BTranslationUtils();
		~BTranslationUtils();
		BTranslationUtils(
				const BTranslationUtils &);
		BTranslationUtils & operator=(
				const BTranslationUtils &);

public:

/***	BITMAP getters - allocate the BBitmap; you call delete on it!	***/

		/*	Get bitmap - first try as file name, then as B_TRANSLATOR_BITMAP */
		/*	resource type from app file -- can be of any kind for which a */
		/*	translator is installed (TGA, etc) */
static	BBitmap * GetBitmap(
				const char * name,
				BTranslatorRoster * use = NULL);
		/*	Get bitmap - from app resource file only */
static	BBitmap * GetBitmap(
				uint32 type,
				int32 id,
				BTranslatorRoster * use = NULL);
static	BBitmap * GetBitmap(
				uint32 type,
				const char * name,
				BTranslatorRoster * use = NULL);
		/*	Get bitmap - from file only	*/
static	BBitmap * GetBitmapFile(
				const char * name,
				BTranslatorRoster * use = NULL);
static	BBitmap * GetBitmap(
				const entry_ref * ref,
				BTranslatorRoster * use = NULL);
		/*	Get bitmap - from open file (or BMemoryIO) */
static	BBitmap * GetBitmap(
				BPositionIO * stream,	/*	not NULL	*/
				BTranslatorRoster * use = NULL);

		/* For styled text, we can translate from a stream directly into a BTextView */
static	status_t GetStyledText(
				BPositionIO * fromStream,
				BTextView * intoView,	/*	not NULL	*/
				BTranslatorRoster * use = NULL);
		/* Saving is slightly different -- given the BTextView, a B_STYLED_TEXT_FORMAT */
		/* stream is written to intoStream. You can then call Translate() yourself to */
		/* translate that stream into something else if you want, if it is a temp file */
		/* or a BMallocIO. It's also OK to write the B_STYLED_TEXT_FORMAT to a file, */
		/* although the StyledEdit format (TEXT file + style attributes) is preferred. */
static	status_t PutStyledText(
				BTextView * fromView,
				BPositionIO * intoStream,
				BTranslatorRoster * use = NULL);
		/* This convenience function is only marginally part of the Translation Kit, */
		/* but what the hey :-) */
static	status_t WriteStyledEditFile(
				BTextView * fromView,
				BFile * intoFile);

		/* Each translator can have default settings, set by the "translations" */
		/* control panel. You can read these settings to pass on to a translator */
		/* using one of these functions. */
static	BMessage * GetDefaultSettings(
				translator_id for_translator,
				BTranslatorRoster * use = NULL);
static	BMessage * GetDefaultSettings(
				const char * translator_name,
				int32 translator_version);

		/* Envious of that "Save As" menu in ShowImage? Well, you can have your own! */
		/* AddTranslationItems will add menu items for all translations from the */
		/* basic format you specify (B_TRANSLATOR_BITMAP, B_TRANSLATOR_TEXT etc). */
		/* The translator ID and format constant chosen will be added to the message */
		/* that is sent to you when the menu item is selected. */
		enum {
			B_TRANSLATION_MENU = 'BTMN'
		};
static	status_t AddTranslationItems(
				BMenu * intoMenu,
				uint32 from_type,
				const BMessage * model = NULL,	/* default B_TRANSLATION_MENU */
				const char * translator_id_name = NULL, /* default "be:translator" */
				const char * translator_type_name = NULL, /* default "be:type" */
				BTranslatorRoster * use = NULL);
				
};

#endif /* _TRANSLATION_UTILS_H */

