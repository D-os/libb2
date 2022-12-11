#ifndef _BITMAP_H
#define _BITMAP_H

#include <Archivable.h>
#include <InterfaceDefs.h>
#include <Rect.h>
#include <View.h>

class BWindow;

enum {
	B_BITMAP_CLEAR_TO_WHITE			 = 0x00000001,
	B_BITMAP_ACCEPTS_VIEWS			 = 0x00000002,
	B_BITMAP_IS_AREA				 = 0x00000004,
	B_BITMAP_IS_LOCKED				 = 0x00000008 | B_BITMAP_IS_AREA,
	B_BITMAP_IS_CONTIGUOUS			 = 0x00000010 | B_BITMAP_IS_LOCKED,
	B_BITMAP_IS_OFFSCREEN			 = 0x00000020,
	B_BITMAP_WILL_OVERLAY			 = 0x00000040 | B_BITMAP_IS_OFFSCREEN,
	B_BITMAP_RESERVE_OVERLAY_CHANNEL = 0x00000080
};

#define B_ANY_BYTES_PER_ROW -1

class BBitmap : public BArchivable
{
   public:
	BBitmap(BRect		bounds,
			uint32		flags,
			color_space depth,
			int32		bytesPerRow = B_ANY_BYTES_PER_ROW,
			screen_id	screenID	= B_MAIN_SCREEN_ID);
	BBitmap(BRect		bounds,
			color_space depth,
			bool		accepts_views	= false,
			bool		need_contiguous = false);
	BBitmap(const BBitmap *source,
			bool		   accepts_views   = false,
			bool		   need_contiguous = false);
	virtual ~BBitmap();

	// Archiving
	BBitmap(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	status_t InitCheck() const;
	bool	 IsValid() const;

	status_t LockBits(uint32 *state = nullptr);
	void	 UnlockBits();

	area_id		Area() const;
	void	   *Bits() const;
	int32		BitsLength() const;
	int32		BytesPerRow() const;
	color_space ColorSpace() const;
	BRect		Bounds() const;

	void SetBits(const void *data,
				 int32		 length,
				 int32		 offset,
				 color_space cs);

	status_t GetOverlayRestrictions(overlay_restrictions *restrict) const;

	// to mimic a BWindow
	virtual void AddChild(BView *view);
	virtual bool RemoveChild(BView *view);
	int32		 CountChildren() const;
	BView		*ChildAt(int32 index) const;
	BView		*FindView(const char *view_name) const;
	BView		*FindView(BPoint point) const;
	bool		 Lock();
	void		 Unlock();
	bool		 IsLocked() const;

   private:
	friend class BView;
	friend class BApplication;

	BBitmap(const BBitmap &);
	BBitmap &operator=(const BBitmap &);
};

#endif /* _BITMAP_H */
