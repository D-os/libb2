/* --------------------------------------------------------------------------
 *
 * ETK++ --- The Easy Toolkit for C++ programing
 * Copyright (C) 2004-2006, Anthony Lee, All Rights Reserved
 *
 * ETK++ library is a freeware; it may be used and distributed according to
 * the terms of The MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * File: view-drawing.cpp
 *
 * --------------------------------------------------------------------------*/

#include <app/Application.h>
#include <interface/Polygon.h>
#include <interface/Region.h>
#include <interface/View.h>
#include <interface/Window.h>
#include <log/log.h>

#define TEST_VIEW_FOLLOW 0

#if TEST_VIEW_FOLLOW == 0
#define TEST_POINT 1
#define TEST_SQUARE_POINT 0
#define TEST_LINE 1
#define TEST_POLYGON 1
#define TEST_RECT_AND_REGION 1
#define TEST_ARC 1

#define TEST_FONT 1
#define TEST_FONT_STRING "Jump over the dog, 跳过那只狗."
#define TEST_FONT_FAMILY "AR PL KaitiM GB"
#define TEST_FONT_STYLE "Regular"
#define TEST_FONT_SIZE 24
#endif	// TEST_VIEW_FOLLOW

class TView : public BView
{
   public:
	TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags);
	virtual ~TView();

	virtual void Draw(BRect updateRect);
};

class TWindow : public BWindow
{
   public:
	TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace = B_CURRENT_WORKSPACE);
	virtual ~TWindow();

	virtual bool QuitRequested();
};

class TApplication : public BApplication
{
   public:
	TApplication();
	virtual ~TApplication();

	virtual void ReadyToRun();
};

TView::TView(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags)
{
}

TView::~TView()
{
}

