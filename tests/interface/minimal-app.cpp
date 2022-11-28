/// https://hoelz.ro/blog/programming-for-haiku-hello-world

#include <Application.h>
#include <Button.h>
#include <Window.h>
#include <stdio.h>

const int32 HELLO_HAIKU = 'HELO';

class HelloWindow : public BWindow
{
   public:
	HelloWindow(BRect frame) : BWindow(frame, "Hello Window", B_TITLED_WINDOW, 0)
	{
		BView *view = new BView(Bounds(), NULL, B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
		AddChild(view);
		BButton *button = new BButton(view->Bounds(), NULL, "Hello", new BMessage(HELLO_HAIKU));
		view->AddChild(button);
	}

	bool QuitRequested()
	{
		be_app_messenger.SendMessage(B_QUIT_REQUESTED);
		return BWindow::QuitRequested();
	}

	void MessageReceived(BMessage *msg)
	{
		switch (msg->what) {
			case HELLO_HAIKU:
				printf("Hello, Haiku!\n");
				break;
			default:
				BWindow::MessageReceived(msg);
		}
	}
};

class HelloHaiku : public BApplication
{
   public:
	HelloHaiku() : BApplication("application/hello-haiku")
	{
	}

	void ReadyToRun()
	{
		BWindow *win = new HelloWindow(BRect(100, 100, 300, 200));
		win->Show();
	}
};

int main(void)
{
	HelloHaiku app;

	app.Run();

	return 0;
}
