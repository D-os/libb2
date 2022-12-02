#include <Application.h>
#include <StringView.h>
#include <Window.h>

class HelloWorldView : public BStringView
{
   public:
	HelloWorldView(BRect rect, const char *name, const char *text)
		: BStringView(rect, name, text, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW | B_PULSE_NEEDED)
	{
		SetFont(be_bold_font);
		SetFontSize(24);
	}
};

class HelloWorldWin : public BWindow
{
   public:
	HelloWorldWin(BRect frame) : BWindow(frame, "Hello", B_TITLED_WINDOW,
										 B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
	{
		HelloWorldView *view;
		BRect			rect(Bounds());
		view = new HelloWorldView(rect, "HelloWorldView", "Hello, World!");
		AddChild(view);
	}

	virtual bool QuitRequested()
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};

class HelloWorldApp : public BApplication
{
   public:
	HelloWorldApp() : BApplication("application/x-vnd.Be-HelloWorld")
	{
		HelloWorldWin *wnd;
		BRect		   rect;

		rect.Set(100, 80, 260, 120);
		wnd = new HelloWorldWin(rect);

		wnd->Show();
	}
};

int main(int argc, char **argv)
{
	HelloWorldApp app;
	app.Run();
	return 0;
}
