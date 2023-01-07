#include <Application.h>
#include <Box.h>
#include <StringView.h>
#include <Window.h>

int main()
{
	new BApplication("application/x-vnd.BoxesTest");

	BWindow mainWindow({0, 0, 580, 300}, "Boxes Test", B_TITLED_WINDOW);

	// --- B_FANCY_BORDER
	BRect pos{10, 10, 190, 60};
	BBox  fancy(pos);
	mainWindow.AddChild(&fancy);
	BStringView fancy_content(fancy.Bounds().InsetByCopy(10, 14), nullptr, "Fancy Box");
	fancy.AddChild(&fancy_content);

	pos.OffsetBy(0, 60);
	BBox fancy_label(pos);
	fancy_label.SetLabel("String Label");
	mainWindow.AddChild(&fancy_label);
	BStringView fancy_label_content(fancy_label.Bounds().InsetByCopy(10, 14), nullptr, "Fancy Box with Label");
	fancy_label.AddChild(&fancy_label_content);

	pos.OffsetBy(0, 60);
	BBox		fancy_view(pos);
	BStringView fancy_view_label({100, 100, 200, 110}, nullptr, "StringView Label");
	fancy_view.SetLabel(&fancy_view_label);
	mainWindow.AddChild(&fancy_view);
	BStringView fancy_view_content(fancy_view.Bounds().InsetByCopy(10, 14), nullptr, "Fancy Box with StringView Label");
	fancy_view.AddChild(&fancy_view_content);

	// --- B_PLAIN_BORDER
	pos.OffsetTo(200, 10);
	BBox plain(pos);
	plain.SetBorder(B_PLAIN_BORDER);
	mainWindow.AddChild(&plain);
	BStringView plain_content(plain.Bounds().InsetByCopy(10, 14), nullptr, "Plain Box");
	plain.AddChild(&plain_content);

	pos.OffsetBy(0, 60);
	BBox plain_label(pos);
	plain_label.SetBorder(B_PLAIN_BORDER);
	plain_label.SetLabel("String Label");
	mainWindow.AddChild(&plain_label);
	BStringView plain_label_content(plain_label.Bounds().InsetByCopy(10, 14), nullptr, "Plain Box with Label");
	plain_label.AddChild(&plain_label_content);

	pos.OffsetBy(0, 60);
	BBox plain_view(pos);
	plain_view.SetBorder(B_PLAIN_BORDER);
	BStringView plain_view_label({100, 100, 200, 110}, nullptr, "StringView Label");
	plain_view.SetLabel(&plain_view_label);
	mainWindow.AddChild(&plain_view);
	BStringView plain_view_content(plain_view.Bounds().InsetByCopy(10, 14), nullptr, "Plain Box with StringView Label");
	plain_view.AddChild(&plain_view_content);

	// --- B_NO_BORDER
	pos.OffsetTo(390, 10);
	BBox none(pos);
	none.SetBorder(B_NO_BORDER);
	mainWindow.AddChild(&none);
	BStringView none_content(none.Bounds().InsetByCopy(10, 14), nullptr, "NoBorder Box");
	none.AddChild(&none_content);

	pos.OffsetBy(0, 60);
	BBox none_label(pos);
	none_label.SetBorder(B_NO_BORDER);
	none_label.SetLabel("String Label");
	mainWindow.AddChild(&none_label);
	BStringView none_label_content(none_label.Bounds().InsetByCopy(10, 14), nullptr, "NoBorder Box with Label");
	none_label.AddChild(&none_label_content);

	pos.OffsetBy(0, 60);
	BBox none_view(pos);
	none_view.SetBorder(B_NO_BORDER);
	BStringView none_view_label({100, 100, 200, 110}, nullptr, "StringView Label");
	none_view.SetLabel(&none_view_label);
	mainWindow.AddChild(&none_view);
	BStringView none_view_content(none_view.Bounds().InsetByCopy(10, 14), nullptr, "NoBorder Box with StringView Label");
	none_view.AddChild(&none_view_content);

	mainWindow.Show();

	be_app->Run();
	return EXIT_SUCCESS;
}
