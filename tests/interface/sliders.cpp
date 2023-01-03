#include <Application.h>
#include <Slider.h>
#include <StringView.h>
#include <TabView.h>
#include <Window.h>

class ConstView : public BView
{
   public:
	ConstView(BRect frame) : BView(frame, "construct_view", B_FOLLOW_ALL, 0)
	{
		SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		BRect r(Bounds());

		auto slider1 = new BSlider(r, "const:slider1", "Construct Speed (%)", nullptr,
								   0, 140);
		slider1->SetBarColor(make_color(152, 152, 255));
		slider1->SetHashMarks(B_HASH_MARKS_BOTTOM);
		slider1->SetHashMarkCount(10);
		slider1->SetLimitLabels("Slow", "Fast");
		AddChild(slider1);

		r.OffsetBy(0, 55);
		auto slider2 = new BSlider(r, "const:slider2", "Safety Factor", nullptr,
								   0, 100, B_TRIANGLE_THUMB);
		slider2->SetBarColor(make_color(152, 152, 255));
		auto fill_color = make_color(255, 102, 102);
		slider2->UseFillColor(true, &fill_color);
		slider2->SetHashMarks(B_HASH_MARKS_BOTH);
		slider2->SetHashMarkCount(6);
		slider2->SetLimitLabels("Deadly", "Safe");
		AddChild(slider2);
	}
};

class DestView : public BView
{
   public:
	DestView(BRect frame) : BView(frame, "obliterate_view", B_FOLLOW_ALL, 0)
	{
		SetViewColor(make_color(200, 0, 0));

		auto view = new BStringView(Bounds(), "obliterate_stringview", "Obliterate");
		AddChild(view);
	}
};

class TestWindow : public BWindow
{
   public:
	TestWindow() : BWindow({0, 0, 268, 200}, "Sliders & Tabs", B_TITLED_WINDOW)
	{
		BRect	  r;
		BTabView *tabView;
		BTab	 *tab;

		r = Bounds();
		r.InsetBy(5, 5);

		tabView = new BTabView(r, "tab_view");
		tabView->SetViewColor(216, 216, 216, 0);

		r = tabView->Bounds();
		r.InsetBy(5, 5);
		r.bottom -= tabView->TabHeight();
		tab = new BTab();
		tabView->AddTab(new ConstView(r), tab);
		tab->SetLabel("Construct");
		tab = new BTab();
		tabView->AddTab(new DestView(r), tab);
		tab->SetLabel("Obliterate");

		this->AddChild(tabView);
	}
};

int main(int argc, char *argv[])
{
	BApplication app("application/x-vnd.test.sliders");

	TestWindow win;
	win.Show();

	be_app->Run();

	return EXIT_SUCCESS;
}
