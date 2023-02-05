#include <Be.h>

#include "Application.h"

class MyApp : public BApplication
{
   public:
	MyApp(const char *signature) : BApplication(signature){};

	void DispatchMessage(BMessage *message, BHandler *target) override;
};

void MyApp::DispatchMessage(BMessage *message, BHandler *target)
{
	std::cout << "=== Got message " << message->what << " for " << (target ? target->Name() : "-default handler") << std::endl;
	message->PrintToStream();

	BApplication::DispatchMessage(message, target);
}

int main()
{
	new MyApp("application/x-vnd.test-app");

	BMessage message('TEST');
	message.AddString("test", "test message");

	BMessageRunner runner(be_app, &message, 1000000);

	be_app->Run();
}
