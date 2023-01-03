#ifndef _VIEW_H
#define _VIEW_H

#include <Font.h>
#include <Handler.h>
#include <InterfaceDefs.h>
#include <Point.h>
#include <Rect.h>

// view definitions
enum {
	B_PRIMARY_MOUSE_BUTTON	 = 0x01,
	B_SECONDARY_MOUSE_BUTTON = 0x02,
	B_TERTIARY_MOUSE_BUTTON	 = 0x04
};

enum {
	B_ENTERED_VIEW = 0,
	B_INSIDE_VIEW,
	B_EXITED_VIEW,
	B_OUTSIDE_VIEW
};

enum {
	B_POINTER_EVENTS  = 0x00000001,
	B_KEYBOARD_EVENTS = 0x00000002
};

enum {
	B_LOCK_WINDOW_FOCUS	 = 0x00000001,
	B_SUSPEND_VIEW_FOCUS = 0x00000002,
	B_NO_POINTER_HISTORY = 0x00000004
};

enum {
	B_TRACK_WHOLE_RECT,
	B_TRACK_RECT_CORNER
};

enum {
	B_FONT_FAMILY_AND_STYLE = 0x00000001,
	B_FONT_SIZE				= 0x00000002,
	B_FONT_SHEAR			= 0x00000004,
	B_FONT_ROTATION			= 0x00000008,
	B_FONT_SPACING			= 0x00000010,
	B_FONT_ENCODING			= 0x00000020,
	B_FONT_FACE				= 0x00000040,
	B_FONT_FLAGS			= 0x00000080,
	B_FONT_ALL				= 0x000000FF
};

const uint32 B_FULL_UPDATE_ON_RESIZE = 0x80000000UL; /* 31 */
const uint32 _B_RESERVED1_			 = 0x40000000UL; /* 30 */
const uint32 B_WILL_DRAW			 = 0x20000000UL; /* 29 */
const uint32 B_PULSE_NEEDED			 = 0x10000000UL; /* 28 */
const uint32 B_NAVIGABLE_JUMP		 = 0x08000000UL; /* 27 */
const uint32 B_FRAME_EVENTS			 = 0x04000000UL; /* 26 */
const uint32 B_NAVIGABLE			 = 0x02000000UL; /* 25 */
const uint32 B_SUBPIXEL_PRECISE		 = 0x01000000UL; /* 24 */
const uint32 B_DRAW_ON_CHILDREN		 = 0x00800000UL; /* 23 */
const uint32 B_INPUT_METHOD_AWARE	 = 0x00400000UL; /* 23 */
const uint32 _B_RESERVED7_			 = 0x00200000UL; /* 22 */

#define _RESIZE_MASK_ ~(B_FULL_UPDATE_ON_RESIZE | _B_RESERVED1_ | B_WILL_DRAW | B_PULSE_NEEDED | B_NAVIGABLE_JUMP | B_FRAME_EVENTS | B_NAVIGABLE | B_SUBPIXEL_PRECISE | B_DRAW_ON_CHILDREN | B_INPUT_METHOD_AWARE | _B_RESERVED7_)

const uint32 _VIEW_TOP_	   = 1UL;
const uint32 _VIEW_LEFT_   = 2UL;
const uint32 _VIEW_BOTTOM_ = 3UL;
const uint32 _VIEW_RIGHT_  = 4UL;
const uint32 _VIEW_CENTER_ = 5UL;

inline uint32 _rule_(uint32 r1, uint32 r2, uint32 r3, uint32 r4)
{
	return ((r1 << 12) | (r2 << 8) | (r3 << 4) | r4);
}

#define B_FOLLOW_NONE 0
#define B_FOLLOW_ALL_SIDES _rule_(_VIEW_TOP_, _VIEW_LEFT_, _VIEW_BOTTOM_, \
								  _VIEW_RIGHT_)
#define B_FOLLOW_ALL B_FOLLOW_ALL_SIDES

