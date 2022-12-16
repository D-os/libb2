#include <Application.h>
#include <Box.h>
#include <RadioButton.h>
#include <Window.h>

int main()
{
	new BApplication("application/x-vnd.RadiosTest");

	BWindow main_window({0, 0, 300, 200}, "Radios Test", B_TITLED_WINDOW);

	BBox		 box(BRect(10, 10, 190, 90), "Bounding Box");
	BRadioButton box_radio1(BRect(10, 10, box.Bounds().right - 10, 30), "Boxed Radio 1", "Radio button 1", nullptr);
	box.AddChild(&box_radio1);
	BRadioButton box_radio2(BRect(10, 30, box.Bounds().right - 10, 50), "Boxed Radio 2", "Radio button 2", nullptr);
	box.AddChild(&box_radio2);
	BRadioButton box_radio3(BRect(10, 50, box.Bounds().right - 10, 70), "Boxed Radio 3", "Radio button 3", nullptr);
	box.AddChild(&box_radio3);
	main_window.AddChild(&box);

	BView		 view(BRect(10, 100, 190, 180), "Bounding View", B_FOLLOW_ALL, 0);
	BRadioButton view_radio1(BRect(10, 10, view.Bounds().right - 10, 30), "View Radio 1", "Radio button 1", nullptr);
	view_radio1.ResizeToPreferred();
	view.AddChild(&view_radio1);
	BRadioButton view_radio2(BRect(10, 30, view.Bounds().right - 10, 50), "View Radio 2", "Radio button 2", nullptr);
	view_radio2.ResizeToPreferred();
	view.AddChild(&view_radio2);
	BRadioButton view_radio3(BRect(10, 50, view.Bounds().right - 10, 70), "View Radio 3", "Radio button 3", nullptr);
	view_radio3.ResizeToPreferred();
	view.AddChild(&view_radio3);
	main_window.AddChild(&view);

	BRadioButton radio_yes(BRect(200, 50, 200, 50), "✔", "Yes", nullptr);
	radio_yes.ResizeToPreferred();
	main_window.AddChild(&radio_yes);
	BRadioButton radio_no(BRect(200, 100, 200, 100), "✘", "No", nullptr);
	radio_no.ResizeToPreferred();
	main_window.AddChild(&radio_no);

	main_window.Show();

	be_app->Run();
	return EXIT_SUCCESS;
}
