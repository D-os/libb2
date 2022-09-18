#include <Application.h>

#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    fprintf(stderr, "=== new BApplication\n");
    new BApplication("application/x-vnd.test-app");

    // fprintf(stderr, "=== SendMessage\n");
    // be_app_messenger.SendMessage('TEST', (BHandler*)NULL);

    fprintf(stderr, "=== Run()\n");
    be_app->Run();

    // fprintf(stderr, "=== snoozing\n");
    // snooze(1000000);

    fprintf(stderr, "=== exiting\n");
    delete be_app;

    return EXIT_SUCCESS;
}
