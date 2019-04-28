#ifndef _BITMAP_TOOLS_H
#define _BITMAP_TOOLS_H

#include <GraphicsDefs.h>
#include <InterfaceDefs.h>

class BBitmap;

namespace BExperimental {

typedef rgb_color (*pixel_reader)(const uint8* pixel, const color_map* cmap);
typedef void (*pixel_writer)(uint8* pixel, const rgb_color color, const color_map* cmap);

class pixel_access
{
public:
	pixel_access();
	pixel_access(color_space space);
	~pixel_access();
	
	status_t set_to(color_space space);
	
	bool valid() const;
	
	inline size_t bpp() const
	{
		return fBPP;
	}
	
	inline rgb_color read(const uint8* const pixel) const
	{
		return (*fReader)(pixel, fColorMap);
	}
	
	inline void write(uint8* const pixel, const rgb_color color)
	{
		(*fWriter)(pixel, color, fColorMap);
	}
	

private:
	// actually public variables due to inlines.
	const color_map *fColorMap;
	size_t fBPP;
	pixel_reader fReader;
	pixel_writer fWriter;
};
	
status_t mix_bitmaps(BBitmap* dest,
					 const BBitmap* src1, const BBitmap* src2, uint8 amount);

status_t scale_bitmap(BBitmap* dest, const BBitmap* src);

status_t copy_bitmap(BBitmap* dest,
					 const BBitmap* src, BRect srcRect, BPoint destPnt);

status_t set_bitmap(BBitmap* dest, const BBitmap* src, bool dither=true);

}	// namespace BExperimental

using namespace BExperimental;

#endif
