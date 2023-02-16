#ifndef _SCREEN_H
#define _SCREEN_H

// #include <Accelerant.h>
#include <GraphicsDefs.h>
// #include <OS.h>
#include <Rect.h>

class BBitmap;
class BWindow;
class BPrivateScreen;

typedef struct
{
	// display_timing timing;			 //! CTRC info
	uint32 space;			 //! pixel configuration
	uint16 virtual_width;	 //! in pixels
	uint16 virtual_height;	 //! in lines
	uint16 h_display_start;	 //! first displayed pixel in line
	uint16 v_display_start;	 //! first displayed line
	uint32 flags;			 //! mode flags
} display_mode;

class BScreen
{
   public:
	BScreen(screen_id id = B_MAIN_SCREEN_ID);
	BScreen(BWindow *win);
	~BScreen();

	bool	 IsValid();
	status_t SetToNext();

	color_space ColorSpace();
	BRect		Frame();
	screen_id	ID();

	status_t WaitForRetrace();
	status_t WaitForRetrace(bigtime_t timeout);

	uint8	  IndexForColor(rgb_color rgb);
	uint8	  IndexForColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	rgb_color ColorForIndex(const uint8 index);
	uint8	  InvertIndex(uint8 index);

	const color_map *ColorMap();

	status_t GetBitmap(BBitmap **screen_shot,
					   bool		 draw_cursor = true,
					   BRect	*bound		 = NULL);
	status_t ReadBitmap(BBitmap *buffer,
						bool	 draw_cursor = true,
						BRect	*bound		 = NULL);

	rgb_color DesktopColor();
	rgb_color DesktopColor(uint32 index);
	void	  SetDesktopColor(rgb_color rgb, bool stick = true);
	void	  SetDesktopColor(rgb_color rgb, uint32 index, bool stick = true);

	status_t ProposeMode(display_mode *target, const display_mode *low, const display_mode *high);
	status_t GetModeList(display_mode **mode_list, uint32 *count);
	status_t GetMode(display_mode *mode);
	status_t GetMode(uint32 workspace, display_mode *mode);
	status_t SetMode(display_mode *mode, bool makeDefault = false);
	status_t SetMode(uint32 workspace, display_mode *mode, bool makeDefault = false);
	// status_t GetDeviceInfo(accelerant_device_info *adi);
	// status_t GetPixelClockLimits(display_mode *mode, uint32 *low, uint32 *high);
	// status_t GetTimingConstraints(display_timing_constraints *dtc);
	status_t SetDPMS(uint32 dpms_state);
	uint32	 DPMSState(void);
	uint32	 DPMSCapabilites(void);

	BPrivateScreen *private_screen();

   private:
	status_t ProposeDisplayMode(display_mode *target, const display_mode *low, const display_mode *high);
	BScreen &operator=(const BScreen &screen);
	BScreen(const BScreen &screen);
	void  *BaseAddress();
	uint32 BytesPerRow();

	BPrivateScreen *screen;
};

inline uint8 BScreen::IndexForColor(rgb_color rgb)
{
	return IndexForColor(rgb.red, rgb.green, rgb.blue, rgb.alpha);
}

#endif /* _SCREEN_H */
