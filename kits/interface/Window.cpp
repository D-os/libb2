#include "Window.h"

#define LOG_TAG "BWindow"

#include <Application.h>
#include <Autolock.h>
#include <Button.h>
#include <Debug.h>
#include <Message.h>
#include <MessageQueue.h>
#include <Point.h>
#include <Rect.h>
#include <View.h>
#include <include/core/SkSurface.h>
#include <log/log.h>
#include <pimpl.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>

#include <cstddef>
#include <cstring>
#include <format>
#include <system_error>

#include "xdg-shell-client-protocol.h"

#define DEFAULT_CURSOR_SIZE 24
#define DEFAULT_CURSOR_NAME "left_ptr"

class BWindow::impl
{
   public:
	/* Globals */
	struct wl_display	  *wl_display;
	struct wl_registry   *wl_registry;
	struct wl_compositor *wl_compositor;
	struct wl_shm		  *wl_shm;
	struct xdg_wm_base	   *xdg_wm_base;
	struct wl_seat		   *wl_seat;
	/* Objects */
	struct wl_surface	  *wl_surface;
	struct xdg_surface	   *xdg_surface;
	struct xdg_toplevel	*xdg_toplevel;
	struct wl_pointer	  *wl_pointer;
	struct wl_surface	  *cursor_surface;
	struct wl_cursor_image *cursor_image;
	struct wl_keyboard	   *wl_keyboard;

	/* Backing */
	size_t				  width;
	size_t				  height;
	struct wl_shm_pool   *wl_shm_pool;
	size_t				  shm_pool_size;
	int					  pool_fd;
	uint8_t			  *pool_data;
	struct wl_buffer	 *wl_buffer;

	/* XKB */
	struct xkb_state	 *xkb_state;
	struct xkb_context *xkb_context;
	struct xkb_keymap  *xkb_keymap;

	/* Drawing */
	SkImageInfo		 info;
	sk_sp<SkSurface> surface;

	/* State */
	BView		top_view;
	const char *title;

	float pointer_x;
	float pointer_y;
	uint32 pointer_buttons;

	uint32 modifiers;

	bool hidden;
	bool minimized;
	bool maximized;
	bool closed;

	bool surface_pending;	 /// surface has pending changes needing commit
	bool surface_committed;	 /// surface is comitted, pending a reattach

	impl()
		: wl_display{nullptr},
		  wl_registry{nullptr},
		  wl_compositor{nullptr},
		  wl_shm{nullptr},
		  xdg_wm_base{nullptr},
		  wl_seat{nullptr},
		  wl_surface{nullptr},
		  xdg_surface{nullptr},
		  xdg_toplevel{nullptr},
		  wl_pointer{nullptr},
		  cursor_surface{nullptr},
		  cursor_image{nullptr},
		  wl_keyboard{nullptr},

		  width{0},
		  height{0},
		  wl_shm_pool{nullptr},
		  shm_pool_size{0},
		  pool_fd{-1},
		  pool_data{nullptr},
		  wl_buffer{nullptr},

		  xkb_state{nullptr},
		  xkb_context{nullptr},
		  xkb_keymap{nullptr},

		  top_view(BRect(B_ORIGIN, B_ORIGIN), "TopView", B_FOLLOW_ALL, B_WILL_DRAW),
		  title{nullptr},
		  pointer_x{-1.0},
		  pointer_y{-1.0},
		  pointer_buttons{0},
		  modifiers{0},
		  hidden{true},
		  minimized{false},
		  maximized{false},
		  closed{false},

		  surface_pending{false},
		  surface_committed{false},

		  wl_registry_listener{
			  .global		 = wl_registry_global_handler,
			  .global_remove = wl_registry_global_remove_handler,
		  },
		  wl_shm_listener{
			  .format = wl_shm_format_handler,
		  },
		  wl_surface_listener{
			  .enter = wl_surface_enter_handler,
			  .leave = wl_surface_leave_handler,
		  },
		  wl_buffer_listener{
			  .release = wl_buffer_release_handler,
		  },
		  xdg_surface_listener{
			  .configure = xdg_surface_configure_handler,
		  },
		  xdg_wm_base_listener{
			  .ping = xdg_wm_base_ping_handler,
		  },
		  xdg_toplevel_listener{
			  .configure = xdg_toplevel_configure_handler,
			  .close	 = xdg_toplevel_close_handler,
		  },
		  wl_seat_listener{
			  .capabilities = wl_seat_capabilities_handler,
			  .name			= wl_seat_name_handler,
		  },
		  wl_pointer_listener{
			  .enter  = wl_pointer_enter_handler,
			  .leave  = wl_pointer_leave_handler,
			  .motion = wl_pointer_motion_handler,
			  .button = wl_pointer_button_handler,
			  .axis	  = wl_pointer_axis_handler,
		  },
		  wl_keyboard_listener{
			  .keymap	   = wl_keyboard_keymap,
			  .enter	   = wl_keyboard_enter,
			  .leave	   = wl_keyboard_leave,
			  .key		   = wl_keyboard_key,
			  .modifiers   = wl_keyboard_modifiers,
			  .repeat_info = wl_keyboard_repeat_info,
		  }
	{
	}

	~impl()
	{
		// FIXME: _destroy all in proper order
		if (wl_surface)
			wl_surface_destroy(wl_surface);
		// FIXME: _disconnect() gracefully
	}

	void set_size(size_t width, size_t height)
	{
		// frame coordinates are in the middle of pixels,
		// so 0<->1 covers 2 pixels, thus we need to add 1
		this->width	 = width + 1;
		this->height = height + 1;

		top_view.ResizeTo(width, height);
	}

	bool connect();
	void resize(size_t width, size_t height);
	void showWindow();
	void hideWindow();
	void minimize(bool minimized);

	void pointer_motion(float x, float y);
	void pointer_button(uint32_t button, uint32_t state);

	void keyboard_key(uint32_t key, uint32_t state);
	void update_modifiers(xkb_keysym_t sym, uint32_t state);
	void keyboard_modifiers(uint32_t mods_depressed,
							uint32_t mods_latched, uint32_t mods_locked,
							uint32_t group);

	void damage(int32 x, int32 y, int32 width, int32 height);

   private:
	std::mutex pool_mutex;
	void	   resize_buffer();

	const struct wl_registry_listener  wl_registry_listener;
	const struct wl_shm_listener	   wl_shm_listener;
	const struct wl_surface_listener   wl_surface_listener;
	const struct wl_buffer_listener	   wl_buffer_listener;
	const struct xdg_surface_listener  xdg_surface_listener;
	const struct xdg_wm_base_listener  xdg_wm_base_listener;
	const struct xdg_toplevel_listener xdg_toplevel_listener;
	const struct wl_seat_listener	   wl_seat_listener;
	const struct wl_pointer_listener   wl_pointer_listener;
	const struct wl_keyboard_listener  wl_keyboard_listener;

