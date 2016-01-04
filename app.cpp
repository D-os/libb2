#include <Message.h>

#include <stdio.h>
#include <stdlib.h>

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

    return 0;
}
