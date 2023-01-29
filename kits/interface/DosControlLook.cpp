#define CHECKBOX_BOX_SIZE 12.0f
#define CHECKBOX_TEXT_PADDING 5.0f

#include "DosControlLook.h"

using namespace BPrivate;

DosControlLook::DosControlLook() {}

DosControlLook::~DosControlLook() {}

BAlignment DosControlLook::DefaultLabelAlignment() const
{
	return BAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_CENTER);
}

float DosControlLook::DefaultLabelSpacing() const
{
	return CHECKBOX_TEXT_PADDING;
}

float DosControlLook::DefaultItemSpacing() const
{
	return CHECKBOX_TEXT_PADDING;
}

void DosControlLook::DrawMenuBarBackground(BView* view, BRect& rect,
										   const BRect&		updateRect,
										   const rgb_color& base,
										   uint32			flags,
										   uint32			borders)
{
}

void DosControlLook::DrawMenuBackground(BView* view,
										BRect& rect, const BRect& updateRect,
										const rgb_color& base, uint32 flags,
										uint32 borders)
{
}

void DosControlLook::DrawBorder(BView* view, BRect& rect,
								const BRect&	 updateRect,
								const rgb_color& base,
								border_style borderStyle, uint32 flags,
								uint32 borders)
{
}

void DosControlLook::DrawLabel(BView* view, const char* label,
							   const BBitmap* icon, BRect rect,
							   const BRect&		updateRect,
							   const rgb_color& base, uint32 flags,
							   const BAlignment& alignment,
							   const rgb_color*)
{
}
