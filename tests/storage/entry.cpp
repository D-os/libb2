#include <StorageKit.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    BEntry entry("/usr/share/doc");
    status_t err;
    char name[B_FILE_NAME_LENGTH];

    BPath path;
    entry.GetPath(&path);
    printf(">> %s\n", path.Path());

    /* Spit out the path components backwards, one at a time. */
    do {
        entry.GetName(name);
        printf("> %s\n", name);
    } while ((err = entry.GetParent(&entry)) == B_OK && strlen(name));

    /* Complain for reasons other than reaching the top. */
    if (err != B_ENTRY_NOT_FOUND)
        printf(">> Error: %s\n", strerror(err));

    return EXIT_SUCCESS;
}