	static void wl_registry_global_handler(void *this_, struct wl_registry *registry, uint32_t name,
										   const char *interface, uint32_t version);
	static void wl_registry_global_remove_handler(void *this_, struct wl_registry *registry, uint32_t name);
	static void wl_shm_format_handler(void *this_, struct wl_shm *wl_shm, uint32_t format);
	static void wl_surface_enter_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
	static void wl_surface_leave_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
	static void wl_buffer_release_handler(void *this_, struct wl_buffer *wl_buffer);
	static void xdg_surface_configure_handler(void *this_, struct xdg_surface *xdg_surface, uint32_t serial);
	static void xdg_wm_base_ping_handler(void *this_, struct xdg_wm_base *xdg_wm_base, uint32_t serial);
	static void xdg_toplevel_configure_handler(void *this_, struct xdg_toplevel *xdg_toplevel,
											   int32_t width, int32_t height, struct wl_array *states);
	static void xdg_toplevel_close_handler(void *this_, struct xdg_toplevel *toplevel);
	static void wl_seat_capabilities_handler(void *this_, struct wl_seat *wl_seat, uint32_t capabilities);
	static void wl_seat_name_handler(void *this_, struct wl_seat *wl_seat, const char *name);
	static void wl_pointer_enter_handler(void *this_, struct wl_pointer *wl_pointer, uint32_t serial,
										 struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
	static void wl_pointer_leave_handler(void *this_, struct wl_pointer *wl_pointer, uint32_t serial,
										 struct wl_surface *surface);
	static void wl_pointer_motion_handler(void *this_, struct wl_pointer *wl_pointer,
										  uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
	static void wl_pointer_button_handler(void *this_, struct wl_pointer *wl_pointer, uint32_t serial,
										  uint32_t time, uint32_t button, uint32_t state);
	static void wl_pointer_axis_handler(void *this_, struct wl_pointer *wl_pointer,
										uint32_t time, uint32_t axis, wl_fixed_t value);
	static void wl_keyboard_keymap(void *this_, struct wl_keyboard *wl_keyboard,
								   uint32_t format, int32_t fd, uint32_t size);
	static void wl_keyboard_enter(void *this_, struct wl_keyboard *wl_keyboard,
								  uint32_t serial, struct wl_surface *surface,
								  struct wl_array *keys);
	static void wl_keyboard_leave(void *this_, struct wl_keyboard *wl_keyboard,
								  uint32_t serial, struct wl_surface *surface);
	static void wl_keyboard_key(void *this_, struct wl_keyboard *wl_keyboard,
								uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
	static void wl_keyboard_modifiers(void *this_, struct wl_keyboard *wl_keyboard,
									  uint32_t serial, uint32_t mods_depressed,
									  uint32_t mods_latched, uint32_t mods_locked,
									  uint32_t group);
	static void wl_keyboard_repeat_info(void *this_, struct wl_keyboard *wl_keyboard,
										int32_t rate, int32_t delay);
};

namespace {
inline window_look type2look(window_type type)
{
	switch (type) {
		case B_DOCUMENT_WINDOW:
			return B_DOCUMENT_WINDOW_LOOK;
		case B_MODAL_WINDOW:
			return B_MODAL_WINDOW_LOOK;
		case B_FLOATING_WINDOW:
			return B_FLOATING_WINDOW_LOOK;
		case B_BORDERED_WINDOW:
			return B_BORDERED_WINDOW_LOOK;
		default:
			return B_TITLED_WINDOW_LOOK;
	}
}

inline window_feel type2feel(window_type type)
{
	switch (type) {
		case B_MODAL_WINDOW:
			return B_MODAL_APP_WINDOW_FEEL;
		case B_FLOATING_WINDOW:
			return B_FLOATING_APP_WINDOW_FEEL;
		default:
			return B_NORMAL_WINDOW_FEEL;
	}
}
}  // namespace

#pragma mark - BWindow::impl

bool BWindow::impl::connect()
{
	wl_display = wl_display_connect(NULL);
	if (!wl_display) return false;
	ALOGD("connected display");

	wl_registry = wl_display_get_registry(wl_display);
	if (!wl_registry) return false;
	ALOGV("connected registry");
	wl_registry_add_listener(wl_registry, &wl_registry_listener, this);

	xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (!xkb_context) return false;
	ALOGV("created xkb_context");

	// wait for the "initial" set of globals to appear
	wl_display_roundtrip(wl_display);
	ALOGV("wl_compositor %p, wl_shm %p, xdg_wm_base %p, wl_seat %p", wl_compositor, wl_shm, xdg_wm_base, wl_seat);
	if (!wl_compositor || !wl_shm || !xdg_wm_base || !wl_seat) return false;

	wl_surface = wl_compositor_create_surface(wl_compositor);
	if (!wl_surface) return false;
	ALOGV("created surface");
	wl_surface_add_listener(wl_surface, &wl_surface_listener, this);

	xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, wl_surface);
	xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);
	if (!xdg_surface) return false;
	ALOGV("created xdg_surface");

	xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	if (!xdg_toplevel) return false;
	ALOGV("created xdg_toplevel");
	xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);
	xdg_toplevel_set_app_id(xdg_toplevel, be_app->Name());
	xdg_toplevel_set_title(xdg_toplevel, title ? title : "BWindow");
	wl_surface_commit(wl_surface);

	return true;
}

void BWindow::impl::resize_buffer()
{
	info			= SkImageInfo::MakeN32Premul(width, height);
	size_t stride	= info.minRowBytes();
	size_t new_size = info.computeByteSize(stride);

	if (new_size == shm_pool_size) return;

	std::lock_guard<std::mutex> guard(pool_mutex);

	ALOGV("Resizing BWindow buffer: %zu(%zu@4)x%zu %zu bytes", width, stride, height, new_size);

	if (pool_fd < 0) {
		pool_fd = syscall(SYS_memfd_create, "wl_shm_buffer", 0);
	}
	ALOG_ASSERT(pool_fd >= 0, "Cannot create shared memory buffer file");

	if (new_size > shm_pool_size) {
		ftruncate(pool_fd, new_size);

		if (pool_data) {
			if (munmap(pool_data, shm_pool_size) == -1) {
				LOG_ALWAYS_FATAL("Failed to unmap memory: %zu bytes @ %p; %d: %s",
								 shm_pool_size, pool_data, errno, strerror(errno));
			}
		}
		pool_data	  = (uint8_t *)mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, pool_fd, 0);
		shm_pool_size = new_size;

		if (wl_shm_pool) {
			wl_shm_pool_resize(wl_shm_pool, new_size);
		}
	}

	if (!wl_shm_pool) {
		wl_shm_pool = wl_shm_create_pool(wl_shm, pool_fd, shm_pool_size);
	}

	int			 index	= 0;
	const size_t offset = height * stride * index;

	wl_buffer = wl_shm_pool_create_buffer(wl_shm_pool,
										  offset, width, height, stride, WL_SHM_FORMAT_XRGB8888);

	wl_buffer_add_listener(wl_buffer, &wl_buffer_listener, this);

	wl_surface_attach(wl_surface, wl_buffer, 0, 0);
	surface_committed = false;
	wl_surface_damage_buffer(wl_surface, 0, 0, INT32_MAX, INT32_MAX);
	surface_pending = true;