void TView::Draw(BRect updateRect)
{
#if TEST_VIEW_FOLLOW == 0
	pattern pat = B_MIXED_COLORS;
#endif

#if TEST_POINT == 1	 // Test Point
	{
		PushState();
		SetDrawingMode(B_OP_COPY);
		float point_test_x = 0;
		for (int32 i = 0; i < 26; ++i) {
			SetHighColor(200, 50, 200);
			SetPenSize(i);
			StrokePoint(BPoint(point_test_x, (float)i / 2.f), i < 13 ? B_SOLID_HIGH : pat);

			SetHighColor(0, 0, 0);
			SetPenSize(3);
			StrokePoint(BPoint(point_test_x, (float)i / 2.f));

			point_test_x += (float)(2 * i + 1) / 2.f + 2.f;
		}

		BPoint pts[4]	= {BPoint(20, 30), BPoint(40, 30), BPoint(60, 30), BPoint(80, 30)};
		// uint8  alpha[4] = {255, 150, 100, 50};

#if TEST_SQUARE_POINT == 1
		SetSquarePointStyle(true);
#endif

		SetDrawingMode(B_OP_COPY);
		SetHighColor(255, 0, 0);
		SetPenSize(17);
		for (auto &point : pts) StrokePoint(point, pat);

		SetHighColor(0, 0, 0);
		SetPenSize(3);
		for (auto &point : pts) StrokePoint(point);

		// SetDrawingMode(B_OP_COPY);
		// SetHighColor(255, 0, 0);
		// SetPenSize(17);
		// for (int32 i = 0; i < 4; ++i) pts[i].x += 100;
		// StrokePoints(pts, 4, alpha, pat);

		PopState();
	}
#endif	// TEST_POINT
#if TEST_LINE == 1
	{
		PushState();

		SetDrawingMode(B_OP_COPY);

		SetHighColor(255, 0, 0);
		SetPenSize(7);
		BPoint pt(0, 50);
		MovePenTo(pt);
		for (int32 i = 0; i < 6; ++i) StrokeLine(pt += BPoint(30, (i % 2 == 0 ? 30 : -30)), pat);

		SetHighColor(0, 0, 0);
		SetPenSize(0);
		pt.x = 0;
		pt.y = 50;
		MovePenTo(pt);
		for (int32 i = 0; i < 6; ++i) StrokeLine(pt += BPoint(30, (i % 2 == 0 ? 30 : -30)));

		SetHighColor(255, 255, 0);
		SetPenSize(3);
		pt.x = 0;
		pt.y = 50;
		for (int32 i = -1; i < 6; ++i) StrokePoint(i < 0 ? pt : pt += BPoint(30, (i % 2 == 0 ? 30 : -30)));

		PopState();
	}
#endif	// TEST_LINE
#if TEST_POLYGON == 1
	{
		BPolygon poly;
		BPoint	 pt(220, 50);
		for (int32 i = -1; i < 6; ++i) {
			if (i >= 0) pt += BPoint(30, (i % 2 == 0 ? 30 : -30));
			poly.AddPoints(&pt, 1);
		}
		poly.PrintToStream();

		PushState();

		SetDrawingMode(B_OP_COPY);

		SetHighColor(255, 0, 0);
		SetPenSize(9);
		StrokePolygon(&poly, true, pat);

		SetHighColor(0, 0, 0);
		SetPenSize(0);
		StrokePolygon(&poly, true);

		SetHighColor(255, 255, 0);
		SetPenSize(3);
		const BPoint *polyPts = poly.Points();
		for (int32 i = 0; i < poly.CountPoints(); ++i) StrokePoint(*polyPts++);

		BPolygon aPoly(poly.Points(), 3);
		SetHighColor(0, 255, 0);
		FillPolygon(&aPoly);

		PopState();
	}
#endif	// TEST_POLYGON
#if TEST_RECT_AND_REGION == 1
	{
		PushState();

		SetDrawingMode(B_OP_COPY);

		SetHighColor(255, 0, 0);
		SetPenSize(9);
		StrokeRect(BRect(10, 100, 50, 120), pat);

		SetHighColor(0, 0, 0);
		SetPenSize(0);
		StrokeRect(BRect(10, 100, 50, 120));

		pattern apat = {0xcc, 0x66, 0x33, 0x99, 0xcc, 0x66, 0x33, 0x99};
		SetHighColor(0, 0, 0);
		SetLowColor(255, 255, 255);
		FillRect(BRect(70, 100, 110, 120), apat);

		SetHighColor(0, 0, 0);
		BRect r(150, 100, 200, 150);
		BRect r1(150, 100, 160, 110);
		BRect r2(190, 100, 200, 110);
		BRect r3(150, 140, 160, 150);
		BRect r4(190, 140, 200, 150);
		StrokeRect(r);
		StrokeRect(r1);
		StrokeRect(r2);
		StrokeRect(r3);
		StrokeRect(r4);
		SetHighColor(255, 0, 0);
		SetDrawingMode(B_OP_BLEND);
		FillRoundRect(r, 10, 10);
		SetDrawingMode(B_OP_COPY);
		StrokeRoundRect(r, 10, 10);

		BRect	rects[3] = {BRect(20, 130, 70, 180), BRect(50, 160, 150, 210), BRect(85, 195, 170, 240)};
		BRegion region;
		for (int8 i = 0; i < 3; ++i) region.Include(rects[i]);
		region.OffsetBy(200, 0);
		region.PrintToStream();

		SetHighColor(0, 0, 0);
		SetPenSize(0);
		for (auto &rect : rects) {
			StrokeRect(rect);
		}
		SetDrawingMode(B_OP_BLEND);
		SetHighColor(0, 0, 255);
		for (auto &rect : rects) {
			FillRect(rect);
		}

		SetHighColor(0, 255, 0);
		FillRegion(&region, apat);

		PopState();
	}
#endif	// TEST_RECT_AND_REGION
#if TEST_ARC == 1
	{
		PushState();

		BRect r(10, 260, 60, 310);

		SetDrawingMode(B_OP_COPY);
		SetHighColor(0, 0, 0);
		SetPenSize(0);
		StrokeRect(r);
		SetHighColor(0, 0, 255);
		StrokeArc(r, 0, 360);
		SetDrawingMode(B_OP_BLEND);
		SetHighColor(255, 0, 0);
		FillArc(r, 30, 240);

		r.OffsetBy(70, 0);
		SetDrawingMode(B_OP_COPY);
		SetHighColor(255, 0, 0);
		SetPenSize(9);
		StrokeArc(r, 0, 360);
		SetHighColor(0, 0, 0);
		SetPenSize(0);
		StrokeArc(r, 0, 360);

		r.OffsetBy(70, 0);
		SetHighColor(0, 0, 0);
		FillArc(r, 0, 360, pat);
		SetHighColor(255, 0, 0);
		SetPenSize(9);
		StrokeArc(r, 0, 360, pat);

		r.OffsetBy(70, 0);
		r.right += 50;
		SetHighColor(0, 0, 0);
		FillEllipse(r, pat);
		SetHighColor(255, 0, 0);
		SetPenSize(9);
		StrokeEllipse(r, pat);

		PopState();
	}
#endif	// TEST_ARC
#if TEST_FONT == 1
	{
		PushState();

		BPoint pt(10, 350);

		SetDrawingMode(B_OP_COPY);
		SetHighColor(0, 0, 0);
		ForceFontAliasing(true);
		SetFontSize(TEST_FONT_SIZE);
		DrawString(TEST_FONT_STRING, pt);

		SetDrawingMode(B_OP_COPY);
		SetLowColor(ViewColor());
		ForceFontAliasing(false);
		BFont font;
		GetFont(&font);
		font.SetFamilyAndStyle(TEST_FONT_FAMILY, TEST_FONT_STYLE);
		font.SetSize(TEST_FONT_SIZE);
		SetFont(&font);
		font_height fontHeight;
		font.GetHeight(&fontHeight);
		float strWidth	= font.StringWidth(TEST_FONT_STRING);

		pt += BPoint(0, fontHeight.leading);

		SetPenSize(0);
		// draw bounding rectangle
		SetHighColor(40, 40, 255);
		StrokeRect(BRect(pt + BPoint(0, -fontHeight.ascent), pt + BPoint(strWidth - 1, fontHeight.descent)));
		// draw baseline
		SetHighColor(255, 40, 40);
		StrokeLine(pt, pt + BPoint(strWidth, 0));
		// draw leading
		SetHighColor(40, 255, 40);
		StrokeLine(pt + BPoint(0, -fontHeight.ascent + fontHeight.leading),
				   pt + BPoint(strWidth, -fontHeight.ascent + fontHeight.leading));

		SetHighColor(0, 0, 0);
		DrawString(TEST_FONT_STRING, pt);

		PopState();
	}
#endif	// TEST_FONT
}

