#include "Bitmap.h"

BBitmap::BBitmap(BRect bounds, uint32 flags, color_space depth, int32 bytesPerRow, screen_id screenID)
{
	debugger(__PRETTY_FUNCTION__);
}

BBitmap::BBitmap(BRect bounds, color_space depth, bool accepts_views, bool need_contiguous)
{
	debugger(__PRETTY_FUNCTION__);
}

BBitmap::BBitmap(const BBitmap *source, bool accepts_views, bool need_contiguous)
{
	debugger(__PRETTY_FUNCTION__);
}

BBitmap::~BBitmap()
{
	debugger(__PRETTY_FUNCTION__);
}

BBitmap::BBitmap(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
}

status_t BBitmap::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BBitmap::AddChild(BView *view)
{
	debugger(__PRETTY_FUNCTION__);
}

bool BBitmap::RemoveChild(BView *view)
{
	debugger(__PRETTY_FUNCTION__);
	return false;
}