#ifndef NDEBUG
	/* Draw checkerboxed background to see drawing artifacts */
	uint32_t *pixels = (uint32_t *)&pool_data[offset];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if ((x + y / 8 * 8) % 16 < 8)
				pixels[y * width + x] = 0xFFFF5555;
			else
				pixels[y * width + x] = 0xFFAAAAFF;
		}
	}
#endif

	surface = SkSurface::MakeRasterDirect(info, pool_data, stride);

	if (!hidden)
		top_view.Invalidate();
}

void BWindow::impl::showWindow()
{
	hidden = false;
}

void BWindow::impl::hideWindow()
{
	hidden = true;
}

void BWindow::impl::minimize(bool minimized)
{
	debugger(__PRETTY_FUNCTION__);
	this->minimized = minimized;
}

void BWindow::impl::resize(size_t width, size_t height)
{
	set_size(width, height);
	resize_buffer();
	surface_pending = true;
}

void BWindow::impl::pointer_motion(float x, float y)
{
	this->pointer_x = x;
	this->pointer_y = y;

	BMessage mouseMoveMessage(B_MOUSE_MOVED);
	mouseMoveMessage.AddPoint("screen_where", BPoint(x, y));
	mouseMoveMessage.AddUInt32("buttons", this->pointer_buttons);
	if (top_view.Window()) {
		top_view.Window()->PostMessage(&mouseMoveMessage);
	}
}

void BWindow::impl::pointer_button(uint32_t button, uint32_t state)
{
	uint32 button_mask = (1 << button);
	if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
		this->pointer_buttons &= ~button_mask;
	}
	else {
		this->pointer_buttons |= button_mask;
	}

	BMessage mouseMoveMessage(state == WL_POINTER_BUTTON_STATE_RELEASED ? B_MOUSE_UP : B_MOUSE_DOWN);
	mouseMoveMessage.AddPoint("screen_where", BPoint(this->pointer_x, this->pointer_y));
	mouseMoveMessage.AddUInt32("buttons", this->pointer_buttons);
	if (top_view.Window()) {
		top_view.Window()->PostMessage(&mouseMoveMessage);
	}
}

void BWindow::impl::keyboard_key(uint32_t key, uint32_t state)
{
	/// NOTE: the scancode from this event is the Linux evdev scancode.
	/// To translate this to an XKB scancode, you must add 8 to the evdev scancode.
	uint32_t	 keycode = key + 8;
	xkb_keysym_t sym	 = xkb_state_key_get_one_sym(xkb_state, keycode);

	int utf8_size = xkb_state_key_get_utf8(xkb_state, keycode, nullptr, 0);
	utf8_size += 1;	 // NUL character
	char utf8[utf8_size];
	xkb_state_key_get_utf8(xkb_state, keycode, utf8, utf8_size);

	ALOGV("keyboard_key %d(0x%x): %d(0x%x) -> %d: '%s'", key, key, keycode, keycode, sym, utf8);

	BMessage keyboardKeyMessage(state == WL_KEYBOARD_KEY_STATE_PRESSED ? B_KEY_DOWN : B_KEY_UP);
	keyboardKeyMessage.AddInt32("key", key);
	// keyboardKeyMessage.AddUInt8("states", 0);

	utf8_size = strlen(utf8);
	if (utf8_size > 0) {
		if (utf8_size == 1) {
			switch (utf8[0]) {
				case 0x0d:
					utf8[0] = B_ENTER;
					break;
			}
			keyboardKeyMessage.AddInt32("raw_char", utf8[0]);
		}
		keyboardKeyMessage.AddString("bytes", utf8);
	}
	else if (sym == XKB_KEY_BackSpace || sym == XKB_KEY_Return || sym == XKB_KEY_Tab || sym == XKB_KEY_Escape
			 || sym == XKB_KEY_Left || sym == XKB_KEY_Right || sym == XKB_KEY_Up || sym == XKB_KEY_Down
			 || sym == XKB_KEY_Insert || sym == XKB_KEY_Delete || sym == XKB_KEY_Home || sym == XKB_KEY_End
			 || sym == XKB_KEY_Page_Up || sym == XKB_KEY_Page_Down) {
		char raw_char = '\0';
		switch (sym) {
			case XKB_KEY_BackSpace:
				raw_char = B_BACKSPACE;
				break;
			case XKB_KEY_Return:
				raw_char = B_ENTER;
				break;
			case XKB_KEY_Tab:
				raw_char = B_TAB;
				break;
			case XKB_KEY_Escape:
				raw_char = B_ESCAPE;
				break;
			case XKB_KEY_Left:
				raw_char = B_LEFT_ARROW;
				break;
			case XKB_KEY_Right:
				raw_char = B_RIGHT_ARROW;
				break;
			case XKB_KEY_Up:
				raw_char = B_UP_ARROW;
				break;
			case XKB_KEY_Down:
				raw_char = B_DOWN_ARROW;
				break;
			case XKB_KEY_Insert:
				raw_char = B_INSERT;
				break;
			case XKB_KEY_Delete:
				raw_char = B_DELETE;
				break;
			case XKB_KEY_Home:
				raw_char = B_HOME;
				break;
			case XKB_KEY_End:
				raw_char = B_END;
				break;
			case XKB_KEY_Page_Up:
				raw_char = B_PAGE_UP;
				break;
			case XKB_KEY_Page_Down:
				raw_char = B_PAGE_DOWN;
				break;
		}
		if (raw_char) {
			keyboardKeyMessage.AddInt32("raw_char", raw_char);
		}
	}
	else {
		keyboardKeyMessage.what = state == WL_KEYBOARD_KEY_STATE_PRESSED ? B_UNMAPPED_KEY_DOWN : B_UNMAPPED_KEY_UP;
	}

	update_modifiers(sym, state);
	keyboardKeyMessage.AddInt32("modifiers", modifiers);

	if (top_view.Window()) {
		top_view.Window()->PostMessage(&keyboardKeyMessage);
	}
}

void BWindow::impl::update_modifiers(xkb_keysym_t sym, uint32_t state)
{
#define SET_MODIFIERS_BIT(bit)                  \
	if (state == WL_KEYBOARD_KEY_STATE_PRESSED) \
		modifiers |= bit;                       \
	else                                        \
		modifiers &= ~bit;

	// Symbol specific handling
	switch (sym) {
		case XKB_KEY_Shift_L:
			SET_MODIFIERS_BIT(B_LEFT_SHIFT_KEY);
			break;
		case XKB_KEY_Shift_R:
			SET_MODIFIERS_BIT(B_RIGHT_SHIFT_KEY);
			break;
		case XKB_KEY_Alt_L:
			SET_MODIFIERS_BIT(B_LEFT_COMMAND_KEY);
			break;
		case XKB_KEY_Alt_R:
			SET_MODIFIERS_BIT(B_RIGHT_COMMAND_KEY);
			break;
		case XKB_KEY_Control_L:
			SET_MODIFIERS_BIT(B_LEFT_CONTROL_KEY);
			break;
		case XKB_KEY_Control_R:
			// SET_MODIFIERS_BIT(B_RIGHT_CONTROL_KEY);
			SET_MODIFIERS_BIT(B_OPTION_KEY | B_RIGHT_OPTION_KEY);  // see NOTE below
			break;
		case XKB_KEY_Meta_L:
			SET_MODIFIERS_BIT(B_LEFT_OPTION_KEY);
			break;
		case XKB_KEY_Meta_R:
			SET_MODIFIERS_BIT(B_RIGHT_OPTION_KEY);
			break;
	}
#undef SET_MODIFIERS_BIT
}

