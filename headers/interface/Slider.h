#ifndef _SLIDER_H
#define _SLIDER_H

#include <Control.h>

enum hash_mark_location {
	B_HASH_MARKS_NONE	= 0,
	B_HASH_MARKS_TOP	= 1,
	B_HASH_MARKS_LEFT	= 1,
	B_HASH_MARKS_BOTTOM = 2,
	B_HASH_MARKS_RIGHT	= 2,
	B_HASH_MARKS_BOTH	= 3
};

enum thumb_style {
	B_BLOCK_THUMB,
	B_TRIANGLE_THUMB
};

class BSlider : public BControl
{
   public:
	BSlider(BRect		frame,
			const char *name,
			const char *label,
			BMessage   *message,
			int32		minValue,
			int32		maxValue,
			thumb_style thumbType	 = B_BLOCK_THUMB,
			uint32		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32		flags		 = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
	BSlider(BRect		frame,
			const char *name,
			const char *label,
			BMessage   *message,
			int32		minValue,
			int32		maxValue,
			orientation posture /*= B_HORIZONTAL*/,
			thumb_style thumbType	 = B_BLOCK_THUMB,
			uint32		resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32		flags		 = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
	virtual ~BSlider();

	BSlider(BMessage *data);
	static BArchivable *Instantiate(BMessage *data);
	virtual status_t	Archive(BMessage *data, bool deep = true) const;
	virtual status_t	Perform(perform_code d, void *arg);

	virtual void WindowActivated(bool state);
	virtual void AttachedToWindow();
	virtual void AllAttached();
	virtual void AllDetached();
	virtual void DetachedFromWindow();

	virtual void MessageReceived(BMessage *msg);
	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float w, float h);
	virtual void KeyDown(const char *bytes, int32 n);
	virtual void KeyUp(const char *bytes, int32 n);
	virtual void MouseDown(BPoint pt);
	virtual void MouseUp(BPoint pt);
	virtual void MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd);
	virtual void Pulse();

	virtual void  SetLabel(const char *label);
	virtual void  SetLimitLabels(const char *minLabel, const char *maxLabel);
	const char	 *MinLimitLabel() const;
	const char	 *MaxLimitLabel() const;
	virtual void  SetValue(int32);
	virtual int32 ValueForPoint(BPoint) const;
	virtual void  SetPosition(float);
	float		  Position() const;
	virtual void  SetEnabled(bool on);
	void		  GetLimits(int32 *minimum, int32 *maximum);

	virtual void  Draw(BRect);
	virtual void  DrawSlider();
	virtual void  DrawBar();
	virtual void  DrawHashMarks();
	virtual void  DrawThumb();
	virtual void  DrawFocusMark();
	virtual void  DrawText();
	virtual char *UpdateText() const;

	virtual BRect BarFrame() const;
	virtual BRect HashMarksFrame() const;
	virtual BRect ThumbFrame() const;

	virtual void SetFlags(uint32 flags);
	virtual void SetResizingMode(uint32 mode);

	virtual void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	virtual status_t  Invoke(BMessage *msg = nullptr);
	virtual BHandler *ResolveSpecifier(BMessage	  *msg,
									   int32	   index,
									   BMessage	  *specifier,
									   int32	   form,
									   const char *property);
	virtual status_t  GetSupportedSuites(BMessage *data);

	virtual void SetModificationMessage(BMessage *message);
	BMessage	*ModificationMessage() const;

	virtual void SetSnoozeAmount(int32);
	int32		 SnoozeAmount() const;

	virtual void SetKeyIncrementValue(int32 value);
	int32		 KeyIncrementValue() const;

	virtual void SetHashMarkCount(int32 count);
	int32		 HashMarkCount() const;

	virtual void	   SetHashMarks(hash_mark_location where);
	hash_mark_location HashMarks() const;

	virtual void SetStyle(thumb_style s);
	thumb_style	 Style() const;

	virtual void SetBarColor(rgb_color);
	rgb_color	 BarColor() const;
	virtual void UseFillColor(bool, const rgb_color *c = nullptr);
	bool		 FillColor(rgb_color *) const;

	BView *OffscreenView() const;

	orientation	 Orientation() const;
	virtual void SetOrientation(orientation);

	float		 BarThickness() const;
	virtual void SetBarThickness(float thickness);

	virtual void SetFont(const BFont *font, uint32 properties = B_FONT_ALL);

   private:
	void _DrawBlockThumb();
	void _DrawTriangleThumb();

	float _MinPosition() const;
	float _MaxPosition() const;

	bool _ConstrainPoint(BPoint &point, BPoint comparePoint) const;

	BSlider &operator=(const BSlider &);

	BMessage *fModificationMessage;
	int32	  fSnoozeAmount;

	rgb_color fBarColor;
	rgb_color fFillColor;
	bool	  fUseFillColor;

	char *fMinLimitStr;
	char *fMaxLimitStr;

	int32 fMinValue;
	int32 fMaxValue;
	int32 fKeyIncrementValue;

	int32			   fHashMarkCount;
	hash_mark_location fHashMarks;

	// BBitmap *fOffScreenBits;
	// BView	*fOffScreenView;

	thumb_style fStyle;

	BPoint fLocation;
	BPoint fInitialLocation;

	orientation fOrientation;
	float		fBarThickness;
};

#endif /* _SLIDER_H */
