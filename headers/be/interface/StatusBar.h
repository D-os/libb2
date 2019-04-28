/*******************************************************************************
/
/	File:			StatusBar.h
/
/   Description:    BStatusBar displays a "percentage-of-completion" gauge.
/
/	Copyright 1996-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#ifndef	_STATUS_BAR_H
#define	_STATUS_BAR_H

#include <BeBuild.h>
#include <View.h>

/*----------------------------------------------------------------*/
/*----- BStatusBar class -----------------------------------------*/

class BStatusBar : public BView {

public:
					BStatusBar(	BRect frame,
								const char *name,
								const char *label = NULL,
								const char *trailing_label = NULL);
					BStatusBar(BMessage *data);
virtual				~BStatusBar();
static	BArchivable	*Instantiate(BMessage *data);
virtual	status_t	Archive(BMessage *data, bool deep = true) const;

virtual	void		AttachedToWindow();
virtual	void		MessageReceived(BMessage *msg);
virtual	void		Draw(BRect updateRect);

virtual	void		SetBarColor(rgb_color color);
virtual	void		SetBarHeight(float height);
virtual	void		SetText(const char *str);
virtual	void		SetTrailingText(const char *str);
virtual	void		SetMaxValue(float max);

virtual	void		Update(	float delta,
							const char *main_text = NULL,
							const char *trailing_text = NULL);
virtual	void		Reset(	const char *label = NULL,
							const char *trailing_label = NULL);

		float		CurrentValue() const;
		float		MaxValue() const;
		rgb_color	BarColor() const;
		float		BarHeight() const;
		const char	*Text() const;
		const char	*TrailingText() const;
		const char	*Label() const;
		const char	*TrailingLabel() const;

virtual	void		MouseDown(BPoint pt);
virtual	void		MouseUp(BPoint pt);
virtual	void		WindowActivated(bool state);
virtual	void		MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
virtual	void		DetachedFromWindow();
virtual	void		FrameMoved(BPoint new_position);
virtual	void		FrameResized(float new_width, float new_height);

virtual BHandler	*ResolveSpecifier(BMessage *msg,
									int32 index,
									BMessage *specifier,
									int32 form,
									const char *property);

virtual void		ResizeToPreferred();
virtual void		GetPreferredSize(float *width, float *height);
virtual void		MakeFocus(bool state = true);
virtual void		AllAttached();
virtual void		AllDetached();
virtual status_t	GetSupportedSuites(BMessage *data);

/*----- Private or reserved -----------------------------------------*/
virtual status_t	Perform(perform_code d, void *arg);

private:

virtual	void		_ReservedStatusBar1();
virtual	void		_ReservedStatusBar2();
virtual	void		_ReservedStatusBar3();
virtual	void		_ReservedStatusBar4();

		BStatusBar	&operator=(const BStatusBar &);

		void		InitObject(const char *l, const char *aux_l);
		void		SetTextData(char **pp, const char *str);
		void		FillBar(BRect r);
		void		Resize();
		void		_Draw(BRect updateRect, bool bar_only);

		char		*fLabel;
		char		*fTrailingLabel;
		char		*fText;
		char		*fTrailingText;
		float		fMax;
		float		fCurrent;
		float		fBarHeight;
		float		fTrailingWidth;
		rgb_color	fBarColor;
		float		fEraseText;
		float		fEraseTrailingText;
		bool		fCustomBarHeight;
		bool		_pad1;
		bool		_pad2;
		bool		_pad3;
		uint32		_reserved[3];		// was 4
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _STATUS_BAR_H */
