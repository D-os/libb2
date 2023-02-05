#include "Slider.h"

#define LOG_TAG "BSlider"

#include <log/log.h>

#define SLIDER_TEXT_PADDING 4.0
#define SLIDER_HASH_MARKS_SIZE 6.0

BSlider::BSlider(BRect frame, const char *name, const char *label, BMessage *message, int32 minValue, int32 maxValue,
				 thumb_style thumbType, uint32 resizingMode, uint32 flags)
	: BSlider(frame, name, label, message, minValue, maxValue, B_HORIZONTAL, thumbType, resizingMode, flags) {}

BSlider::BSlider(BRect frame, const char *name, const char *label, BMessage *message, int32 minValue, int32 maxValue,
				 orientation posture, thumb_style thumbType, uint32 resizingMode, uint32 flags)
	: BControl(frame, name, label, message, resizingMode, flags),
	  fModificationMessage{nullptr},
	  fSnoozeAmount{20000},
	  fBarColor{ui_color(B_PANEL_BACKGROUND_COLOR)},
	  fFillColor{},
	  fUseFillColor{false},
	  fMinLimitStr{nullptr},
	  fMaxLimitStr{nullptr},
	  fMinValue{minValue},
	  fMaxValue{maxValue},
	  fKeyIncrementValue{1},
	  fHashMarkCount{0},
	  fHashMarks{B_HASH_MARKS_NONE},
	  fStyle{thumbType},
	  fOrientation{posture},
	  fBarThickness{6.0}
{
}

BSlider::~BSlider()
{
	delete fModificationMessage;
	free(fMinLimitStr);
	free(fMaxLimitStr);
}

BSlider::BSlider(BMessage *data) : BControl(data) {}

