/******************************************************************************
/
/	File:			Bitmap.h
/
/	Description:	BBitmap objects represent off-screen windows that
/					contain bitmap data.
/
/	Copyright 1992-99, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/


#ifndef	_BITMAP_H
#define	_BITMAP_H

#include <BeBuild.h>
#include <Archivable.h>
#include <InterfaceDefs.h>
#include <Rect.h>

class BWindow;

enum {
	B_BITMAP_CLEAR_TO_WHITE				= 0x00000001,
	B_BITMAP_ACCEPTS_VIEWS				= 0x00000002,
	B_BITMAP_IS_AREA					= 0x00000004,
	B_BITMAP_IS_LOCKED					= 0x00000008 | B_BITMAP_IS_AREA,
	B_BITMAP_IS_CONTIGUOUS				= 0x00000010 | B_BITMAP_IS_LOCKED,
	B_BITMAP_IS_OFFSCREEN				= 0x00000020,
	B_BITMAP_WILL_OVERLAY				= 0x00000040 | B_BITMAP_IS_OFFSCREEN,
	B_BITMAP_RESERVE_OVERLAY_CHANNEL	= 0x00000080
};

#define B_ANY_BYTES_PER_ROW -1

/*----------------------------------------------------------------*/
/*----- BBitmap class --------------------------------------------*/

class BBitmap : public BArchivable {

public:
					BBitmap(BRect bounds,
							uint32 flags,
							color_space depth,
							int32 bytesPerRow=B_ANY_BYTES_PER_ROW,
							screen_id screenID=B_MAIN_SCREEN_ID);
					BBitmap(BRect bounds,
							color_space depth,
							bool accepts_views = false,
							bool need_contiguous = false);
					BBitmap(const BBitmap* source,
							bool accepts_views = false,
							bool need_contiguous = false);
virtual				~BBitmap();

/* Archiving */
					BBitmap(BMessage *data);
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

		status_t	InitCheck() const;
		bool		IsValid() const;

		status_t	LockBits(uint32 *state=NULL);
		void		UnlockBits();

		area_id		Area() const;
		void *		Bits() const;
		int32		BitsLength() const;
		int32		BytesPerRow() const;
		color_space	ColorSpace() const;
		BRect		Bounds() const;

		void		SetBits(const void *data,
							int32 length,
							int32 offset,
							color_space cs);


		status_t	GetOverlayRestrictions(overlay_restrictions *restrict) const;

/* to mimic a BWindow */
virtual	void		AddChild(BView *view);
virtual	bool		RemoveChild(BView *view);
		int32		CountChildren() const;
		BView		*ChildAt(int32 index) const;
		BView		*FindView(const char *view_name) const;
		BView		*FindView(BPoint point) const;
		bool		Lock();
		void		Unlock();
		bool		IsLocked() const;

/*----- Private or reserved -----------------------------------------*/
	
virtual status_t	Perform(perform_code d, void *arg);

private:
friend class BView;
friend class BApplication;
friend void  _IMPEXP_BE _get_screen_bitmap_(BBitmap *,BRect,bool);

virtual	void		_ReservedBitmap1();
virtual	void		_ReservedBitmap2();
virtual	void		_ReservedBitmap3();

					BBitmap(const BBitmap &);
		BBitmap		&operator=(const BBitmap &);

		char		*get_shared_pointer() const;
		void		set_bits(long offset, char *data, long length);
		void		set_bits_24(long offset, char *data, long length);
		void		set_bits_24_local_gray(long offset, char *data, long len);
		void		set_bits_24_local_256(long offset, uchar *data, long len);
		void		set_bits_24_24(long offset, char *data, long length,
									bool big_endian_dst);
		void		set_bits_8_24(long offset, char *data, long length,
									bool big_endian_dst);
		void		set_bits_gray_24(long offset, char *data, long length,
									bool big_endian_dst);
		int32		get_server_token() const;
		void 		InitObject(	BRect frame, color_space depth,
								uint32 flags, int32 bytesPerRow, screen_id screenID);
		void		AssertPtr();

		void		*fBasePtr;
		int32		fSize;
		color_space	fType;
		BRect		fBound;
		int32		fRowBytes;
		BWindow		*fWindow;
		int32		fServerToken;
		int32		fToken;
		uint8		unused;
		area_id		fArea;
		area_id		fOrigArea;
		uint32		fFlags;
		status_t	fInitError;
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _BITMAP_H */
