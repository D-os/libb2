#include <Message.h>
#include <Archivable.h>
#include <String.h>
#include <Handler.h>
#include <Looper.h>

#include <stdio.h>
#include <stdlib.h>

#include <Application.h>
#include <Messenger.h>
//BApplication *be_app = NULL;
//BMessenger be_app_messenger;

class TestArchivable : public BArchivable {
public:
    BString string;
    int32 number;
    TestArchivable() : number(0) {}
    TestArchivable(BMessage* archive);
    static BArchivable* Instantiate(BMessage *archive);
    status_t Archive(BMessage* archive, bool deep = true) const;
};

status_t TestArchivable::Archive(BMessage* archive, bool deep) const
{
    BArchivable::Archive(archive, deep);
    status_t ret = archive->AddString("test:string", string);
    if (ret != B_OK) return ret;
    return archive->AddInt32("test:number", number);
}

TestArchivable::TestArchivable(BMessage *archive)
{
    BString string;
    if (archive->FindString("test:string", &string) == B_OK)
        this->string = string;
    int32 number;
    if (archive->FindInt32("test:number", &number) == B_OK)
        this->number = number;
}

BArchivable* TestArchivable::Instantiate(BMessage* archive)
{
    if (validate_instantiation(archive, "TheClass"))
        return new TestArchivable(archive);
    return NULL;
}

class TestLooper : public BLooper {
public:
    void MessageReceived(BMessage *message);
};

void TestLooper::MessageReceived(BMessage *message) {
    printf("TestLooper::MessageReceived %.4s\n", (char*)&message->what);
    BLooper::MessageReceived(message);
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    BMessage msg('MSG_');
    printf("BMessage->what: %.4s\n", (char*)&msg.what);

    printf("IsSystem: %d\n", msg.IsSystem());

    msg.AddBool("bool", false);

    BMessage copy(msg);
    copy.what = 'CPY1';

    BMessage copy2;
    copy2 = msg;
    copy2.what = 'CPY2';

    size_t size = msg.FlattenedSize();
    printf("Flattened size: %zu\n", size);
    char *buffer = (char*)malloc(size);
    if (msg.Flatten(buffer, size) != B_OK) {
        printf("Flattening failed\n");
        return EXIT_FAILURE;
    }

    BMessage unf;
    unf.Unflatten(buffer);
    unf.what = 'UNFL';

    copy.ReplaceBool("bool", true);
    copy2.AddInt8("zero", 0);

    msg.PrintToStream();
    copy.PrintToStream();
    copy2.PrintToStream();
    unf.PrintToStream();

    // --- archiving
    TestArchivable test;
    test.string = "FooBar";
    test.number = 42;
    BMessage archive;
    test.Archive(&archive);
    archive.PrintToStream();

    TestArchivable utest(&archive);
    printf("Test(%s, %d)\n", utest.string.String(), utest.number);

//    Test *uitest = dynamic_cast<Test *>(instantiate_object(&archive));
//    if (uitest) {
//        printf("Test(%s, %d)\n", uitest->string.String(), uitest->number);
//    }

    // --- handler
    BHandler handler;
    handler.SetName(test.string);
    printf("Handler: %s\n", handler.Name());

    // --- looper
    BLooper *looper = new TestLooper();
    thread_id tid = looper->Run();
    printf("Looper %p thread_id: %d\n", looper, tid);

    printf("PostMessage: %d\n", looper->PostMessage(&msg));

    // --- application
//    new BApplication("application/x-vnd.test-app");
//    be_app->Run();
//    delete be_app;

//    snooze(1000000);
//    looper->Lock();
//    looper->Quit();

    return EXIT_SUCCESS;
}
