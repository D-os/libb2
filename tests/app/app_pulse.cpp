#include <Be.h>

class MyApplication : public BApplication
{
   public:
	MyApplication(const char *sig) : BApplication(sig) {}

   private:
	void Pulse() override
	{
		std::cout << "Pulse" << std::endl;
	}
};

int main()
{
	new MyApplication("application/x-vnd.test-app");

	be_app->SetPulseRate(1000000);

	be_app->Run();
}
