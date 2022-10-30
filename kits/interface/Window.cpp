#include "Window.h"

#define LOG_TAG "BWindow"

#include <Rect.h>
#include <log/log.h>
#include <pimpl.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <cstring>
#include <system_error>

class BWindow::impl
{
   public:
	struct wl_display	  *display;
	struct wl_registry   *registry;
	struct wl_compositor *compositor;
	struct wl_surface	  *surface;
	struct wl_shm		  *shm;
	struct wl_shell		*shell;
	struct wl_shm_pool   *shm_pool;
	size_t				  shm_pool_size;
	int					  pool_fd;
	uint8_t			  *pool_data;
	struct wl_buffer	 *buffer;

	bool minimized;

	impl()
		: display{nullptr},
		  registry{nullptr},
		  compositor{nullptr},
		  surface{nullptr},
		  shm{nullptr},
		  shell{nullptr},
		  shm_pool{nullptr},
		  shm_pool_size{0},
		  pool_fd{-1},
		  pool_data{nullptr},
		  buffer{nullptr},

		  minimized{false},

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
		  }
	{
	}

	bool connect();
	void showWindow(const size_t width, const size_t height);
	void hideWindow();

   private:
	std::mutex mutex;

	struct wl_registry_listener registry_listener;
	struct wl_shm_listener		shm_listener;
	struct wl_surface_listener	surface_listener;

	static void registry_global_handler(void *this_, struct wl_registry *registry, uint32_t name,
										const char *interface, uint32_t version);
	static void registry_global_remove_handler(void *this_, struct wl_registry *registry, uint32_t name);
	static void shm_format_handler(void *this_, struct wl_shm *wl_shm, uint32_t format);
	static void surface_enter_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
	static void surface_leave_handler(void *this_, struct wl_surface *surface, struct wl_output *output);
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
	display = wl_display_connect(NULL);
	if (!display) return false;
	ALOGD("connected display");

	registry = wl_display_get_registry(display);
	if (!registry) return false;
	ALOGV("connected registry");
	wl_registry_add_listener(registry, &registry_listener, this);

	// wait for the "initial" set of globals to appear
	wl_display_roundtrip(display);
	ALOGV("compositor %p, shm %p, shell %p", compositor, shm, shell);
	if (!compositor || !shm || !shell) return false;

	surface = wl_compositor_create_surface(compositor);
	if (!surface) return false;
	ALOGV("created surface");
	wl_surface_add_listener(surface, &surface_listener, this);

	return true;
}

void BWindow::impl::showWindow(const size_t width, const size_t height)
{
	std::lock_guard<std::mutex> guard(mutex);

	const size_t stride			   = width * 4;
	const size_t new_shm_pool_size = height * stride * 2;  // *2 double buffer

	if (pool_fd < 0) {
		pool_fd = syscall(SYS_memfd_create, "wl_shm_buffer", 0);
	}
	ALOG_ASSERT(pool_fd >= 0, "Cannot create shared memory buffer file");

	if (new_shm_pool_size > shm_pool_size) {
		ftruncate(pool_fd, new_shm_pool_size);

		if (pool_data) {
			munmap(pool_data, shm_pool_size);
		}
		pool_data = (uint8_t *)mmap(NULL, shm_pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, pool_fd, 0);

		if (shm_pool) {
			wl_shm_pool_resize(shm_pool, new_shm_pool_size);
		}
	}

	if (!shm_pool) {
		shm_pool = wl_shm_create_pool(shm, pool_fd, shm_pool_size);
	}

	int			 index	= 0;
	const size_t offset = height * stride * index;

	if (buffer) {
		wl_buffer_destroy(buffer);
	}
	buffer = wl_shm_pool_create_buffer(shm_pool, offset, width, height, stride, WL_SHM_FORMAT_XRGB8888);

	uint32_t *pixels = (uint32_t *)&pool_data[offset];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if ((x + y / 8 * 8) % 16 < 8) {
				pixels[y * width + x] = 0xFF666666;
			}
			else {
				pixels[y * width + x] = 0xFFEEEEEE;
			}
		}
	}

	wl_surface_attach(surface, buffer, 0, 0);
	wl_surface_damage(surface, 0, 0, UINT32_MAX, UINT32_MAX);
	wl_surface_commit(surface);
}

void BWindow::impl::hideWindow()
{
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
		static_cast<BWindow::impl *>(this_)->compositor = (struct wl_compositor *)wl_registry_bind(
			registry, name, &wl_compositor_interface, 5);
	}
	else if (strcmp(interface, wl_shm_interface.name) == 0) {
		static_cast<BWindow::impl *>(this_)->shm = (struct wl_shm *)wl_registry_bind(
			registry, name, &wl_shm_interface, 1);
		wl_shm_add_listener(static_cast<BWindow::impl *>(this_)->shm, &static_cast<BWindow::impl *>(this_)->shm_listener, this_);
	}
	else if (strcmp(interface, "wl_shell") == 0) {
		static_cast<BWindow::impl *>(this_)->shell = (struct wl_shell *)wl_registry_bind(
			registry, name, &wl_shell_interface, 1);
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

#pragma mark - BWindow

BWindow::BWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
	: BWindow(frame, title, type2look(type), type2feel(type), flags, workspace)
{
}

BWindow::BWindow(BRect frame, const char *title, window_look look, window_feel feel, uint32 flags, uint32 workspace)
	: BLooper("w>", B_DISPLAY_PRIORITY),
	  fTitle{nullptr},
	  fShowLevel{1},
	  fFlags{flags},
	  fLook{look},
	  fFeel{feel}
{
	frame.left	 = roundf(frame.left);
	frame.top	 = roundf(frame.top);
	frame.right	 = roundf(frame.right);
	frame.bottom = roundf(frame.bottom);

	fFrame = frame;

	SetTitle(title);

	if (!m->connect()) {
		throw std::system_error(std::error_code(errno, std::system_category()), "Cannot connect display");
	}
}

BWindow::~BWindow() = default;

status_t BWindow::Archive(BMessage *data, bool deep) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BWindow::Quit()
{
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::DispatchMessage(BMessage *message, BHandler *handler)
{
	debugger(__PRETTY_FUNCTION__);
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
	debugger(__PRETTY_FUNCTION__);
}

void BWindow::Zoom(BPoint rec_position, float rec_width, float rec_height)
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

void BWindow::WindowActivated(bool state)
{
	debugger(__PRETTY_FUNCTION__);
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

BRect BWindow::Frame() const
{
	return fFrame;
}

const char *BWindow::Title() const
{
	return fTitle;
}

void BWindow::SetTitle(const char *title)
{
	if (!title)
		title = "";

	if (fTitle)
		free(fTitle);
	fTitle = strdup(title);

	char name[B_OS_NAME_LENGTH];
	name[0] = 'w';
	name[1] = '>';
	strncpy(name + 2, fTitle, B_OS_NAME_LENGTH - 2);
	name[B_OS_NAME_LENGTH - 1] = '\0';

	SetName(name);
	if (fRunCalled && fThread != B_ERROR) {
		rename_thread(fThread, name);
	}
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

status_t BWindow::Perform(perform_code d, void *arg)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool BWindow::QuitRequested()
{
	return BLooper::QuitRequested();
}

thread_id BWindow::Run()
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}

void BWindow::task_looper()
{
	debugger(__PRETTY_FUNCTION__);
}