void BWindow::impl::keyboard_modifiers(uint32_t mods_depressed,
									   uint32_t mods_latched, uint32_t mods_locked,
									   uint32_t group)
{
	enum xkb_state_component changed = xkb_state_update_mask(xkb_state,
															 mods_depressed, mods_latched, mods_locked,
															 0, 0, group);

	/// NOTE: By default, the keys closest to the space bar function as Command keys,
	/// no matter what their labels on particular keyboards.
	/// If a keyboard doesn't have Option keys (for example, a standard 101-key keyboard),
	/// the key on the right labeled "Control" functions as the right Option key,
	/// and only the left "Control" key is available to function as a Control modifier.
	if (changed & XKB_STATE_MODS_EFFECTIVE) {
		int shift_mod = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_SHIFT, nullptr);
		int caps_mod  = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_CAPS, nullptr);
		int ctrl_mod  = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_CTRL, nullptr);
		int alt_mod	  = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_ALT, nullptr);
		int num_mod	  = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_NUM, nullptr);
		int logo_mod  = xkb_state_mod_names_are_active(xkb_state, XKB_STATE_MODS_EFFECTIVE, XKB_STATE_MATCH_NON_EXCLUSIVE,
													   XKB_MOD_NAME_LOGO, nullptr);
#define SET_MODIFIERS_BIT(mod, bit, secondary) \
	if (mod >= 0) {                            \
		if (mod)                               \
			modifiers |= bit;                  \
		else                                   \
			modifiers &= ~(bit | secondary);   \
	}
		SET_MODIFIERS_BIT(shift_mod, B_SHIFT_KEY, B_LEFT_SHIFT_KEY | B_RIGHT_SHIFT_KEY);
		SET_MODIFIERS_BIT(alt_mod, B_COMMAND_KEY, B_LEFT_COMMAND_KEY | B_RIGHT_COMMAND_KEY);
		SET_MODIFIERS_BIT(ctrl_mod, B_CONTROL_KEY, B_LEFT_CONTROL_KEY | B_RIGHT_CONTROL_KEY);
		SET_MODIFIERS_BIT(caps_mod, B_CAPS_LOCK, 0);
		// SET_MODIFIERS_BIT(, B_SCROLL_LOCK, 0);
		SET_MODIFIERS_BIT(num_mod, B_NUM_LOCK, 0);
		// SET_MODIFIERS_BIT(, B_OPTION_KEY, B_LEFT_OPTION_KEY | B_RIGHT_OPTION_KEY);
		SET_MODIFIERS_BIT(logo_mod, B_MENU_KEY, 0);
#undef SET_MODIFIERS_BIT
	}
}

void BWindow::impl::damage(int32 x, int32 y, int32 width, int32 height)
{
	wl_surface_damage_buffer(wl_surface, x, y, width, height);
	surface_pending = true;
}

#define THIS static_cast<BWindow::impl *>(this_)

void BWindow::impl::wl_registry_global_handler(
	void			   *this_,
	struct wl_registry *registry,
	uint32_t			name,
	const char		   *interface,
	uint32_t			version)
{
	ALOGV("%u: %s (v.%u)", name, interface, version);

	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		THIS->wl_compositor = (struct wl_compositor *)wl_registry_bind(registry, name, &wl_compositor_interface, 4);
	}
	else if (strcmp(interface, wl_shm_interface.name) == 0) {
		THIS->wl_shm = (struct wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 1);
		wl_shm_add_listener(THIS->wl_shm, &THIS->wl_shm_listener, this_);
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		THIS->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(THIS->xdg_wm_base, &THIS->xdg_wm_base_listener, this_);
	}
	else if (strcmp(interface, wl_seat_interface.name) == 0) {
		THIS->wl_seat = (struct wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 4);
		wl_seat_add_listener(THIS->wl_seat, &THIS->wl_seat_listener, this_);
	}
}
void BWindow::impl::wl_registry_global_remove_handler(
	void			   *this_,
	struct wl_registry *registry,
	uint32_t			name) {}

void BWindow::impl::wl_shm_format_handler(void *this_, struct wl_shm *wl_shm, uint32_t format)
{
	ALOGV("SHM format: 0x%x: %.4s", format,
		  format == 0 ? "ARG8"
					  : (format == 1 ? "XRG8"
									 : (char *)&format));
}

void BWindow::impl::wl_surface_enter_handler(void *this_, struct wl_surface *surface, struct wl_output *output)
{
	ALOGV("%p surface enter %p", surface, output);
}
void BWindow::impl::wl_surface_leave_handler(void *this_, struct wl_surface *surface, struct wl_output *output)
{
	ALOGV("%p surface leave %p", surface, output);
}

void BWindow::impl::wl_buffer_release_handler(void *this_, struct wl_buffer *wl_buffer)
{
	// Sent by the compositor when it's no longer using this buffer
	if (wl_buffer == THIS->wl_buffer) {
		// if this is our surface buffer, we immediately reattach it as pending buffer
		wl_surface_attach(THIS->wl_surface, THIS->wl_buffer, 0, 0);
		THIS->surface_committed = false;
	}
	else {
		wl_buffer_destroy(wl_buffer);
	}
}

void BWindow::impl::xdg_surface_configure_handler(void *this_, struct xdg_surface *xdg_surface, uint32_t serial)
{
	xdg_surface_ack_configure(xdg_surface, serial);
	THIS->resize_buffer();
}

void BWindow::impl::xdg_wm_base_ping_handler(void *this_, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
	ALOGV("xdg_wm_base responding to PING");
	xdg_wm_base_pong(xdg_wm_base, serial);
}

void BWindow::impl::xdg_toplevel_configure_handler(void *this_, struct xdg_toplevel *xdg_toplevel,
												   int32_t width, int32_t height, struct wl_array *states)
{
	if (width == 0 || height == 0) {
		/* Compositor is deferring to us */
		return;
	}

	THIS->width	 = width;
	THIS->height = height;
}

void BWindow::impl::xdg_toplevel_close_handler(void *this_, struct xdg_toplevel *toplevel)
{
	THIS->closed = true;
}

void BWindow::impl::wl_seat_name_handler(void *this_, struct wl_seat *wl_seat, const char *name)
{
	ALOGD("seat name: %s", name);
}

