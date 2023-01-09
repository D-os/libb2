#include <Application.h>
#include <CheckBox.h>
#include <Slider.h>
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

class CustomSlider : public BSlider
{
   public:
	CustomSlider(BRect		 frame,
				 const char *name,
				 const char *label,
				 BMessage	*message,
				 int32		 minValue,
				 int32		 maxValue,
				 thumb_style thumbType	  = B_BLOCK_THUMB,
				 uint32		 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 uint32		 flags		  = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS)
		: BSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags)
	{
	}

	const rgb_color kWhiteGray	= {235, 235, 235, 255};
	const rgb_color kDarkGray	= {100, 100, 100, 255};
	const rgb_color kBlackColor = {0, 0, 0, 255};

	void DrawThumb(void)
	{
		BRect  r;
		BView *v;
		auto   hi = HighColor();

		// Get the frame rectangle of the thumb
		// and the offscreen view.

		r = ThumbFrame();
		v = OffscreenView();

		// Draw the black shadow

		v->SetHighColor(kBlackColor);
		r.top++;
		r.left++;
		v->StrokeEllipse(r);

		// Draw the dark grey edge

		v->SetHighColor(kDarkGray);
		r.bottom--;
		r.right--;
		v->StrokeEllipse(r);

		// Fill the inside of the thumb

		v->SetHighColor(kWhiteGray);
		r.InsetBy(1, 1);
		v->FillEllipse(r);

		v->SetHighColor(hi);
	}
};

class DestView : public BView
{
   public:
	DestView(BRect frame) : BView(frame, "obliterate_view", B_FOLLOW_ALL, 0)
	{
		SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		BRect r(Bounds());

		auto slider1 = new CustomSlider(r, "obl:slider1", "Totality of Damage (%)", nullptr,
										0, 100);
		slider1->SetBarColor(make_color(80, 80, 80));
		auto fill_color = make_color(255, 102, 102);
		slider1->UseFillColor(true, &fill_color);
		slider1->SetHashMarks(B_HASH_MARKS_TOP);
		slider1->SetHashMarkCount(10);
		slider1->SetLimitLabels("None", "Total");
		AddChild(slider1);

		r.OffsetBy(0, 55);
		auto slider2 = new BSlider(r, "obl:slider2", "Acceptable Collateral Losses (%)", nullptr,
								   0, 100);
		slider2->SetBarColor(make_color(80, 80, 80));
		slider2->UseFillColor(true, &fill_color);
		slider2->SetHashMarks(B_HASH_MARKS_BOTTOM);
		slider2->SetHashMarkCount(10);
		slider2->SetLimitLabels("None", "Massive");
		AddChild(slider2);

		r.OffsetBy(0, 58);
		r.bottom	  = r.top;
		BView *check1 = new BCheckBox(r, "obl:check1", "Use Nuclear Weapons", nullptr);
		AddChild(check1);

		r.OffsetBy(0, 16);
		BView *check2 = new BCheckBox(r, "obl:check2", "Warn Civilians First", nullptr);
		AddChild(check2);
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
