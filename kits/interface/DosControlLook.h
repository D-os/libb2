#define CHECKBOX_BOX_SIZE 12.0f
#define CHECKBOX_TEXT_PADDING 5.0f

#include <ControlLook.h>

namespace BPrivate {

class DosControlLook : public BControlLook
{
   public:
	DosControlLook();
	virtual ~DosControlLook();

	virtual BAlignment DefaultLabelAlignment() const;
	virtual float	   DefaultLabelSpacing() const;

	virtual float DefaultItemSpacing() const;

	virtual void DrawMenuBarBackground(BView* view, BRect& rect,
									   const BRect&		updateRect,
									   const rgb_color& base,
									   uint32			flags	= 0,
									   uint32			borders = B_ALL_BORDERS);

	virtual void DrawMenuBackground(BView* view,
									BRect& rect, const BRect& updateRect,
									const rgb_color& base, uint32 flags = 0,
									uint32 borders = B_ALL_BORDERS);

	virtual void DrawBorder(BView* view, BRect& rect,
							const BRect&	 updateRect,
							const rgb_color& base,
							border_style borderStyle, uint32 flags = 0,
							uint32 borders = B_ALL_BORDERS);

	virtual void DrawLabel(BView* view, const char* label,
						   const BBitmap* icon, BRect rect,
						   const BRect&		updateRect,
						   const rgb_color& base, uint32 flags,
						   const BAlignment& alignment,
						   const rgb_color*	 textColor = nullptr);
};

}  // namespace BPrivate