TWindow::TWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspace)
	: BWindow(frame, title, type, flags, workspace)
{
	BView *view_top = new TView(frame.OffsetToCopy(B_ORIGIN), NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(view_top);

#if TEST_VIEW_FOLLOW == 1
	BView *view = new BView(BRect(50, 50, 100, 100), NULL, B_FOLLOW_NONE, 0);
	view->SetViewColor(255, 0, 0);
	view_top->AddChild(view);

	view = new BView(BRect(50, 150, 100, 200), NULL, B_FOLLOW_RIGHT, 0);
	view->SetViewColor(0, 255, 0);
	view_top->AddChild(view);

	view = new BView(BRect(150, 250, 200, 300), NULL, B_FOLLOW_H_CENTER | B_FOLLOW_RIGHT, 0);
	view->SetViewColor(0, 0, 155);
	view_top->AddChild(view);
#endif	// TEST_VIEW_FOLLOW
}

TWindow::~TWindow()
{
}

bool TWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

TApplication::TApplication()
	: BApplication("application/x-vnd.lee-example-app")
{
}

TApplication::~TApplication()
{
}

void TApplication::ReadyToRun()
{
	TWindow *win = new TWindow(BRect(100, 100, 550, 500), "View Example: Drawing", B_TITLED_WINDOW, 0);
	win->Show();

#if TEST_FONT == 1
	int32 numFamilies = count_font_families();
	for (int32 i = 0; i < numFamilies; ++i) {
		font_family family;
		uint32		flags;
		if (get_font_family(i, &family, &flags) == B_OK) {
			dprintf(2, "Font[%d]:(%s)[%s]", i, family, flags & B_IS_FIXED ? "fixed-width" : "proportional");
			int32 numStyles = count_font_styles(family);
			for (int32 j = 0; j < numStyles; ++j) {
				font_style style;
				if (get_font_style(family, j, &style, &flags) == B_OK) {
					dprintf(2, " (%s[%s]", style, flags & B_IS_FIXED ? "fixed-width" : "proportional");
					dprintf(2, ")");
				}
			}
			dprintf(2, "\n");
		}
	}

	if (be_plain_font) be_plain_font->PrintToStream();
	if (be_bold_font) be_bold_font->PrintToStream();
	if (be_fixed_font) be_fixed_font->PrintToStream();
#endif	// TEST_FONT
}

int main(int argc, char **argv)
{
	TApplication app;
	app.Run();

	return 0;
}

#if defined(_WIN32) && !(defined(_MSC_VER) && defined(_DEBUG))
#include <windows.h>
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif	// defined(_WIN32) && !(defined(_MSC_VER) && defined(_DEBUG))
