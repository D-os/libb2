#include "Window.h"

#define LOG_TAG "BWindow"

#include <Application.h>
#include <Autolock.h>
#include <Message.h>
#include <MessageQueue.h>
#include <Point.h>
#include <Rect.h>
#include <View.h>
#include <include/core/SkSurface.h>
#include <log/log.h>
#include <pimpl.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <cstddef>
#include <cstring>
#include <system_error>

#include "xdg-shell-client-protocol.h"

class BWindow::impl
{
   public:
	/* Globals */
	struct wl_display	  *wl_display;
	struct wl_registry   *wl_registry;
	struct wl_compositor *wl_compositor;
	struct wl_shm		  *wl_shm;
	struct xdg_wm_base   *xdg_wm_base;
	/* Objects */
	struct wl_surface	  *wl_surface;
	struct xdg_surface  *xdg_surface;
	struct xdg_toplevel *xdg_toplevel;

	/* Backing */
	size_t				  width;
	size_t				  height;
	struct wl_shm_pool   *wl_shm_pool;
	size_t				  shm_pool_size;
	int					  pool_fd;
	uint8_t			  *pool_data;
	struct wl_buffer	 *wl_buffer;

	/* Drawing */
	SkImageInfo		 info;
	sk_sp<SkSurface> surface;

	/* State */
	BView		top_view;
	const char *title;

	bool minimized;
	bool maximized;
	bool closed;

	impl()
		: wl_display{nullptr},
		  wl_registry{nullptr},
		  wl_compositor{nullptr},
		  wl_shm{nullptr},
		  xdg_wm_base{nullptr},
		  wl_surface{nullptr},
		  xdg_surface{nullptr},
		  xdg_toplevel{nullptr},
		  width{0},
		  height{0},
		  wl_shm_pool{nullptr},
		  shm_pool_size{0},
		  pool_fd{-1},
		  pool_data{nullptr},
		  wl_buffer{nullptr},

		  top_view(BRect(B_ORIGIN, B_ORIGIN), "TopView", B_FOLLOW_ALL, B_WILL_DRAW),
		  title{nullptr},
		  minimized{false},
		  maximized{false},
		  closed{false},

		  registry_listener{
			  .global		 = registry_global_handler,
			  .global_remove = registry_global_remove_handler,
		  },
		  shm_listener{
			  .format = shm_format_handler,
		  },
		  surface_listener{
			  .enter = surface_enter_handler,
			  .leave = surface_leave_handler,
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
		  }
	{
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
	void showWindow(const size_t width, const size_t height);
	void hideWindow();
	void minimize(bool minimized);

   private:
	std::mutex pool_mutex;
	void	   resize_buffer();

	const struct wl_registry_listener registry_listener;
	const struct wl_shm_listener	  shm_listener;
	const struct wl_surface_listener  surface_listener;
	const struct wl_buffer_listener	  wl_buffer_listener;
	const struct xdg_surface_listener xdg_surface_listener;
	const struct xdg_wm_base_listener xdg_wm_base_listener;
	const struct xdg_toplevel_listener xdg_toplevel_listener;

	static void registry_global_handler(void *this_, struct wl_registry *registry, uint32_t name,
										const char *interface, uint32_t version);
	static void registry_global_remove_handler(void *this_, struct wl_registry *registry, uint32_t name);
	static void shm_format_handler(void *this_, struct wl_shm *wl_shm, uint32_t format);
	static void surface_enter_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
	static void surface_leave_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
	static void wl_buffer_release_handler(void *this_, struct wl_buffer *wl_buffer);
	static void xdg_surface_configure_handler(void *this_, struct xdg_surface *xdg_surface, uint32_t serial);
	static void xdg_wm_base_ping_handler(void *this_, struct xdg_wm_base *xdg_wm_base, uint32_t serial);
	static void xdg_toplevel_configure_handler(void *this_, struct xdg_toplevel *xdg_toplevel,
											   int32_t width, int32_t height, struct wl_array *states);
	static void xdg_toplevel_close_handler(void *this_, struct xdg_toplevel *toplevel);
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
	wl_registry_add_listener(wl_registry, &registry_listener, this);

	// wait for the "initial" set of globals to appear
	wl_display_roundtrip(wl_display);
	ALOGV("compositor %p, shm %p, xdg_wm_base %p", wl_compositor, wl_shm, xdg_wm_base);
	if (!wl_compositor || !wl_shm || !xdg_wm_base) return false;

	wl_surface = wl_compositor_create_surface(wl_compositor);
	if (!wl_surface) return false;
	ALOGV("created surface");
	wl_surface_add_listener(wl_surface, &surface_listener, this);

	xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, wl_surface);
	xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);
	if (!xdg_surface) return false;
	ALOGV("created xdg_surface");

	xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	if (!xdg_toplevel) return false;
	ALOGV("created xdg_toplevel");
	xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);
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

#ifndef NDEBUG
	/* Draw checkerboxed background */
	uint32_t *pixels = (uint32_t *)&pool_data[offset];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if ((x + y / 8 * 8) % 16 < 8)
				pixels[y * width + x] = 0xFF555555;
			else
				pixels[y * width + x] = 0xFFAAAAAA;
		}
	}
#endif

	surface = SkSurface::MakeRasterDirect(info, pool_data, stride);
}

void BWindow::impl::showWindow(size_t width, size_t height)
{
}

void BWindow::impl::hideWindow()
{
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
	wl_surface_commit(wl_surface);
}

