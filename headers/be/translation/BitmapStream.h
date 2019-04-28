/********************************************************************************
/
/      File:           BitmapStream.h
/
/      Description:    BPositionIO subclass to read/write bitmap format to/from 
/                      BBitmap instance.
/
/      Copyright 1998, Be Incorporated, All Rights Reserved.
/      Copyright 1995-1997, Jon Watte
/
********************************************************************************/

#if !defined(_BITMAP_STREAM_H)
#define _BITMAP_STREAM_H

#include <BeBuild.h>
#include <TranslationDefs.h>
#include <DataIO.h>
#include <TranslatorFormats.h>


class BBitmap;


class BBitmapStream :
	public BPositionIO
{
public:
		BBitmapStream(
				BBitmap *		map = NULL);
		~BBitmapStream();

	/* Overrides from BPositionIO */

		ssize_t ReadAt(
				off_t			pos,
				void *			buffer,
				size_t			size);
		ssize_t WriteAt(
				off_t			pos,
				const void *	data,
				size_t			size);
		off_t Seek(
				off_t			position,
				uint32			whence);
		off_t Position() const;
		off_t Size() const;
		status_t SetSize(
				off_t			size);

		status_t DetachBitmap(
				BBitmap * *		outMap);	/*	not NULL	*/

protected:
		TranslatorBitmap fHeader;
		BBitmap * fMap;
		size_t fPosition;
		size_t fSize;
		bool fDetached;

		void SwapHeader(
				const TranslatorBitmap *	source,
				TranslatorBitmap *			destination);
private:

virtual	void _ReservedBitmapStream1();
virtual void _ReservedBitmapStream2();

		TranslatorBitmap * fSwappedHeader; /* always in big-endian format */
		long _reservedBitmapStream[5];
};

#endif /* _BITMAP_STREAM_H */