#define B_FOLLOW_LEFT _rule_(0, _VIEW_LEFT_, 0, _VIEW_LEFT_)
#define B_FOLLOW_RIGHT _rule_(0, _VIEW_RIGHT_, 0, _VIEW_RIGHT_)
#define B_FOLLOW_LEFT_RIGHT _rule_(0, _VIEW_LEFT_, 0, _VIEW_RIGHT_)
#define B_FOLLOW_H_CENTER _rule_(0, _VIEW_CENTER_, 0, _VIEW_CENTER_)

#define B_FOLLOW_TOP _rule_(_VIEW_TOP_, 0, _VIEW_TOP_, 0)
#define B_FOLLOW_BOTTOM _rule_(_VIEW_BOTTOM_, 0, _VIEW_BOTTOM_, 0)
#define B_FOLLOW_TOP_BOTTOM _rule_(_VIEW_TOP_, 0, _VIEW_BOTTOM_, 0)
#define B_FOLLOW_V_CENTER _rule_(_VIEW_CENTER_, 0, _VIEW_CENTER_, 0)

class BBitmap;
class BCursor;
class BPicture;
class BPolygon;
class BRegion;
class BScrollBar;
class BScrollView;
class BShape;
class BString;
class BWindow;

class BView : public BHandler
{
   public:
	BView(BRect		  frame,
		  const char *name,
		  uint32	  resizeMask,
		  uint32	  flags);
	virtual ~BView();

