#include <Message.h>
#include <Archivable.h>
#include <String.h>
#include <Handler.h>

#include <stdio.h>
#include <stdlib.h>

class Test : public BArchivable {
public:
    BString string;
    int32 number;
    Test() : number(0) {}
    Test(BMessage* archive);
    static BArchivable* Instantiate(BMessage *archive);
    status_t Archive(BMessage* archive,bool deep = true) const;
};

status_t Test::Archive(BMessage* archive, bool deep) const
{
    BArchivable::Archive(archive, deep);
    status_t ret = archive->AddString("test:string", string);
    if (ret != B_OK) return ret;
    return archive->AddInt32("test:number", number);
}

Test::Test(BMessage *archive)
{
    BString string;
    if (archive->FindString("test:string", &string) == B_OK)
        this->string = string;
    int32 number;
    if (archive->FindInt32("test:number", &number) == B_OK)
        this->number = number;
}

BArchivable* Test::Instantiate(BMessage* archive)
{
    if (validate_instantiation(archive, "TheClass"))
        return new Test(archive);
    return NULL;
}

int main(int argc, char **argv) {
    BMessage msg('MSG_');

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
    Test test;
    test.string = "FooBar";
    test.number = 42;
    BMessage archive;
    test.Archive(&archive);
    archive.PrintToStream();

    Test utest(&archive);
    printf("Test(%s, %d)\n", utest.string.String(), utest.number);

//    Test *uitest = dynamic_cast<Test *>(instantiate_object(&archive));
//    if (uitest) {
//        printf("Test(%s, %d)\n", uitest->string.String(), uitest->number);
//    }

    // --- handler
    BHandler handler;

    return EXIT_SUCCESS;
}
