#include <Be.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

class MyApplication : public BApplication
{
   public:
	MyApplication(const char *signature, status_t *error) : BApplication(signature, error) {}

	void ReadyToRun() override
	{
		BList *teams = new BList();
		be_roster->GetAppList(teams);
		std::cout << "=== Apps: " << teams->CountItems() << std::endl;

		Quit();
	}

	void MessageReceived(BMessage *message) override
	{
		switch (message->what) {
			case 'TEST':
				std::cout << "=== TEST message received" << std::endl;
				break;
			default:
				BApplication::MessageReceived(message);
		}
	}
};

int main()
{
    setbuf(stdout, NULL); // do not buffer

    fprintf(stderr, "=== new BApplication\n");
	status_t app_status;
	new MyApplication("application/x-vnd.test-app", &app_status);
	fprintf(stderr, "=== MyApplication status: %d\n", app_status);

	fprintf(stderr, "=== SendMessage\n");
	auto ret = be_app_messenger.SendMessage('TEST');
	fprintf(stderr, "=== SendMessage status: %d\n", ret);

	fprintf(stderr, "=== Run()\n");
	be_app->Run();

	fprintf(stderr, "=== snoozing\n");
	snooze(200000);

	fprintf(stderr, "=== exiting\n");
	delete be_app;

	return EXIT_SUCCESS;
}
