#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SYSLOG_NAMES
#include <cutils/log.h>
#include <cutils/klog.h>

int main(int argc, char* const argv[])
{
    klog_set_level(KLOG_DEBUG_LEVEL); // pass all messages to klog

    bool wantsUsage = false;
    bool useKLog = true;
    int result = 0;
    int i;
    char *name, *tag = NULL;

    int facility = LOG_USER;
    int priority = LOG_INFO;

    while (1) {
        int ic = getopt(argc, argv, "h?f:p:");
        if (ic < 0)
            break;

        switch (ic) {
        case 'h':
        case '?':
            wantsUsage = true;
            break;
        case 'f':
            for (i = 0; (name = facilitynames[i].c_name); i++) {
                if (strcmp(name, optarg) == 0) {
                    facility = facilitynames[i].c_val;
                    useKLog = false;
                    break;
                }
            }
            if (name == NULL) {
                fprintf(stderr, "writelog: Unknown facility '%s'\n", optarg);
            }
            break;
        case 'p':
            for (i = 0; (name = prioritynames[i].c_name); i++) {
                if (strcmp(name, optarg) == 0) {
                    priority = prioritynames[i].c_val;
                    break;
                }
            }
            if (name == NULL) {
                fprintf(stderr, "writelog: Unknown priority '%s'\n", optarg);
            }
            break;
        default:
            fprintf(stderr, "writelog: Unknown option -%c\n", ic);
            wantsUsage = true;
            result = 10;
            break;
        }
    }

    if (optind >= argc) {
        wantsUsage = true;
    } else if (!wantsUsage) {
        if (argc - optind > 1) {
            tag = argv[optind++];
            useKLog = false;
        }

        char *text;
        asprintf(&text, "%s", argv[optind++]);
        while (optind < argc) {
            char *concat;
            asprintf(&concat, "%s %s", text, argv[optind++]);
            free(text);
            text = concat;
        }

        if (useKLog) {
            klog_write(KLOG_WARNING_LEVEL, "%s\n", text);
        } else {
            __log_write(facility, priority, tag, text);
        }
        free(text);
    }

    if (wantsUsage) {
        fprintf(stderr, "%s", "Usage: writelog [-h|-?] [-f facility] [-l level] [tag] message...\n");
        return result;
    }

    return result;
}