void BWindow::impl::registry_global_handler(
	void			   *this_,
	struct wl_registry *registry,
	uint32_t			name,
	const char		   *interface,
	uint32_t			version)
{
	ALOGV("%u: %s (v.%u)", name, interface, version);

	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		static_cast<BWindow::impl *>(this_)->wl_compositor = (struct wl_compositor *)wl_registry_bind(
			registry, name, &wl_compositor_interface, 5);
	}
	else if (strcmp(interface, wl_shm_interface.name) == 0) {
		static_cast<BWindow::impl *>(this_)->wl_shm = (struct wl_shm *)wl_registry_bind(
			registry, name, &wl_shm_interface, 1);
		wl_shm_add_listener(static_cast<BWindow::impl *>(this_)->wl_shm,
							&static_cast<BWindow::impl *>(this_)->shm_listener, this_);
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		static_cast<BWindow::impl *>(this_)->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(
			registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(static_cast<BWindow::impl *>(this_)->xdg_wm_base,
								 &static_cast<BWindow::impl *>(this_)->xdg_wm_base_listener, this_);
	}
}
void BWindow::impl::registry_global_remove_handler(
	void			   *this_,
	struct wl_registry *registry,
	uint32_t			name) {}

void BWindow::impl::shm_format_handler(void *this_, struct wl_shm *wl_shm, uint32_t format)
{
	ALOGV("SHM format: 0x%x: %.4s", format,
		  format == 0 ? "ARG8"
					  : (format == 1 ? "XRG8"
									 : (char *)&format));
}

void BWindow::impl::surface_enter_handler(void *this_, struct wl_surface *surface, struct wl_output *output)
{
	ALOGV("%p surface enter %p", surface, output);
}
void BWindow::impl::surface_leave_handler(void *this_, struct wl_surface *surface, struct wl_output *output)
{
	ALOGV("%p surface leave %p", surface, output);
}

void BWindow::impl::wl_buffer_release_handler(void *this_, struct wl_buffer *wl_buffer)
{
	// Sent by the compositor when it's no longer using this buffer
	wl_buffer_destroy(wl_buffer);
}

void BWindow::impl::xdg_surface_configure_handler(void *this_, struct xdg_surface *xdg_surface, uint32_t serial)
{
	xdg_surface_ack_configure(xdg_surface, serial);

	static_cast<BWindow::impl *>(this_)->resize_buffer();
	wl_surface_commit(static_cast<BWindow::impl *>(this_)->wl_surface);
}

void BWindow::impl::xdg_wm_base_ping_handler(void *this_, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
	ALOGV("responding to PING");
	xdg_wm_base_pong(xdg_wm_base, serial);
}

void BWindow::impl::xdg_toplevel_configure_handler(void *this_, struct xdg_toplevel *xdg_toplevel,
												   int32_t width, int32_t height, struct wl_array *states)
{
	if (width == 0 || height == 0) {
		/* Compositor is deferring to us */
		return;
	}

	static_cast<BWindow::impl *>(this_)->width	= width;
	static_cast<BWindow::impl *>(this_)->height = height;
}

void BWindow::impl::xdg_toplevel_close_handler(void *this_, struct xdg_toplevel *toplevel)
{
	static_cast<BWindow::impl *>(this_)->closed = true;
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
	  fLook{look},
	  fFeel{feel}
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
					int32 nextWidth;
					if (pendingMessage->FindInt32("width", &nextWidth) == B_OK)
						width = nextWidth;

					int32 nextHeight;
					if (pendingMessage->FindInt32("height", &nextHeight)
						== B_OK) {
						height = nextHeight;
					}

					MessageQueue()->RemoveMessage(pendingMessage);
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
			ALOGW("Window '%s' received B_WINDOW_MOVED message", Name());
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

		case B_KEY_DOWN: {
			debugger("B_KEY_DOWN");
			break;
		}

		case B_UNMAPPED_KEY_DOWN: {
			debugger("B_UNMAPPED_KEY_DOWN");
			break;
		}

		case B_PULSE:
			// if (handler == this && fPulseRunner) {
			// 	m->top_view._Pulse();
			// }
			// else
			handler->MessageReceived(message);
			break;

		case _UPDATE_: {
			ALOGV("_UPDATE_ @ %s", Name());

			// TODO: repaint all child views if dirty
			// if topview is dirty fill it with ViewColor()

			break;
		}

		default:
			BLooper::DispatchMessage(message, handler);
			break;
	}
}

void BWindow::MessageReceived(BMessage *message)
{
	debugger(__PRETTY_FUNCTION__);
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

void BWindow::MenusBeginning()
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::MenusEnded()
{
	debugger(__PRETTY_FUNCTION__);
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

void BWindow::Show()
{
	bool runCalled = true;
	if (Lock()) {
		runCalled = fRunCalled;

		fShowLevel -= 1;
		if (fShowLevel <= 0)
			m->showWindow(150, 100);

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
	return BRect(B_ORIGIN, BPoint(m->info.width(), m->info.height()));
}

BRect BWindow::Frame() const
{
	// On Wayland we have our dedicated surface
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

status_t BWindow::SetFlags(uint32)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
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

	// loop: As long as we are not terminating.
	while (!fTerminating && wl_display_dispatch(m->wl_display)) {
		/* This space deliberately left blank */
	}

	ALOGD("BWindow::task_looper() done");
}
