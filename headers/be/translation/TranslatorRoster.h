/********************************************************************************
/
/      File:           TranslatorRoster.h
/
/      Description:    The core class of the Translation Kit.
/
/      Copyright 1998, Be Incorporated, All Rights Reserved.
/      Copyright 1995-1997, Jon Watte
/
********************************************************************************/



#ifndef _TRANSLATOR_ROSTER_H
#define _TRANSLATOR_ROSTER_H

#include <TranslationDefs.h>
#include <OS.h>
#include <StorageDefs.h>
#include <InterfaceDefs.h>
#include <Archivable.h>
#include <TranslatorFormats.h>


struct translation_format;


class BBitmap;
class BView;
class BPositionIO;
class BQuery;
class BMessage;


class BTranslatorRoster :
	public BArchivable
{
			/*	private unimplemented	*/
		BTranslatorRoster(
				const BTranslatorRoster &);
		BTranslatorRoster & operator=(
				const BTranslatorRoster &);

public:

		BTranslatorRoster();	/*	doesn't call AddTranslators()	*/
		BTranslatorRoster(
				BMessage *model);
		~BTranslatorRoster();

virtual	status_t Archive(
				BMessage *into, 
				bool deep = true) const;
static	BArchivable *Instantiate(
				BMessage *from);

static	const char *Version(	/*	returns version string	*/
				int32 * outCurVersion,	/*	current version spoken	*/
				int32 * outMinVersion,	/*	minimum version understood	*/
	/*	what app thinks it's doing	*/
				int32 inAppVersion = B_TRANSLATION_CURRENT_VERSION);

			/*	This call will return the "default" set of translators.	*/
			/*	If there is not yet a deafult set of translators, it will	*/
			/*	instantiate one and load translator add-ons from the 	*/
			/*	default location (~/config/add-ons/Datatypes, and is 	*/
			/*	modifiable with the DATATYPES environment variable)	*/

static	BTranslatorRoster *Default();	/*	you shouldn't delete this object	*/

			/*	You can pass a folder (which will be scanned for loadable	*/
			/* translators) or a specific translator (which will just be loaded)	*/
		status_t AddTranslators(		/*	establish connection	*/
				const char * load_path = NULL);	/*	NULL means default translators	*/

			/* You can add a BTranslator object you create yourself, too. */
		status_t AddTranslator(
				BTranslator * translator);

			/*	these functions call through to the translators	*/
			/*	when wantType is not 0, will only take into consideration 	*/
			/*	translators that can read input data and produce output data	*/

virtual	status_t Identify(	/*	find out what something is	*/
				BPositionIO * inSource,		/*	not NULL	*/
				BMessage * ioExtension,
				translator_info * outInfo,	/*	not NULL	*/
				uint32 inHintType = 0,
				const char * inHintMIME = NULL,
				uint32 inWantType = 0);

virtual	status_t GetTranslators(/*	find all translators for a type	*/
				BPositionIO * inSource,		/*	not NULL	*/
				BMessage * ioExtension,
				translator_info * * outInfo,/*	not NULL, call delete[] on *outInfo when done	*/
				int32 * outNumInfo,			/*	not NULL	*/
				uint32 inHintType = 0,
				const char * inHintMIME = NULL,
				uint32 inWantType = 0);

virtual	status_t GetAllTranslators(/*	find all translator IDs	*/
				translator_id * * outList,	/*	not NULL, call delete[] when done	*/
				int32 * outCount);			/*	not NULL	*/

virtual	status_t GetTranslatorInfo(/*	given a translator, get user-visible info	*/
				translator_id forTranslator,
				const char * * outName,
				const char * * outInfo,
				int32 * outVersion);

			/*	note that translators may choose to be "invisible" to	*/
			/*	the public formats, and just kick in when they 	*/
			/*	recognize a file format by its data.	*/

virtual	status_t GetInputFormats(/*	find all input formats for translator	*/
				translator_id forTranslator,
				const translation_format * * outFormats,/*	not NULL, don't write contents!	*/
				int32 * outNumFormats);		/*	not NULL	*/

virtual	status_t GetOutputFormats(/*	find all output formats for translator	*/
				translator_id forTranslator,
				const translation_format * *	outFormats,/*	not NULL, don't write contents!	*/
				int32 * outNumFormats);		/*	not NULL	*/

			/*	actually do some work	*/
			/*	Translate() and Identify() are thread safe (can be re-entered)	*/
			/*	as long as you don't call AddTranslators() or delete the object	*/
			/*	at the same time. Making sure you don't is up to you; there is	*/
			/*	no explicit lock provided.	*/

virtual	status_t Translate(	/*	morph data into form we want	*/
				BPositionIO * inSource,		/*	not NULL	*/
				const translator_info * inInfo,/*	may be NULL	*/
				BMessage * ioExtension,
				BPositionIO * outDestination,	/*	not NULL	*/
				uint32 inWantOutType,
				uint32 inHintType = 0,
				const char * inHintMIME = NULL);

virtual	status_t Translate(	/*	Make it easy to use a specific translator	*/
				translator_id inTranslator,
				BPositionIO * inSource,			/*	not NULL	*/
				BMessage * ioExtension,
				BPositionIO * outDestination,	/*	not NULL	*/
				uint32 inWantOutType);

			/*	For configuring options of the translator, a translator can support 	*/
			/*	creating a view to cofigure the translator. The translator can save 	*/
			/*	its preferences in the database or settings file as it sees fit.	*/
			/*	As a convention, the translator should always save whatever 	*/
			/*	settings are changed when the view is deleted or hidden.	*/

virtual	status_t MakeConfigurationView(
				translator_id forTranslator,
				BMessage * ioExtension,
				BView * * outView,		/*	not NULL	*/
				BRect * outExtent);		/*	not NULL	*/

			/*	For saving settings and using them later, your app can get the 	*/
			/*	current settings from a translator into a BMessage that you create 	*/
			/*	and pass in empty. Pass this message (or a copy thereof) to the 	*/
			/*	translator later in a call to Translate() to translate using 	*/
			/*	those settings.	*/

virtual	status_t GetConfigurationMessage(
				translator_id forTranslator,
				BMessage * ioExtension);

		status_t GetRefFor(
				translator_id translator,
				entry_ref * out_ref);

private:

		status_t LoadTranslator(
				const char * path);
		void LoadDir(
				const char * path,
				int32 & loadErr,
				int32 & nLoaded);
		bool CheckFormats(
				struct _XLInfo * info,
				uint32 hintType,
				const char * hintMIME,
				const translation_format * & outFormat);
		status_t AddTranslatorLocked(
				BTranslator * translator,
				image_id image);


		static void _roster_cleanup();

static	BTranslatorRoster * _defaultTranslators;
		friend class _AutoRosterDeleter;
		struct PrivateData;
		PrivateData * _private;

		/*	FBC goop	*/

		int32 _reservedTranslators[6];

virtual	void ReservedTranslatorRoster1();
virtual	void ReservedTranslatorRoster2();
virtual	void ReservedTranslatorRoster3();
virtual	void ReservedTranslatorRoster4();
virtual	void ReservedTranslatorRoster5();
virtual	void ReservedTranslatorRoster6();

};

#endif	/* _TRANSLATOR_ROSTER_H */

