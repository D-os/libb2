#include <Application.h>
#include <Window.h>

class WindowApplication : public BApplication
{
   public:
	WindowApplication();
};

int main()
{
	WindowApplication windowApplication;
	windowApplication.Run();
}

WindowApplication::WindowApplication()
	: BApplication("application/x-vnd.testapp")
{
	// If allocated on the stack, the memory used by myWindow would be released
	// after exiting constructor, while still in use by the application.
	auto myWindow = new BWindow(BRect(10, 10, 210, 110), "Some Title", B_TITLED_WINDOW, 0);

	myWindow->SetSizeLimits(50, 500, 50, 500);

	// make window visible
	myWindow->Show();
}