void BWindow::impl::wl_seat_capabilities_handler(void *this_, struct wl_seat *wl_seat, uint32_t capabilities)
{
	bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

	if (have_pointer && !THIS->wl_pointer) do {
			THIS->wl_pointer = wl_seat_get_pointer(wl_seat);
			if (!THIS->wl_pointer) break;
			wl_pointer_add_listener(THIS->wl_pointer, &THIS->wl_pointer_listener, this_);
			ALOGV("got wl_pointer");

			if (!THIS->cursor_image) {
				struct wl_cursor_theme *cursor_theme = wl_cursor_theme_load(nullptr, DEFAULT_CURSOR_SIZE, THIS->wl_shm);
				if (!cursor_theme) break;
				ALOGV("loaded cursor_theme");

				struct wl_cursor *cursor = wl_cursor_theme_get_cursor(cursor_theme, DEFAULT_CURSOR_NAME);
				if (!cursor) break;
				ALOGV("got wl_cursor");

				if (cursor->image_count == 0) break;
				THIS->cursor_image = cursor->images[0];
				ALOGV("have cursor_image");
			}

			struct wl_buffer *cursor_buffer = wl_cursor_image_get_buffer(THIS->cursor_image);
			ALOGV("got cursor_buffer");

			THIS->cursor_surface = wl_compositor_create_surface(THIS->wl_compositor);
			wl_surface_attach(THIS->cursor_surface, cursor_buffer, 0, 0);
			wl_surface_commit(THIS->cursor_surface);
			ALOGV("created cursor_surface");
		} while (false);
	else if (!have_pointer && THIS->wl_pointer) {
		wl_pointer_release(THIS->wl_pointer);
		THIS->wl_pointer = nullptr;
	}

	bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

	if (have_keyboard && !THIS->wl_keyboard) {
		THIS->wl_keyboard = wl_seat_get_keyboard(THIS->wl_seat);
		ALOGV("got wl_keyboard");
		wl_keyboard_add_listener(THIS->wl_keyboard, &THIS->wl_keyboard_listener, this_);
	}
	else if (!have_keyboard && THIS->wl_keyboard) {
		wl_keyboard_release(THIS->wl_keyboard);
		THIS->wl_keyboard = nullptr;
	}
}

