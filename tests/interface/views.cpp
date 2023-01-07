#include <Application.h>
#include <View.h>
#include <Window.h>

class TestView : public BView
{
   public:
	TestView(BRect rect, const char *name, rgb_color color) : BView(rect, name, 0, B_WILL_DRAW)
	{
		SetHighColor(color);
	}

	void Draw(BRect updateRect) override
	{
		StrokeRect(updateRect);
	}
};

class TestViewsWin : public BWindow
{
   public:
	TestViewsWin(BRect frame) : BWindow(frame, "Hello", B_TITLED_WINDOW,
										B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
	{
		auto red1 = new TestView({10, 10, 90, 90}, "Red1", {255, 0, 0, 255});
		AddChild(red1);
		auto red11 = new TestView({10, 10, 20, 20}, "Red11", {255, 0, 0, 255});
		auto red12 = new TestView({30, 30, 50, 50}, "Red12", {255, 0, 0, 255});
		red1->AddChild(red11);
		red1->AddChild(red12);
		auto red121 = new TestView({10, 10, 30, 30}, "Red121", {255, 0, 0, 255});
		red12->AddChild(red121);

		auto wrapper = new BView(Bounds().OffsetBySelf({100, 0}), "Wrapper", 0, 0);
		AddChild(wrapper);
		auto blue1 = new TestView({10, 10, 90, 90}, "Blue1", {0, 0, 255, 255});
		wrapper->AddChild(blue1);
		auto blue11 = new TestView({10, 10, 20, 20}, "Blue11", {0, 0, 255, 255});
		auto blue12 = new TestView({30, 30, 50, 50}, "Blue12", {0, 0, 255, 255});
		blue1->AddChild(blue11);
		blue1->AddChild(blue12);
		auto blue121 = new TestView({10, 10, 30, 30}, "Blue121", {0, 0, 255, 255});
		blue12->AddChild(blue121);
	}

	virtual bool QuitRequested()
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};

class TestViewsApp : public BApplication
{
   public:
	TestViewsApp() : BApplication("application/x-vnd.TestViews")
	{
		TestViewsWin *wnd;
		BRect		  rect;

		rect.Set(0, 0, 200, 200);
		wnd = new TestViewsWin(rect);

		wnd->Show();
	}
};

int main(int argc, char **argv)
{
	TestViewsApp app;
	app.Run();
	return 0;
}
