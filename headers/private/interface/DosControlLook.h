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

	virtual uint32 Flags(BControl* control) const;

	virtual void DrawMenuBarBackground(BView* view, BRect& rect,
									   const BRect&		updateRect,
									   const rgb_color& base,
									   uint32			flags	= 0,
									   uint32			borders = B_ALL_BORDERS);

	virtual void DrawMenuBackground(BView* view,
									BRect& rect, const BRect& updateRect,
									const rgb_color& base, uint32 flags = 0,
									uint32 borders = B_ALL_BORDERS);

	virtual void DrawMenuItemBackground(BView* view,
										BRect& rect, const BRect& updateRect,
										const rgb_color& base, uint32 flags = 0,
										uint32 borders = B_ALL_BORDERS);

	virtual void DrawArrowShape(BView* view,
								BRect& rect, const BRect& updateRect,
								const rgb_color& base, uint32 direction,
								uint32 flags = 0,
								float  tint	 = B_DARKEN_MAX_TINT);

	virtual void DrawBorder(BView* view, BRect& rect,
							const BRect&	 updateRect,
							const rgb_color& base,
							border_style borderStyle, uint32 flags = 0,
							uint32 borders = B_ALL_BORDERS);

	virtual void DrawLabel(BView* view, const char* label,
						   BRect rect, const BRect& updateRect,
						   const rgb_color& base, uint32 flags,
						   const rgb_color* textColor = nullptr);
	virtual void DrawLabel(BView* view, const char* label,
						   BRect rect, const BRect& updateRect,
						   const rgb_color& base, uint32 flags,
						   const BAlignment& alignment,
						   const rgb_color*	 textColor = nullptr);
	virtual void DrawLabel(BView* view, const char* label,
						   const rgb_color& base, uint32 flags,
						   const BPoint&	where,
						   const rgb_color* textColor = nullptr);

	virtual void DrawLabel(BView* view, const char* label,
						   const BBitmap* icon, BRect rect,
						   const BRect&		updateRect,
						   const rgb_color& base, uint32 flags,
						   const BAlignment& alignment,
						   const rgb_color*	 textColor = nullptr);

   protected:
	void _DrawOuterResessedFrame(BView* view,
								 BRect& rect, const rgb_color& base,
								 float	contrast   = 1.0f,
								 float	brightness = 1.0f,
								 uint32 flags	   = 0,
								 uint32 borders	   = B_ALL_BORDERS);

	void _DrawFrame(BView* view, BRect& rect,
					const rgb_color& left,
					const rgb_color& top,
					const rgb_color& right,
					const rgb_color& bottom,
					uint32			 borders = B_ALL_BORDERS);
	void _DrawFrame(BView* view, BRect& rect,
					const rgb_color& left,
					const rgb_color& top,
					const rgb_color& right,
					const rgb_color& bottom,
					const rgb_color& rightTop,
					const rgb_color& leftBottom,
					uint32			 borders = B_ALL_BORDERS);

	void _FillGradient(BView* view, const BRect& rect,
					   const rgb_color& base, float topTint,
					   float	   bottomTint,
					   orientation orientation = B_HORIZONTAL);

	// BLayoutUtils
	BRect _AlignInFrame(BRect frame, BSize maxSize, BAlignment alignment);
	BRect _AlignOnRect(BRect rect, BSize size, BAlignment alignment);
};

}  // namespace BPrivate