void BWindow::impl::wl_pointer_enter_handler(void *this_, struct wl_pointer *pointer, uint32_t serial,
											 struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	ALOGV("wl_pointer enter %p @%p", pointer, surface);
	wl_pointer_set_cursor(pointer, serial,
						  THIS->cursor_surface,
						  THIS->cursor_image->hotspot_x, THIS->cursor_image->hotspot_y);

	THIS->pointer_motion(wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
}

void BWindow::impl::wl_pointer_leave_handler(void *this_, struct wl_pointer *pointer, uint32_t serial,
											 struct wl_surface *surface)
{
	ALOGV("wl_pointer leave %p @%p", pointer, surface);
}

void BWindow::impl::wl_pointer_motion_handler(void *this_, struct wl_pointer *wl_pointer,
											  uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	THIS->pointer_motion(wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
}

void BWindow::impl::wl_pointer_button_handler(void *this_, struct wl_pointer *wl_pointer, uint32_t serial,
											  uint32_t time, uint32_t button, uint32_t state)
{
	THIS->pointer_button(button, state);
}

void BWindow::impl::wl_pointer_axis_handler(void *this_, struct wl_pointer *wl_pointer,
											uint32_t time, uint32_t axis, wl_fixed_t value)
{
	ALOGD("pointer axis event; axis: %x, value: %f", axis, wl_fixed_to_double(value));
}

void BWindow::impl::wl_keyboard_keymap(void *this_, struct wl_keyboard *wl_keyboard,
									   uint32_t format, int32_t fd, uint32_t size)
{
	ASSERT(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

	void *map_shm = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	ASSERT(map_shm != MAP_FAILED);

	struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(
		THIS->xkb_context, static_cast<char *>(map_shm),
		XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap(map_shm, size);
	close(fd);
	ALOGV("created xkb_keymap");

	struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);
	xkb_keymap_unref(THIS->xkb_keymap);
	xkb_state_unref(THIS->xkb_state);
	THIS->xkb_keymap = xkb_keymap;
	THIS->xkb_state	 = xkb_state;
}

void BWindow::impl::wl_keyboard_key(void *this_, struct wl_keyboard *wl_keyboard,
									uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
	THIS->keyboard_key(key, state);
}

void BWindow::impl::wl_keyboard_enter(void *this_, struct wl_keyboard *wl_keyboard,
									  uint32_t serial, struct wl_surface *surface,
									  struct wl_array *keys)
{
	for (uint32_t *key = static_cast<uint32_t *>((keys)->data);	 // wl_array_for_each() converted to C++ compatible
		 (const char *)key < ((const char *)static_cast<uint32_t *>((keys)->data) + (keys)->size);
		 (key)++) {
		xkb_keysym_t sym = xkb_state_key_get_one_sym(THIS->xkb_state, *key + 8);
		THIS->update_modifiers(sym, WL_KEYBOARD_KEY_STATE_PRESSED);
	}
}

void BWindow::impl::wl_keyboard_leave(void *this_, struct wl_keyboard *wl_keyboard,
									  uint32_t serial, struct wl_surface *surface)
{
	THIS->modifiers = 0;
}

void BWindow::impl::wl_keyboard_modifiers(void *this_, struct wl_keyboard *wl_keyboard,
										  uint32_t serial, uint32_t mods_depressed,
										  uint32_t mods_latched, uint32_t mods_locked,
										  uint32_t group)
{
	THIS->keyboard_modifiers(mods_depressed, mods_latched, mods_locked, group);
}

void BWindow::impl::wl_keyboard_repeat_info(void *this_, struct wl_keyboard *wl_keyboard,
											int32_t rate, int32_t delay)
{
	ALOGD("keyboard repeat; rate: %d, delay: %d", rate, delay);
}

#undef THIS

SkCanvas *BWindow::_get_canvas() const
{
	return m->surface ? m->surface->getCanvas() : nullptr;
}

void BWindow::_damage_window(int32 x, int32 y, int32 width, int32 height)
{
	m->damage(x, y, width, height);
}

void BWindow::_try_pulse()
{
	// generate pulse message if needed
	if (fViewsEvents & B_PULSE_NEEDED && fPulseRate > 0) {
		bigtime_t currentTime = system_time();
		if (currentTime >= fPulsedTime + fPulseRate) {
			fPulsedTime = currentTime;
			PostMessage(B_PULSE);
		}
	}
}

#pragma mark - BWindow

BWindow::BWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
	: BWindow(frame, title, type2look(type), type2feel(type), flags, workspace)
{
}

BWindow::BWindow(BRect frame, const char *title, window_look look, window_feel feel, uint32 flags, uint32 workspace)
	: BLooper("w>", B_DISPLAY_PRIORITY),
	  fShowLevel{1},
	  fFlags{flags},
	  fFocus{nullptr},
	  fLastMouseMovedView{nullptr},
	  fDefaultButton{nullptr},
	  fPulseRate{500000},
	  fLook{look},
	  fFeel{feel},
	  fViewsEvents{0},
	  fPulsedTime{0}
{
	frame.left	 = roundf(frame.left);
	frame.top	 = roundf(frame.top);
	frame.right	 = roundf(frame.right);
	frame.bottom = roundf(frame.bottom);

	const size_t width	= frame.right - frame.left;
	const size_t height = frame.bottom - frame.top;
	m->set_size(width, height);

	SetTitle(title);

	if (!m->connect()) {
		throw std::system_error(std::error_code(errno, std::system_category()), "Cannot connect display");
		abort();
	}

	m->top_view._attach(this);
}

BWindow::~BWindow() = default;

status_t BWindow::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BWindow::Quit()
{
	if (!IsLocked()) {
		ALOGE("You must Lock a looper before calling Quit(), team=%d, looper=%s",
			  Team(), Name());
	}

	// Try to lock
	if (!Lock()) {
		// We're toast already
		return;
	}

	while (!IsHidden()) {
		Hide();
	}

	if (fFlags & B_QUIT_ON_WINDOW_CLOSE)
		be_app->PostMessage(B_QUIT_REQUESTED);

	BLooper::Quit();
}

void BWindow::AddChild(BView *child, BView *before)
{
	BAutolock locker(this);
	if (locker.IsLocked())
		m->top_view.AddChild(child, before);
}

bool BWindow::RemoveChild(BView *child)
{
	BAutolock locker(this);
	if (!locker.IsLocked())
		return false;

	return m->top_view.RemoveChild(child);
}

int32 BWindow::CountChildren() const
{
	BAutolock locker(const_cast<BWindow *>(this));
	if (!locker.IsLocked())
		return 0;

	return m->top_view.CountChildren();
}

BView *BWindow::ChildAt(int32 index) const
{
	BAutolock locker(const_cast<BWindow *>(this));
	if (!locker.IsLocked())
		return NULL;

	return m->top_view.ChildAt(index);
}

void BWindow::DispatchMessage(BMessage *message, BHandler *handler)
{
	if (!message)
		return;

	switch (message->what) {
		case B_ZOOM:
			Zoom();
			break;

		case B_MINIMIZE: {
			bool minimize;
			if (message->FindBool("minimize", &minimize) == B_OK)
				Minimize(minimize);
			break;
		}

		case B_WINDOW_RESIZED: {
			int32 width, height;
			if (message->FindInt32("width", &width) == B_OK
				&& message->FindInt32("height", &height) == B_OK) {
				// combine with pending resize notifications
				BMessage *pendingMessage;
				while ((pendingMessage = MessageQueue()->FindMessage(B_WINDOW_RESIZED, 0))) {
					MessageQueue()->RemoveMessage(pendingMessage);

					int32 nextWidth;
					if (pendingMessage->FindInt32("width", &nextWidth) == B_OK)
						width = nextWidth;

					int32 nextHeight;
					if (pendingMessage->FindInt32("height", &nextHeight) == B_OK) {
						height = nextHeight;
					}

					delete pendingMessage;
					// this deletes the first *additional* message
					// fCurrentMessage is safe
				}
				const auto bounds = Bounds();
				if (width != bounds.Width() || height != bounds.Height()) {
					m->resize(width, height);
				}

				FrameResized(width, height);
			}
			break;
		}

		case B_WINDOW_MOVED: {
			// Wayland does not provide window position information
			ALOGW("Window '%s' received B_WINDOW_MOVED message ¯\\_(ツ)_/¯", Name());
			break;
		}

		case B_WINDOW_ACTIVATED: {
			debugger("B_WINDOW_ACTIVATED");
			break;
		}

		case B_SCREEN_CHANGED: {
			debugger("B_SCREEN_CHANGED");
			break;
		}

		case B_WORKSPACE_ACTIVATED: {
			debugger("B_WORKSPACE_ACTIVATED");
			break;
		}

		case B_WORKSPACES_CHANGED: {
			debugger("B_WORKSPACES_CHANGED");
			break;
		}

		case B_KEY_DOWN:
		case B_KEY_UP:
		case B_UNMAPPED_KEY_DOWN:
		case B_UNMAPPED_KEY_UP: {
			if (handler && handler != this) {
				// pass on
				handler->MessageReceived(message);
				break;
			}

			const char *bytes = nullptr;
			if (message->FindString("bytes", &bytes) != B_OK || !bytes)
				break;

			BView *target = nullptr;
			// B_KEY_UP events are always assigned to the view that's in focus when the user releases the key
			// even if the previous B_KEY_DOWN message performed a shortcut, forced keyboard navigation, or was assigned to the default button
			if (message->what == B_KEY_DOWN) {
				char key = bytes[0];

				uint32 modifiers;
				if (message->FindInt32("modifiers", (int32 *)&modifiers) != B_OK)
					modifiers = 0;

				// Optionally close window when the escape key is pressed
				if (key == B_ESCAPE && (Flags() & B_CLOSE_ON_ESCAPE) != 0) {
					BMessage command(B_QUIT_REQUESTED);
					command.AddBool("shortcut", true);
					this->PostMessage(&command);
					break;
				}

				// intercepts some keys (like menu shortcuts and the Command+W close window sequence)
				if (modifiers & B_COMMAND_KEY) {
					if (key == 'w') {
						BMessage command(B_QUIT_REQUESTED);
						command.AddBool("shortcut", true);
						this->PostMessage(&command);
						break;
					}
					if (key == 'q') {
						BMessage command(B_QUIT_REQUESTED);
						command.AddBool("shortcut", true);
						be_app->PostMessage(&command);
						break;
					}
					target = CurrentFocus();
					if (target) {
						if (key == 'a') {
							BMessage command(B_SELECT_ALL);
							command.AddBool("shortcut", true);
							target->MessageReceived(&command);
							break;
						}
						if (key == 'z') {
							BMessage command(B_UNDO);
							command.AddBool("shortcut", true);
							target->MessageReceived(&command);
							break;
						}
						if (key == 'x') {
							BMessage command(B_CUT);
							command.AddBool("shortcut", true);
							target->MessageReceived(&command);
							break;
						}
						if (key == 'c') {
							BMessage command(B_COPY);
							command.AddBool("shortcut", true);
							target->MessageReceived(&command);
							break;
						}
						if (key == 'v') {
							BMessage command(B_PASTE);
							command.AddBool("shortcut", true);
							target->MessageReceived(&command);
							break;
						}
					}
				}
				if (modifiers & B_OPTION_KEY) {
					if (key == B_TAB) {
						// Keyboard navigation through views
						// (B_OPTION_KEY makes BTextViews and friends navigable, even in editing mode)
						// TODO: _KeyboardNavigation();
						break;
					}
				}

				// if we have a default button, it might want to hear
				// about pressing the <enter> key
				target = DefaultButton();
				if (target) {
					const int32 kNonLockModifierKeys = B_SHIFT_KEY | B_COMMAND_KEY | B_CONTROL_KEY | B_OPTION_KEY | B_MENU_KEY;
					int32		rawChar;
					int32		modifiers;
					if (message->FindInt32("raw_char", &rawChar) == B_OK
						&& message->FindInt32("modifiers", &modifiers) == B_OK
						&& rawChar == B_ENTER
						&& (modifiers & kNonLockModifierKeys) == 0) {
						target->MessageReceived(message);
						break;
					}
				}
			}

			// pass the message along to the currently focused view
			target = CurrentFocus();
			if (target) {
				target->MessageReceived(message);
				break;
			}

			break;
		}

		case B_MOUSE_MOVED:
		case B_MOUSE_DOWN:
		case B_MOUSE_UP: {
			if (handler && handler != this) {
				// pass on
				handler->MessageReceived(message);
				break;
			}

			BPoint screen_where;
			if (message->FindPoint("screen_where", 0, &screen_where) != B_OK)
				break;

			BPoint view_where{screen_where};
			ConvertFromScreen(&view_where);
			BView *viewUnderPointer = &m->top_view;
			bool   stop				= false;
			while (!stop) {
				stop = true;
				for (BView *child = viewUnderPointer->fFirstChild; child; child = child->fNextSibling) {
					if (child->Frame().Contains(view_where)) {
						viewUnderPointer = child;
						child->ConvertFromParent(&view_where);
						stop = false;
						break;
					}
				}
			}
			// ALOGV("found viewUnderPointer %p %s %x", viewUnderPointer, viewUnderPointer->Name(), fViewsEvents);

			if (fViewsEvents & B_POINTER_EVENTS) {
				bool still_wanted_pointer_events = false;

				std::function<void(BView *)> send_events = [&](BView *view) {
					for (BView *child = view->fFirstChild; child; child = child->fNextSibling) {
						if (child->EventMask() & B_POINTER_EVENTS) {
							still_wanted_pointer_events = true;

							if (child != viewUnderPointer) {
								BMessage message_copy(*message);
								if (message_copy.what == B_MOUSE_MOVED) {
									message_copy.AddUInt32("be:transit", B_OUTSIDE_VIEW);
								}
								else {
									message_copy.AddPoint("where", child->ConvertFromScreen(screen_where));
								}
								message_copy.AddPoint("be:view_where", child->ConvertFromScreen(screen_where));
								child->MessageReceived(&message_copy);
							}
						}

						send_events(child);
					}
				};

				send_events(&m->top_view);
				if (!still_wanted_pointer_events) {
					fViewsEvents &= ~B_POINTER_EVENTS;
				}
			}

			if (message->what == B_MOUSE_MOVED) {
				if (fLastMouseMovedView != viewUnderPointer) {
					if (fLastMouseMovedView) {
						BMessage message_copy(*message);
						message_copy.AddUInt32("be:transit", B_EXITED_VIEW);
						message_copy.AddPoint("be:view_where", fLastMouseMovedView->ConvertFromScreen(screen_where));
						fLastMouseMovedView->MessageReceived(&message_copy);
					}
					message->AddUInt32("be:transit", B_ENTERED_VIEW);
				}
				else {
					message->AddUInt32("be:transit", B_INSIDE_VIEW);
				}
			}
			else {
				message->AddPoint("where", view_where);
			}

			message->AddPoint("be:view_where", view_where);
			viewUnderPointer->MessageReceived(message);

			fLastMouseMovedView = viewUnderPointer;
			break;
		}

		case B_PULSE:
			if (handler == this) {
				bool still_wanted_pulses = false;

				std::function<void(BView *)> send_pulse = [&](BView *view) {
					for (BView *child = view->fFirstChild; child; child = child->fNextSibling) {
						if (child->Flags() & B_PULSE_NEEDED) {
							still_wanted_pulses = true;
							child->Pulse();
						}

						send_pulse(child);
					}
				};

				send_pulse(&m->top_view);
				if (!still_wanted_pulses) {
					fViewsEvents &= ~B_PULSE_NEEDED;
				}
			}
			else
				handler->MessageReceived(message);
			break;

		case _UPDATE_:
			ALOGV("_UPDATE_ @ %s, handler: %s",
				  Name(), handler ? handler->Name() : "(null)");

			if (!handler) {
				ALOGE("%s: Dropping _UPDATE_ with no handler", Name());
				break;
			}

			handler->MessageReceived(message);
			break;
		case _UPDATE_IF_NEEDED_:
			// _UPDATE_IF_NEEDED_ will exit message draining loop and allow repaint to happen
			break;

		default:
			BLooper::DispatchMessage(message, handler);
			break;
	}
}

void BWindow::MessageReceived(BMessage *message)
{
	ALOGV("MessageReceived 0x%x: %.4s", message->what, (char *)&message->what);
	switch (message->what) {
		case _UPDATE_: {
			BMessage message(_UPDATE_);
			message.AddRect("updateRect", Bounds());
			PostMessage(&message, &m->top_view);
			break;
		}

		default:
			BLooper::MessageReceived(message);
	}
}

void BWindow::FrameMoved(BPoint new_position)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::WorkspacesChanged(uint32 old_ws, uint32 new_ws)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::WorkspaceActivated(int32 ws, bool state)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::FrameResized(float new_width, float new_height)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::Minimize(bool minimize)
{
	if (IsModal() || IsFloating() || IsHidden() || m->minimized == minimize || !Lock())
		return;

	m->minimize(minimize);

	Unlock();
}

void BWindow::Zoom(BPoint rec_position, float rec_width, float rec_height)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::Zoom()
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::ScreenChanged(BRect screen_size, color_space depth)
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::SetPulseRate(bigtime_t rate)
{
	if (fPulseRate == rate) return;

	fPulseRate = rate;
	_try_pulse();
}

bigtime_t BWindow::PulseRate() const
{
	return fPulseRate;
}

void BWindow::SetDefaultButton(BButton *button)
{
	if (fDefaultButton == button) return;

	const auto current_default = fDefaultButton;

	if (current_default) {
		fDefaultButton = nullptr;
		current_default->MakeDefault(false);
		current_default->Invalidate();
	}
	fDefaultButton = button;
	if (button) {
		button->MakeDefault(true);
		button->Invalidate();
	}
}

BButton *BWindow::DefaultButton() const
{
	return fDefaultButton;
}

void BWindow::MenusBeginning()
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::MenusEnded()
{
	debugger(__PRETTY_FUNCTION__);
}

bool BWindow::NeedsUpdate() const
{
	return MessageQueue()->FindMessage(_UPDATE_, 0);
}

void BWindow::UpdateIfNeeded()
{
	ALOGV("BWindow::UpdateIfNeeded %d", MessageQueue()->CountMessages());
	BMessage *pendingMessage;
	while ((pendingMessage = MessageQueue()->FindMessage(_UPDATE_, 0))) {
		MessageQueue()->RemoveMessage(pendingMessage);

		BHandler *handler = pendingMessage->_get_handler();
		if (handler)
			handler->MessageReceived(pendingMessage);
		else
			MessageReceived(pendingMessage);

		delete pendingMessage;
	}

	// this will break out of message drain loop and trigger repaint
	PostMessage(_UPDATE_IF_NEEDED_, this);
}

BView *BWindow::FindView(const char *view_name) const
{
	BAutolock locker(const_cast<BWindow *>(this));
	if (!locker.IsLocked())
		return nullptr;

	return m->top_view.FindView(view_name);
}

BView *BWindow::FindView(BPoint) const
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

BView *BWindow::CurrentFocus() const
{
	return fFocus;
}

void BWindow::WindowActivated(bool state)
{
	// the default implementation does nothing
}

void BWindow::ConvertToScreen(BPoint *pt) const
{
}
BPoint BWindow::ConvertToScreen(BPoint pt) const
{
	return pt;
}
void BWindow::ConvertFromScreen(BPoint *pt) const
{
}
BPoint BWindow::ConvertFromScreen(BPoint pt) const
{
	return pt;
}
void BWindow::ConvertToScreen(BRect *rect) const
{
}
BRect BWindow::ConvertToScreen(BRect rect) const
{
	return rect;
}
void BWindow::ConvertFromScreen(BRect *rect) const
{
}
BRect BWindow::ConvertFromScreen(BRect rect) const
{
	return rect;
}

void BWindow::Show()
{
	bool runCalled = true;
	if (Lock()) {
		runCalled = fRunCalled;

		fShowLevel -= 1;
		if (fShowLevel <= 0) {
			m->showWindow();
		}

		Unlock();
	}

	if (!runCalled) {
		// We are still Locked - Run will unlock
		Run();
	}
}

void BWindow::Hide()
{
	if (Lock()) {
		// If we are minimized and are about to be hidden, unminimize
		if (IsMinimized() && fShowLevel == 0)
			Minimize(false);

		fShowLevel += 1;
		if (fShowLevel > 0)
			m->hideWindow();

		Unlock();
	}
}

bool BWindow::IsHidden() const
{
	return fShowLevel > 0;
}

bool BWindow::IsMinimized() const
{
	return m->minimized;
}

BRect BWindow::Bounds() const
{
	BPoint dim(m->width, m->height);
	if (!m->info.isEmpty())
		dim.Set(m->info.width(), m->info.height());

	return BRect(B_ORIGIN, dim - BPoint{1, 1});
}

BRect BWindow::Frame() const
{
	// On Wayland we do not know the position ow the window on screen
	return Bounds();
}

const char *BWindow::Title() const
{
	return m->title;
}

void BWindow::SetTitle(const char *title)
{
	if (!title)
		title = "";

	if (m->title)
		free(const_cast<char *>(m->title));
	m->title = strdup(title);

	char name[B_OS_NAME_LENGTH];
	name[0] = 'w';
	name[1] = '>';
	strncpy(name + 2, m->title, B_OS_NAME_LENGTH - 2);
	name[B_OS_NAME_LENGTH - 1] = '\0';

	SetName(name);
	if (fRunCalled && fThread != B_ERROR) {
		rename_thread(fThread, name);
	}
}

BView *BWindow::LastMouseMovedView() const
{
	return fLastMouseMovedView;
}

BHandler *BWindow::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier, int32 form, const char *property)
{
	debugger(__PRETTY_FUNCTION__);
	return nullptr;
}

status_t BWindow::GetSupportedSuites(BMessage *data)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BWindow::SetType(window_type type)
{
	status_t status = SetLook(type2look(type));
	if (status == B_OK)
		status = SetFeel(type2feel(type));

	return status;
}

window_type BWindow::Type() const
{
	debugger(__PRETTY_FUNCTION__);
	return B_UNTYPED_WINDOW;
}

status_t BWindow::SetLook(window_look look)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

window_look BWindow::Look() const
{
	return fLook;
}

status_t BWindow::SetFeel(window_feel feel)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

window_feel BWindow::Feel() const
{
	return fFeel;
}

status_t BWindow::SetFlags(uint32 flags)
{
	fFlags = flags;
	return B_OK;
}

uint32 BWindow::Flags() const
{
	return fFlags;
}

bool BWindow::IsModal() const
{
	return fFeel == B_MODAL_SUBSET_WINDOW_FEEL
		   || fFeel == B_MODAL_APP_WINDOW_FEEL
		   || fFeel == B_MODAL_ALL_WINDOW_FEEL;
}

bool BWindow::IsFloating() const
{
	return fFeel == B_FLOATING_SUBSET_WINDOW_FEEL
		   || fFeel == B_FLOATING_APP_WINDOW_FEEL
		   || fFeel == B_FLOATING_ALL_WINDOW_FEEL;
}

bool BWindow::QuitRequested()
{
	return BLooper::QuitRequested();
}

thread_id BWindow::Run()
{
	return BLooper::Run();
}

void BWindow::task_looper()
{
	/* !!! Keep this implementation in sync with BLooper::task_looper */

	ALOGD("BWindow::task_looper()");
	// Check that looper is locked (should be)
	AssertLocked();
	// Unlock the looper
	Unlock();

	if (IsLocked())
		debugger("task_looper() cannot unlock Looper");

	// create epoll instance for monitoring incoming events
	int epollfd = epoll_create1(0);
	if (epollfd == -1) {
		debugger("epoll_create1");
	}

	int				   wl_fd = wl_display_get_fd(m->wl_display);
	struct epoll_event ev;
	ev.events  = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = wl_fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, wl_fd, &ev) == -1) {
		debugger("epoll_ctl wl_fd");
	}

	int msg_fd = _get_thread_data_read_fd();
	ev.events  = EPOLLIN;
	ev.data.fd = wl_fd;

#define MAX_EVENTS 4
	struct epoll_event events[MAX_EVENTS];
	int				   nfds;
	bool			   wl_did_read = false;

	// loop: As long as we are not terminating.
	while (!fTerminating) {
		// process all pending events before waiting
		while (wl_display_prepare_read(m->wl_display) != 0)
			wl_display_dispatch_pending(m->wl_display);
		// send all outgoing messages (if any)
		wl_display_flush(m->wl_display);
		wl_did_read = false;

		// wait until something happens
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, fPulseRate > 0 ? fPulseRate / 1000 : -1);
		if (nfds == -1) {
			debugger("epoll_wait");
		}

		for (auto n = 0; n < nfds; ++n) {
			if (events[n].data.fd == wl_fd) {
				if (events[n].events & EPOLLHUP) {
					throw std::system_error(std::error_code(errno, std::system_category()), "Connection to display terminated");
					abort();
				}

				if (!(events[n].events & EPOLLERR)) {
					wl_display_read_events(m->wl_display);
					wl_did_read = true;
				}
			}

			if (events[n].data.fd == msg_fd) {
				if (events[n].events & EPOLLERR) {
					debugger("failed waiting for data");
				}

				if (events[n].events & EPOLLIN) {
					thread_id sender;
					uint32	  code = receive_data(&sender, nullptr, 0);
					ALOGD("received data from %d: %.4s", sender, (char *)&code);
				}
			}
		}

		if (wl_did_read)
			// process incoming events (if any)
			wl_display_dispatch_pending(m->wl_display);
		else
			wl_display_cancel_read(m->wl_display);

		_try_pulse();

		// only when not waiting for surface ready
		if (!m->surface_committed) {
			// process messages like BLooper
			Lock();
			_drain_message_queue();
			if (!fTerminating) {
				Unlock();
			}

			if (m->surface_pending) {
				m->surface_pending = false;
				wl_surface_commit(m->wl_surface);
				m->surface_committed = true;
			}
		}
	}

	ALOGD("BWindow::task_looper() done");
}

void BWindow::set_focus(BView *focusView, bool notifyInputServer)
{
	if (fFocus == focusView)
		return;

	// we notify the input server if we are passing focus
	// from a view which has the B_INPUT_METHOD_AWARE to a one
	// which does not, or vice-versa

	fFocus = focusView;
	SetPreferredHandler(focusView);
}