	BView(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;

	virtual void AttachedToWindow();
	virtual void AllAttached();
	virtual void DetachedFromWindow();
	virtual void AllDetached();

	virtual void MessageReceived(BMessage *msg);

	void   AddChild(BView *child, BView *before = nullptr);
	bool   RemoveChild(BView *child);
	int32  CountChildren() const;
	BView *ChildAt(int32 index) const;
	BView *NextSibling() const;
	BView *PreviousSibling() const;
	bool   RemoveSelf();

	BWindow *Window() const;

	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint where);
	virtual void MouseUp(BPoint where);
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage *dnd);
	virtual void WindowActivated(bool state);
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void KeyUp(const char *bytes, int32 numBytes);
	virtual void Pulse();
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);

	virtual void TargetedByScrollView(BScrollView *scroll_view);
	void		 BeginRectTracking(BRect  startRect,
								   uint32 style = B_TRACK_WHOLE_RECT);
	void		 EndRectTracking();

	void GetMouse(BPoint *location,
				  uint32 *buttons,
				  bool	  checkMessageQueue = true);

	void DragMessage(BMessage *aMessage,
					 BRect	   dragRect,
					 BHandler *reply_to = nullptr);
	void DragMessage(BMessage *aMessage,
					 BBitmap	 *anImage,
					 BPoint	   offset,
					 BHandler *reply_to = nullptr);
	void DragMessage(BMessage	  *aMessage,
					 BBitmap	 *anImage,
					 drawing_mode dragMode,
					 BPoint		  offset,
					 BHandler	  *reply_to = nullptr);

	BView *FindView(const char *name) const;
	BView *Parent() const;
	BRect  Bounds() const;
	BRect  Frame() const;
	void   ConvertToScreen(BPoint *pt) const;
	BPoint ConvertToScreen(BPoint pt) const;
	void   ConvertFromScreen(BPoint *pt) const;
	BPoint ConvertFromScreen(BPoint pt) const;
	void   ConvertToScreen(BRect *r) const;
	BRect  ConvertToScreen(BRect r) const;
	void   ConvertFromScreen(BRect *r) const;
	BRect  ConvertFromScreen(BRect r) const;
	void   ConvertToParent(BPoint *pt) const;
	BPoint ConvertToParent(BPoint pt) const;
	void   ConvertFromParent(BPoint *pt) const;
	BPoint ConvertFromParent(BPoint pt) const;
	void   ConvertToParent(BRect *r) const;
	BRect  ConvertToParent(BRect r) const;
	void   ConvertFromParent(BRect *r) const;
	BRect  ConvertFromParent(BRect r) const;
	BPoint LeftTop() const;

	void		 GetClippingRegion(BRegion *region) const;
	virtual void ConstrainClippingRegion(BRegion *region);
	void		 ClipToPicture(BPicture *picture,
							   BPoint	 where = B_ORIGIN,
							   bool		 sync  = true);
	void		 ClipToInversePicture(BPicture *picture,
									  BPoint	where = B_ORIGIN,
									  bool		sync  = true);

	virtual void SetDrawingMode(drawing_mode mode);
	drawing_mode DrawingMode() const;

	void SetBlendingMode(source_alpha srcAlpha, alpha_function alphaFunc);
	void GetBlendingMode(source_alpha *srcAlpha, alpha_function *alphaFunc) const;

	virtual void SetPenSize(float size);
	float		 PenSize() const;

	void SetViewCursor(const BCursor *cursor, bool sync = true);

	virtual void SetViewColor(rgb_color c);
	void		 SetViewColor(uchar r, uchar g, uchar b, uchar a = 255);
	rgb_color	 ViewColor() const;

	void SetViewBitmap(const BBitmap *bitmap,
					   BRect srcRect, BRect dstRect,
					   uint32 followFlags = B_FOLLOW_TOP | B_FOLLOW_LEFT,
					   uint32 options	  = B_TILE_BITMAP);
	void SetViewBitmap(const BBitmap *bitmap,
					   uint32		  followFlags = B_FOLLOW_TOP | B_FOLLOW_LEFT,
					   uint32		  options	  = B_TILE_BITMAP);
	void ClearViewBitmap();

	status_t SetViewOverlay(const BBitmap *overlay,
							BRect srcRect, BRect dstRect,
							rgb_color *colorKey,
							uint32	   followFlags = B_FOLLOW_TOP | B_FOLLOW_LEFT,
							uint32	   options	   = 0);
	status_t SetViewOverlay(const BBitmap *overlay, rgb_color *colorKey,
							uint32 followFlags = B_FOLLOW_TOP | B_FOLLOW_LEFT,
							uint32 options	   = 0);
	void	 ClearViewOverlay();

	virtual void SetHighColor(rgb_color a_color);
	void		 SetHighColor(uchar r, uchar g, uchar b, uchar a = 255);
	rgb_color	 HighColor() const;

	virtual void SetLowColor(rgb_color a_color);
	void		 SetLowColor(uchar r, uchar g, uchar b, uchar a = 255);
	rgb_color	 LowColor() const;

	void	  SetLineMode(cap_mode	lineCap,
						  join_mode lineJoin,
						  float		miterLimit = B_DEFAULT_MITER_LIMIT);
	join_mode LineJoinMode() const;
	cap_mode  LineCapMode() const;
	float	  LineMiterLimit() const;

	void   SetOrigin(BPoint pt);
	void   SetOrigin(float x, float y);
	BPoint Origin() const;

	void PushState();
	void PopState();

	void   MovePenTo(BPoint pt);
	void   MovePenTo(float x, float y);
	void   MovePenBy(float x, float y);
	BPoint PenLocation() const;
	void   StrokePoint(BPoint  pt,
					   pattern p = B_SOLID_HIGH);
	void   StrokeLine(BPoint  toPt,
					  pattern p = B_SOLID_HIGH);
	void   StrokeLine(BPoint  pt0,
					  BPoint  pt1,
					  pattern p = B_SOLID_HIGH);
	void   BeginLineArray(int32 count);
	void   AddLine(BPoint pt0, BPoint pt1, rgb_color col);
	void   EndLineArray();

	void StrokePolygon(const BPolygon *aPolygon,
					   bool			   closed = true,
					   pattern		   p	  = B_SOLID_HIGH);
	void StrokePolygon(const BPoint *ptArray,
					   int32		 numPts,
					   bool			 closed = true,
					   pattern		 p		= B_SOLID_HIGH);
	void StrokePolygon(const BPoint *ptArray,
					   int32		 numPts,
					   BRect		 bounds,
					   bool			 closed = true,
					   pattern		 p		= B_SOLID_HIGH);
	void FillPolygon(const BPolygon *aPolygon,
					 pattern		 p = B_SOLID_HIGH);
	void FillPolygon(const BPoint *ptArray,
					 int32		   numPts,
					 pattern	   p = B_SOLID_HIGH);
	void FillPolygon(const BPoint *ptArray,
					 int32		   numPts,
					 BRect		   bounds,
					 pattern	   p = B_SOLID_HIGH);

	void StrokeTriangle(BPoint	pt1,
						BPoint	pt2,
						BPoint	pt3,
						BRect	bounds,
						pattern p = B_SOLID_HIGH);
	void StrokeTriangle(BPoint	pt1,
						BPoint	pt2,
						BPoint	pt3,
						pattern p = B_SOLID_HIGH);
	void FillTriangle(BPoint  pt1,
					  BPoint  pt2,
					  BPoint  pt3,
					  pattern p = B_SOLID_HIGH);
	void FillTriangle(BPoint  pt1,
					  BPoint  pt2,
					  BPoint  pt3,
					  BRect	  bounds,
					  pattern p = B_SOLID_HIGH);

	void StrokeRect(BRect r, pattern p = B_SOLID_HIGH);
	void FillRect(BRect r, pattern p = B_SOLID_HIGH);
	void FillRegion(BRegion *a_region, pattern p = B_SOLID_HIGH);
	void InvertRect(BRect r);

	void StrokeRoundRect(BRect	 r,
						 float	 xRadius,
						 float	 yRadius,
						 pattern p = B_SOLID_HIGH);
	void FillRoundRect(BRect   r,
					   float   xRadius,
					   float   yRadius,
					   pattern p = B_SOLID_HIGH);

	void StrokeEllipse(BPoint  center,
					   float   xRadius,
					   float   yRadius,
					   pattern p = B_SOLID_HIGH);
	void StrokeEllipse(BRect r, pattern p = B_SOLID_HIGH);
	void FillEllipse(BPoint	 center,
					 float	 xRadius,
					 float	 yRadius,
					 pattern p = B_SOLID_HIGH);
	void FillEllipse(BRect r, pattern p = B_SOLID_HIGH);

	void StrokeArc(BPoint  center,
				   float   xRadius,
				   float   yRadius,
				   float   start_angle,
				   float   arc_angle,
				   pattern p = B_SOLID_HIGH);
	void StrokeArc(BRect   r,
				   float   start_angle,
				   float   arc_angle,
				   pattern p = B_SOLID_HIGH);
	void FillArc(BPoint	 center,
				 float	 xRadius,
				 float	 yRadius,
				 float	 start_angle,
				 float	 arc_angle,
				 pattern p = B_SOLID_HIGH);
	void FillArc(BRect	 r,
				 float	 start_angle,
				 float	 arc_angle,
				 pattern p = B_SOLID_HIGH);

	void StrokeBezier(BPoint *controlPoints,
					  pattern p = B_SOLID_HIGH);
	void FillBezier(BPoint *controlPoints,
					pattern p = B_SOLID_HIGH);

	void StrokeShape(BShape *shape,
					 pattern p = B_SOLID_HIGH);
	void FillShape(BShape *shape,
				   pattern p = B_SOLID_HIGH);

	void CopyBits(BRect src, BRect dst);
	void DrawBitmapAsync(const BBitmap *aBitmap,
						 BRect			srcRect,
						 BRect			dstRect);
	void DrawBitmapAsync(const BBitmap *aBitmap);
	void DrawBitmapAsync(const BBitmap *aBitmap, BPoint where);
	void DrawBitmapAsync(const BBitmap *aBitmap, BRect dstRect);
	void DrawBitmap(const BBitmap *aBitmap,
					BRect		   srcRect,
					BRect		   dstRect);
	void DrawBitmap(const BBitmap *aBitmap);
	void DrawBitmap(const BBitmap *aBitmap, BPoint where);
	void DrawBitmap(const BBitmap *aBitmap, BRect dstRect);

	void DrawChar(char aChar);
	void DrawChar(char aChar, BPoint location);
	void DrawString(const char	   *aString,
					escapement_delta *delta = nullptr);
	void DrawString(const char *aString, BPoint location,
					escapement_delta *delta = nullptr);
	void DrawString(const char *aString, int32 length,
					escapement_delta *delta = nullptr);
	void DrawString(const char	   *aString,
					int32			  length,
					BPoint			  location,
					escapement_delta *delta = 0L);

	virtual void SetFont(const BFont *font, uint32 mask = B_FONT_ALL);
	void		 GetFont(BFont *font) const;

	void  TruncateString(BString *in_out,
						 uint32	  mode,
						 float	  width) const;
	float StringWidth(const char *string) const;
	float StringWidth(const char *string, int32 length) const;
	void  GetStringWidths(char *stringArray[],
						  int32 lengthArray[],
						  int32 numStrings,
						  float widthArray[]) const;
	void  SetFontSize(float size);
	void  ForceFontAliasing(bool enable);
	void  GetFontHeight(font_height *height) const;

	void Invalidate(BRect invalRect);
	void Invalidate(const BRegion *invalRegion);
	void Invalidate();

	void SetDiskMode(char *filename, long offset);

	void	  BeginPicture(BPicture *a_picture);
	void	  AppendToPicture(BPicture *a_picture);
	BPicture *EndPicture();

	void DrawPicture(const BPicture *a_picture);
	void DrawPicture(const BPicture *a_picture, BPoint where);
	void DrawPicture(const char *filename, long offset, BPoint where);
	void DrawPictureAsync(const BPicture *a_picture);
	void DrawPictureAsync(const BPicture *a_picture, BPoint where);
	void DrawPictureAsync(const char *filename, long offset, BPoint where);

	status_t SetEventMask(uint32 mask, uint32 options = 0);
	uint32	 EventMask();
	status_t SetMouseEventMask(uint32 mask, uint32 options = 0);

	virtual void SetFlags(uint32 flags);
	uint32		 Flags() const;
	virtual void SetResizingMode(uint32 mode);
	uint32		 ResizingMode() const;
	void		 MoveBy(float dh, float dv);
	void		 MoveTo(BPoint where);
	void		 MoveTo(float x, float y);
	void		 ResizeBy(float dh, float dv);
	void		 ResizeTo(float width, float height);
	void		 ScrollBy(float dh, float dv);
	void		 ScrollTo(float x, float y);
	virtual void ScrollTo(BPoint where);
	virtual void MakeFocus(bool focusState = true);
	bool		 IsFocus() const;

	virtual void Show();
	virtual void Hide();
	bool		 IsHidden() const;
	bool		 IsHidden(const BView *looking_from) const;

	void Flush() const;
	void Sync() const;

	virtual void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	BScrollBar *ScrollBar(orientation posture) const;

	virtual BHandler *ResolveSpecifier(BMessage	*msg,
									   int32	   index,
									   BMessage	*specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	bool IsPrinting() const;
	void SetScale(float scale) const;

	virtual void DrawAfterChildren(BRect updateRect);

   private:
	class impl;
	// friend class BScrollBar;
	friend class BWindow;
	// friend class BBitmap;
	// friend class BPrintJob;
	// friend class BShelf;
	friend class BTab;

	BView(const BView &);
	BView &operator=(const BView &);

	void _attach(BWindow *);
	void _detach();
	void _damage_rect(BRect r);

	uint32		fFlags;
	BPoint		fParentOffset;
	BRect		fBounds;
	BWindow	*fOwner;
	BView	  *fParent;
	BView	  *fNextSibling;
	BView	  *fPrevSibling;
	BView	  *fFirstChild;
	int16		fShowLevel;
	uint32		fEventMask;
	uint32		fEventOptions;
	uint32		fMouseEventMask;
	uint32		fMouseEventOptions;

	pimpl<impl> fState;
};

// inline definitions

inline void BView::ScrollTo(float x, float y)
{
	ScrollTo(BPoint(x, y));
}

inline void BView::SetViewColor(uchar r, uchar g, uchar b, uchar a)
{
	rgb_color a_color;
	a_color.red	  = r;
	a_color.green = g;
	a_color.blue  = b;
	a_color.alpha = a;
	SetViewColor(a_color);
}

inline void BView::SetHighColor(uchar r, uchar g, uchar b, uchar a)
{
	rgb_color a_color;
	a_color.red	  = r;
	a_color.green = g;
	a_color.blue  = b;
	a_color.alpha = a;
	SetHighColor(a_color);
}

inline void BView::SetLowColor(uchar r, uchar g, uchar b, uchar a)
{
	rgb_color a_color;
	a_color.red	  = r;
	a_color.green = g;
	a_color.blue  = b;
	a_color.alpha = a;
	SetLowColor(a_color);
}

#endif /* _VIEW_H */