status_t BSlider::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BSlider::Perform(perform_code d, void *arg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BSlider::WindowActivated(bool state)
{
	BControl::WindowActivated(state);
}

void BSlider::AttachedToWindow()
{
	ResizeToPreferred();

	BControl::AttachedToWindow();

	BView *view = OffscreenView();
	if (view && view->LockLooper()) {
		view->SetViewColor(Parent()->ViewColor());
		view->UnlockLooper();
	}

	// make sure the value is within valid bounds and update position
	int32 value = Value();
	BControl::SetValueNoUpdate(value - 1);
	SetValue(value);
}

void BSlider::AllAttached()
{
	BControl::AllAttached();
}

void BSlider::AllDetached()
{
	BControl::AllDetached();
}

void BSlider::DetachedFromWindow()
{
	BControl::DetachedFromWindow();
}

void BSlider::MessageReceived(BMessage *msg)
{
	BControl::MessageReceived(msg);
}

void BSlider::FrameMoved(BPoint new_position)
{
	BControl::FrameMoved(new_position);
}

void BSlider::FrameResized(float w, float h)
{
	BControl::FrameResized(w, h);
}

void BSlider::KeyDown(const char *bytes, int32 numBytes)
{
	if (!IsEnabled() || IsHidden())
		return;

	int32 newValue = Value();

	switch (bytes[0]) {
		case B_LEFT_ARROW:
		case B_DOWN_ARROW:
			newValue -= KeyIncrementValue();
			break;

		case B_RIGHT_ARROW:
		case B_UP_ARROW:
			newValue += KeyIncrementValue();
			break;

		case B_HOME:
			newValue = fMinValue;
			break;

		case B_END:
			newValue = fMaxValue;
			break;

		default:
			BControl::KeyDown(bytes, numBytes);
			return;
	}

	if (newValue < fMinValue)
		newValue = fMinValue;

	if (newValue > fMaxValue)
		newValue = fMaxValue;

	if (newValue != Value()) {
		fInitialLocation = fLocation;
		SetValue(newValue);
		InvokeNotify(ModificationMessage(), B_CONTROL_MODIFIED);
	}
}

void BSlider::KeyUp(const char *bytes, int32 n)
{
	if (fInitialLocation != fLocation) {
		// The last KeyDown event triggered the modification message or no
		// notification at all, we may also have sent the modification message
		// continually while the user kept pressing the key. In either case,
		// finish with the final message to make the behavior consistent with
		// changing the value by mouse.
		Invoke();
	}
}

void BSlider::MouseDown(BPoint pt)
{
	if (!IsEnabled())
		return;

	if (BarFrame().Contains(pt) || ThumbFrame().Contains(pt))
		fInitialLocation = fLocation;

	_ConstrainPoint(pt, fInitialLocation);
	SetValue(ValueForPoint(pt));

	if (fLocation != fInitialLocation)
		InvokeNotify(ModificationMessage(), B_CONTROL_MODIFIED);

	SetTracking(true);
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
}
void BSlider::MouseUp(BPoint pt)
{
	if (IsTracking()) {
		if (fLocation != fInitialLocation)
			Invoke();

		SetTracking(false);
	}
	else
		BControl::MouseUp(pt);
}

void BSlider::MouseMoved(BPoint pt, uint32 transit, const BMessage *dnd)
{
	if (IsTracking()) {
		if (_ConstrainPoint(pt, fLocation)) {
			int32 value = ValueForPoint(pt);
			if (value != Value()) {
				SetValue(value);
				InvokeNotify(ModificationMessage(), B_CONTROL_MODIFIED);
			}
		}
	}
	else
		BControl::MouseMoved(pt, transit, dnd);
}

void BSlider::Pulse()
{
	BControl::Pulse();
}

void BSlider::SetLabel(const char *label)
{
	BControl::SetLabel(label);
}

void BSlider::SetLimitLabels(const char *minLabel, const char *maxLabel)
{
	free(fMinLimitStr);
	fMinLimitStr = minLabel ? strdup(minLabel) : nullptr;

	free(fMaxLimitStr);
	fMaxLimitStr = maxLabel ? strdup(maxLabel) : nullptr;

	Invalidate();
}

const char *BSlider::MinLimitLabel() const
{
	return fMinLimitStr;
}

const char *BSlider::MaxLimitLabel() const
{
	return fMaxLimitStr;
}

void BSlider::SetValue(int32 value)
{
	if (value < fMinValue)
		value = fMinValue;

	if (value > fMaxValue)
		value = fMaxValue;

	if (value == Value())
		return;

	BPoint loc;
	float  range = (float)(fMaxValue - fMinValue);
	if (range == 0)
		range = 1;

	float pos = (float)(value - fMinValue) / range * (_MaxPosition() - _MinPosition());

	switch (fOrientation) {
		case B_HORIZONTAL:
			loc.x = ceil(_MinPosition() + pos);
			loc.y = 0;
			break;
		case B_VERTICAL:
			loc.x = 0;
			loc.y = floor(_MaxPosition() - pos);
			break;
	}
	fLocation = loc;

	BRect oldThumbFrame = ThumbFrame();

	BControl::SetValueNoUpdate(value);

	BRect invalid = oldThumbFrame | ThumbFrame();

	if (fUseFillColor)
		invalid = invalid | BarFrame();

	Invalidate(invalid);
}

int32 BSlider::ValueForPoint(BPoint location) const
{
	float min;
	float max;
	float position;
	switch (fOrientation) {
		case B_HORIZONTAL:
			min		 = _MinPosition();
			max		 = _MaxPosition();
			position = location.x;
			break;
		case B_VERTICAL:
			max		 = _MinPosition();
			min		 = _MaxPosition();
			position = min + (max - location.y);
			break;
	}

	if (position < min)
		position = min;

	if (position > max)
		position = max;

	return roundf(((position - min) * (fMaxValue - fMinValue) / (max - min)) + fMinValue);
}

void BSlider::SetPosition(float position)
{
	if (position <= 0.0f)
		SetValue(fMinValue);
	else if (position >= 1.0f)
		SetValue(fMaxValue);
	else
		SetValue(position * (fMaxValue - fMinValue) + fMinValue);
}

float BSlider::Position() const
{
	float range = (float)(fMaxValue - fMinValue);
	if (range == 0.0f)
		range = 1.0f;

	return (float)(Value() - fMinValue) / range;
}

void BSlider::SetEnabled(bool on)
{
	BControl::SetEnabled(on);
}

void BSlider::GetLimits(int32 *minimum, int32 *maximum)
{
	if (minimum)
		*minimum = fMinValue;

	if (maximum)
		*maximum = fMaxValue;
}

#pragma mark - drawing

void BSlider::Draw(BRect)
{
	DrawSlider();
}

void BSlider::DrawSlider()
{
	if (LockLooper()) {
		auto view = OffscreenView();
		view->PushState();
		view->SetLineMode(B_SQUARE_CAP, B_BEVEL_JOIN);

		DrawBar();
		DrawHashMarks();
		DrawThumb();
		DrawFocusMark();
		DrawText();

		OffscreenView()->PopState();

		UnlockLooper();
	}
}

void BSlider::DrawBar()
{
	BRect  frame = BarFrame();
	BView *view	 = OffscreenView();

	auto hi = view->HighColor();

	auto shadow_color = tint_color(view->ViewColor(), B_DARKEN_2_TINT);

	view->SetHighColor(ui_color(B_SHINE_COLOR));
	view->StrokeRoundRect(frame, 2, 2);
	view->SetHighColor(shadow_color);
	frame.right -= 1;
	frame.bottom -= 1;
	view->StrokeRoundRect(frame, 2, 2);
	frame.left += 1;
	frame.top += 1;
	view->MovePenTo(frame.LeftBottom());
	view->StrokeLine(frame.LeftTop());
	view->StrokeLine(frame.RightTop());
	view->SetHighColor(ui_color(B_SHINE_COLOR));
	view->StrokeLine(frame.RightBottom());
	view->StrokeLine(frame.LeftBottom());

	view->SetHighColor(ui_color(B_SHADOW_COLOR));
	view->StrokeRoundRect(frame, 2, 2);

	frame.InsetBy(1, 1);
	if (fUseFillColor) {
		float split		 = floorf(Position() * (_MaxPosition() - _MinPosition()) + _MinPosition());
		BRect fill_frame = frame;
		BRect bar_frame	 = frame;
		switch (fOrientation) {
			case B_HORIZONTAL:
				fill_frame.right = std::min(fill_frame.right, split);
				bar_frame.left	 = std::max(bar_frame.left, split);
				break;
			case B_VERTICAL:
				fill_frame.bottom = std::min(fill_frame.bottom, split);
				bar_frame.top	  = std::max(bar_frame.top, split);
				break;
		}
		if (bar_frame.IsValid()) {
			view->SetHighColor(fBarColor);
			view->FillRect(bar_frame);
		}
		if (fill_frame.IsValid()) {
			view->SetHighColor(fFillColor);
			view->FillRect(fill_frame);
		}
	}
	else {
		view->SetHighColor(fBarColor);
		view->FillRect(frame);
	}

	view->SetHighColor(hi);
}

void BSlider::DrawHashMarks()
{
	if (fHashMarks == B_HASH_MARKS_NONE || fHashMarkCount <= 0)
		return;

	BRect  frame = HashMarksFrame();
	BView *view	 = OffscreenView();

	auto hi = view->HighColor();

	BRect bar_frame	   = BarFrame();
	auto  shadow_color = tint_color(view->ViewColor(), B_DARKEN_2_TINT);

	float min	= _MinPosition();
	float max	= _MaxPosition();
	float width = max - min;
	float pos	= floorf(min);
	float delta = width / (fHashMarkCount - 1);

	auto drawMark = [&](BPoint &p1, BPoint &p2) {
		view->SetHighColor(ui_color(B_SHADOW_COLOR));
		view->StrokeLine(p1, p2, B_SOLID_HIGH);
		view->SetHighColor(shadow_color);
		view->MovePenTo(p1 + BPoint{1, -1});
		view->StrokeLine(p2 + BPoint{1, 1});
		view->StrokeLine(p2 + BPoint{-1, 1});
		view->SetHighColor(ui_color(B_SHINE_COLOR));
		view->StrokeLine(p1 + BPoint{-1, -1});
		view->StrokeLine(p1 + BPoint{1, -1});
	};

	auto count = fHashMarkCount;
	while (count-- > 0) {
		auto p = floorf(pos);

		if (fHashMarks == B_HASH_MARKS_TOP || fHashMarks == B_HASH_MARKS_LEFT || fHashMarks == B_HASH_MARKS_BOTH) {
			BPoint p1, p2;
			switch (fOrientation) {
				case B_HORIZONTAL:
					p1.x = p;
					p1.y = frame.top + 1;
					p2.x = p;
					p2.y = bar_frame.top - 2;
					break;
				case B_VERTICAL:
					p1.x = frame.left + 1;
					p1.y = p;
					p2.x = bar_frame.left - 2;
					p2.y = p;
					break;
			}
			drawMark(p1, p2);
		}
		if (fHashMarks == B_HASH_MARKS_BOTTOM || fHashMarks == B_HASH_MARKS_RIGHT || fHashMarks == B_HASH_MARKS_BOTH) {
			BPoint p1, p2;
			switch (fOrientation) {
				case B_HORIZONTAL:
					p1.x = p;
					p1.y = bar_frame.bottom + 2;
					p2.x = p;
					p2.y = frame.bottom - 1;
					break;
				case B_VERTICAL:
					p1.x = bar_frame.right + 2;
					p1.y = p;
					p2.x = frame.right - 1;
					p2.y = p;
					break;
			}
			drawMark(p1, p2);
		}

		pos += delta;
	}

	view->SetHighColor(hi);
}

void BSlider::DrawThumb()
{
	switch (fStyle) {
		case B_BLOCK_THUMB:
			return _DrawBlockThumb();
		case B_TRIANGLE_THUMB:
			return _DrawTriangleThumb();
	}
}

void BSlider::DrawFocusMark()
{
	if (!IsFocus())
		return;

	BRect  frame = ThumbFrame();
	BView *view	 = OffscreenView();

	view->SetHighColor(ui_color(B_KEYBOARD_NAVIGATION_COLOR));

	switch (fStyle) {
		case B_BLOCK_THUMB:
			frame.left += 2.0f;
			frame.top += 2.0f;
			frame.right -= 3.0f;
			frame.bottom -= 3.0f;
			view->StrokeRect(frame);
			break;
		case B_TRIANGLE_THUMB:
			switch (fOrientation) {
				case B_HORIZONTAL:
					break;
					view->StrokeLine(BPoint(frame.left, frame.bottom + 2.0f),
									 BPoint(frame.right, frame.bottom + 2.0f));
				case B_VERTICAL:
					view->StrokeLine(BPoint(frame.left - 2.0f, frame.top),
									 BPoint(frame.left - 2.0f, frame.bottom));
					break;
			}
			break;
	}
}

void BSlider::DrawText()
{
	BRect  bounds(Bounds());
	BView *view = OffscreenView();

	font_height fontHeight;
	GetFontHeight(&fontHeight);

	switch (fOrientation) {
		case B_HORIZONTAL:
			if (Label()) {
				view->DrawString(Label(), BPoint(0.0f, ceilf(fontHeight.ascent)));
			}

			if (UpdateText()) {
				view->DrawString(UpdateText(),
								 BPoint(bounds.right - StringWidth(UpdateText()), ceilf(fontHeight.ascent)));
			}

		if (fMinLimitStr) {
				view->DrawString(fMinLimitStr, BPoint(0.0f, bounds.bottom - fontHeight.descent));
		}

		if (fMaxLimitStr) {
				view->DrawString(fMaxLimitStr,
								 BPoint(bounds.right - StringWidth(fMaxLimitStr), bounds.bottom - fontHeight.descent));
		}
		break;

		case B_VERTICAL: {
		float lineHeight = ceilf(fontHeight.ascent) + ceilf(fontHeight.descent) + ceilf(fontHeight.leading);
		float baseLine = ceilf(fontHeight.ascent);

		if (Label()) {
				view->DrawString(Label(),
								 BPoint((bounds.Width() - StringWidth(Label())) / 2.0, baseLine));
				baseLine += lineHeight;
		}

		if (fMaxLimitStr) {
				view->DrawString(fMaxLimitStr,
								 BPoint((bounds.Width() - StringWidth(fMaxLimitStr)) / 2.0, baseLine));
		}

		baseLine = bounds.bottom - ceilf(fontHeight.descent);

		if (fMinLimitStr) {
				view->DrawString(fMinLimitStr,
								 BPoint((bounds.Width() - StringWidth(fMinLimitStr)) / 2.0, baseLine));
				baseLine -= lineHeight;
		}

		if (UpdateText()) {
				view->DrawString(UpdateText(),
								 BPoint((bounds.Width() - StringWidth(UpdateText())) / 2.0, baseLine));
		}
		} break;
	}
}

#pragma mark -

char *BSlider::UpdateText() const
{
	return nullptr;
}

BRect BSlider::BarFrame() const
{
	BRect frame(Bounds());

	font_height fontHeight;
	GetFontHeight(&fontHeight);

	float textHeight = ceilf(fontHeight.ascent) + ceilf(fontHeight.descent);
	float leading	 = ceilf(fontHeight.leading);

	float thumbInset = 0.0;
	switch (fStyle) {
		case B_BLOCK_THUMB:
		thumbInset = 8.0;
		break;

		case B_TRIANGLE_THUMB:
		thumbInset = 7.0;
		break;
	}

	switch (fOrientation) {
		case B_HORIZONTAL:
		frame.left = thumbInset;
		frame.top  = SLIDER_HASH_MARKS_SIZE + (Label() || UpdateText() ? textHeight + SLIDER_TEXT_PADDING - ceilf(fontHeight.descent) : 0.0);
		frame.right -= thumbInset;
		frame.bottom = frame.top + fBarThickness;
		break;

		case B_VERTICAL:
		frame.left = floorf((frame.Width() - fBarThickness) / 2.0);
		frame.top  = thumbInset;
		if (Label())
				frame.top += textHeight;

		if (fMaxLimitStr) {
				frame.top += textHeight;
				if (Label())
					frame.top += leading;
		}

		frame.right	 = frame.left + fBarThickness;
		frame.bottom = frame.bottom - thumbInset;
		if (fMinLimitStr)
				frame.bottom -= textHeight;

		if (UpdateText()) {
				frame.bottom -= textHeight;
				if (fMinLimitStr)
					frame.bottom -= leading;
		}
		break;
	}

	return frame;
}

BRect BSlider::HashMarksFrame() const
{
	BRect frame(BarFrame());

	switch (fOrientation) {
		case B_HORIZONTAL:
		frame.top -= SLIDER_HASH_MARKS_SIZE;
		frame.bottom += SLIDER_HASH_MARKS_SIZE;
		break;
		case B_VERTICAL:
		frame.left -= SLIDER_HASH_MARKS_SIZE;
		frame.right += SLIDER_HASH_MARKS_SIZE;
		break;
	}

	return frame;
}

BRect BSlider::ThumbFrame() const
{
	// TODO: The slider looks really ugly and broken when it is too little.
	// I would suggest using BarFrame() here to get the top and bottom coords
	// and spread them further apart for the thumb

	BRect frame = Bounds();

	font_height fontHeight;
	GetFontHeight(&fontHeight);

	float textHeight = ceilf(fontHeight.ascent) + ceilf(fontHeight.descent);

	switch (fStyle) {
		case B_BLOCK_THUMB:
		switch (fOrientation) {
				case B_HORIZONTAL:
					frame.left	 = floorf(Position() * (_MaxPosition() - _MinPosition()) + _MinPosition()) - 8;
					frame.top	 = 2 + (Label() || UpdateText() ? textHeight - ceilf(fontHeight.descent) + SLIDER_TEXT_PADDING : 0);
					frame.right	 = frame.left + 17;
					frame.bottom = frame.top + fBarThickness + 7;
					break;
				case B_VERTICAL:
					frame.left	 = floor((frame.Width() - fBarThickness) / 2) - 3;
					frame.top	 = floorf(Position() * (_MaxPosition() - _MinPosition()) + _MinPosition()) - 8;
					frame.right	 = frame.left + fBarThickness + 7;
					frame.bottom = frame.top + 17;
					break;
		}
		break;
		case B_TRIANGLE_THUMB:
		switch (fOrientation) {
				case B_HORIZONTAL:
					frame.left	 = floorf(Position() * (_MaxPosition() - _MinPosition()) + _MinPosition()) - 6;
					frame.right	 = frame.left + 12;
					frame.top	 = 3 + fBarThickness + (Label() ? textHeight + SLIDER_TEXT_PADDING - ceilf(fontHeight.descent) : 0);
					frame.bottom = frame.top + 8;
					break;
				case B_VERTICAL:
					frame.left	 = floorf((frame.Width() + fBarThickness) / 2) - 3;
					frame.top	 = floorf(Position() * (_MaxPosition() - _MinPosition())) + _MinPosition() - 6;
					frame.right	 = frame.left + 8;
					frame.bottom = frame.top + 12;
					break;
		}
		break;
	}

	return frame;
}

void BSlider::SetFlags(uint32 flags)
{
	BControl::SetFlags(flags);
}

void BSlider::SetResizingMode(uint32 mode)
{
	BControl::SetResizingMode(mode);
}

void BSlider::GetPreferredSize(float *_width, float *_height)
{
	if (!_width && !_height)
		return;

	font_height fontHeight;
	GetFontHeight(&fontHeight);
	float lineHeightNoLeading = ceilf(fontHeight.ascent) + ceilf(fontHeight.descent);
	float lineHeight = lineHeightNoLeading + ceilf(fontHeight.leading);

	float width	 = 0.0f;
	float height = 0.0f;

	switch (fOrientation) {
		case B_HORIZONTAL: {
		height = 2 * SLIDER_HASH_MARKS_SIZE + fBarThickness;

		float labelWidth   = 0;
		float labelSpacing = StringWidth("M") * 2;
		if (Label()) {
				labelWidth = StringWidth(Label());
				height += lineHeightNoLeading + SLIDER_TEXT_PADDING - ceilf(fontHeight.descent);
		}

		if (fMinLimitStr)
				width = StringWidth(fMinLimitStr);

		if (fMaxLimitStr) {
				// some space between the labels
				if (fMinLimitStr)
					width += labelSpacing;

				width += StringWidth(fMaxLimitStr);
		}

		if (labelWidth > width)
				width = labelWidth;

		if (width < 32.0f)
				width = 32.0f;

		if (fMinLimitStr || fMaxLimitStr)
				height += lineHeightNoLeading + SLIDER_TEXT_PADDING;

		if (_width) {
				// NOTE: For compatibility reasons, a horizontal BSlider
				// never shrinks horizontally. This only affects applications
				// which do not use the new layout system.
				*_width = std::max(Bounds().Width(), width);
		}

			if (_height)
				*_height = height;
		} break;
		case B_VERTICAL: {
			// B_VERTICAL
			width  = 2 * SLIDER_HASH_MARKS_SIZE + fBarThickness;
			height = 32.0f;

			// find largest label
			float labelWidth = 0;
			if (Label()) {
				labelWidth = StringWidth(Label());
				height += lineHeightNoLeading;
			}
			if (fMaxLimitStr) {
				labelWidth = std::max(labelWidth, StringWidth(fMaxLimitStr));
				height += Label() ? lineHeight : lineHeightNoLeading;
			}
			if (fMinLimitStr) {
				labelWidth = std::max(labelWidth, StringWidth(fMinLimitStr));
				height += lineHeightNoLeading;
			}

			width = std::max(labelWidth, width);

			if (_width)
				*_width = width;

			if (_height) {
				// NOTE: Similarly, a vertical BSlider never shrinks
				// vertically. This only affects applications which do not
				// use the new layout system.
				*_height = std::max(Bounds().Height(), height);
			}
		} break;
	}
}

void BSlider::ResizeToPreferred()
{
	BControl::ResizeToPreferred();
}

status_t BSlider::Invoke(BMessage *msg)
{
	return BControl::Invoke(msg);
}

BHandler *BSlider::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t BSlider::GetSupportedSuites(BMessage *data)
{
	return BControl::GetSupportedSuites(data);
}

void BSlider::SetModificationMessage(BMessage *message)
{
	delete fModificationMessage;
	fModificationMessage = message;
}

BMessage *BSlider::ModificationMessage() const
{
	return fModificationMessage;
}

void BSlider::SetSnoozeAmount(int32 snoozeTime)
{
	if (snoozeTime < 10000)
		snoozeTime = 10000;
	else if (snoozeTime > 1000000)
		snoozeTime = 1000000;

	fSnoozeAmount = snoozeTime;
}

int32 BSlider::SnoozeAmount() const
{
	return fSnoozeAmount;
}

void BSlider::SetKeyIncrementValue(int32 incrementValue)
{
	fKeyIncrementValue = incrementValue;
}

int32 BSlider::KeyIncrementValue() const
{
	return fKeyIncrementValue;
}

void BSlider::SetHashMarkCount(int32 count)
{
	fHashMarkCount = count;
	Invalidate();
}

int32 BSlider::HashMarkCount() const
{
	return fHashMarkCount;
}

void BSlider::SetHashMarks(hash_mark_location where)
{
	fHashMarks = where;
	Invalidate();
}

hash_mark_location BSlider::HashMarks() const
{
	return fHashMarks;
}

void BSlider::SetStyle(thumb_style style)
{
	fStyle = style;
	Invalidate();
}

thumb_style BSlider::Style() const
{
	return fStyle;
}

void BSlider::SetBarColor(rgb_color barColor)
{
	fBarColor = barColor;
	Invalidate(BarFrame());
}

rgb_color BSlider::BarColor() const
{
	return fBarColor;
}

void BSlider::UseFillColor(bool useFill, const rgb_color *fillColor)
{
	fUseFillColor = useFill;

	if (useFill && fillColor)
		fFillColor = *fillColor;

	Invalidate(BarFrame());
}

bool BSlider::FillColor(rgb_color *fillColor) const
{
	if (fillColor && fUseFillColor)
		*fillColor = fFillColor;

	return fUseFillColor;
}

BView *BSlider::OffscreenView() const
{
#if USE_OFF_SCREEN_VIEW
	return fOffScreenView;
#else
	return (BView *)this;
#endif
}

orientation BSlider::Orientation() const
{
	return fOrientation;
}

void BSlider::SetOrientation(orientation posture)
{
	fOrientation = posture;
}

float BSlider::BarThickness() const
{
	return fBarThickness;
}

void BSlider::SetBarThickness(float thickness)
{
	if (thickness < 1.0)
		thickness = 1.0;
	else
		thickness = roundf(thickness);

	if (thickness != fBarThickness) {
		// calculate invalid barframe and extend by hashmark size
		float hInset = 0.0;
		float vInset = 0.0;
		switch (fOrientation) {
			case B_HORIZONTAL:
				vInset = -6.0;
				break;
			case B_VERTICAL:
				hInset = -6.0;
				break;
		}
		BRect invalid = BarFrame().InsetByCopy(hInset, vInset) | ThumbFrame();

		fBarThickness = thickness;

		invalid = invalid | BarFrame().InsetByCopy(hInset, vInset) | ThumbFrame();
		Invalidate(invalid);
	}
}

void BSlider::SetFont(const BFont *font, uint32 properties)
{
	BControl::SetFont(font, properties);
}

#pragma mark - private

void BSlider::_DrawBlockThumb()
{
	BRect  frame = ThumbFrame();
	BView *view	 = OffscreenView();

	auto hi = view->HighColor();

	frame.bottom -= 1;
	frame.right -= 1;
	view->SetHighColor(tint_color(view->ViewColor(), B_DARKEN_3_TINT));
	view->StrokePoint(frame.RightBottom());
	view->SetHighColor(tint_color(view->ViewColor(), B_DARKEN_2_TINT));
	view->StrokeRoundRect(frame.OffsetByCopy(1, 1), 2, 2);

	view->SetHighColor(ui_color(B_SHADOW_COLOR));
	view->StrokeRoundRect(frame, 2, 2);

	auto back_color = ui_color(B_CONTROL_BACKGROUND_COLOR);

	frame.InsetBy(1, 1);
	view->SetHighColor(tint_color(back_color, B_DARKEN_1_TINT));
	view->MovePenTo(frame.RightTop());
	view->StrokeLine(frame.RightBottom());
	view->StrokeLine(frame.LeftBottom());
	view->SetHighColor(ui_color(B_SHINE_COLOR));
	view->StrokeLine(frame.LeftTop());
	view->StrokeLine(frame.RightTop());

	frame.InsetBy(1, 1);
	view->SetHighColor(back_color);
	view->FillRect(frame);

	view->SetHighColor(hi);
}

void BSlider::_DrawTriangleThumb()
{
	BRect  frame = ThumbFrame();
	BView *view	 = OffscreenView();

	auto hi = view->HighColor();

	auto back_color = ui_color(B_CONTROL_BACKGROUND_COLOR);

	switch (fOrientation) {
		case B_HORIZONTAL: {
			auto size = std::min(frame.Height(), roundf((frame.right - frame.left) / 2));
			auto mid  = roundf((frame.left + frame.right) / 2);

			view->SetHighColor(back_color);
			view->FillTriangle(
				{mid - size + 1, frame.top + size},
				{mid, frame.top + 1},
				{mid + size - 1, frame.top + size});

			view->SetHighColor(tint_color(back_color, B_DARKEN_1_TINT));
			view->MovePenTo({mid + size - 1, frame.top + size});
			view->StrokeLine({mid - size + 1, frame.top + size});
			view->SetHighColor(ui_color(B_SHINE_COLOR));
			view->StrokeLine({mid, frame.top + 1});
			view->StrokeLine({mid + size - 2, frame.top + size - 1});

			view->SetHighColor(tint_color(view->ViewColor(), B_DARKEN_2_TINT));
			view->StrokeLine({mid - size + 1, frame.top + size + 2}, {mid + size, frame.top + size + 2}, B_SOLID_HIGH);

			view->SetHighColor(ui_color(B_SHADOW_COLOR));
			view->MovePenTo({mid + size, frame.top + size + 1});
			view->StrokeLine({mid - size, frame.top + size + 1});
			view->StrokeLine({mid - size, frame.top + size});
			view->StrokeLine({mid, frame.top});
			view->StrokeLine({mid + size, frame.top + size});

		} break;

		case B_VERTICAL:
			debugger(__PRETTY_FUNCTION__);
			break;
	}

	view->SetHighColor(hi);
}

float BSlider::_MinPosition() const
{
	switch (fOrientation) {
		case B_HORIZONTAL:
			return BarFrame().left + 1.0f;
		case B_VERTICAL:
			return BarFrame().bottom - 1.0f;
	}
}

float BSlider::_MaxPosition() const
{
	switch (fOrientation) {
		case B_HORIZONTAL:
			return BarFrame().right - 1.0f;
		case B_VERTICAL:
			return BarFrame().top + 1.0f;
	}
}

bool BSlider::_ConstrainPoint(BPoint &point, BPoint comparePoint) const
{
	switch (fOrientation) {
		case B_HORIZONTAL:
			if (point.x != comparePoint.x) {
				if (point.x < _MinPosition())
					point.x = _MinPosition();
				else if (point.x > _MaxPosition())
					point.x = _MaxPosition();

				return true;
			}
			break;
		case B_VERTICAL:
			if (point.y != comparePoint.y) {
				if (point.y > _MinPosition())
					point.y = _MinPosition();
				else if (point.y < _MaxPosition())
					point.y = _MaxPosition();

				return true;
			}
			break;
	}

	return false;
}
