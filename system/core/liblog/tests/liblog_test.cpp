/*
 * Copyright (C) 2013-2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <string.h>

#include <gtest/gtest.h>
#include <log/log.h>
#include <log/logger.h>
#include <log/log_read.h>

// enhanced version of LOG_FAILURE_RETRY to add support for EAGAIN and
// non-syscall libs. Since we are only using this in the emergency of
// a signal to stuff a terminating code into the logs, we will spin rather
// than try a usleep.
#define LOG_FAILURE_RETRY(exp) ({  \
    typeof (exp) _rc;              \
    do {                           \
        _rc = (exp);               \
    } while (((_rc == -1)          \
           && ((errno == EINTR)    \
            || (errno == EAGAIN))) \
          || (_rc == -EINTR)       \
          || (_rc == -EAGAIN));    \
    _rc; })

TEST(liblog, __android_log_buf_print) {
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "radio"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_SYSTEM, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "system"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_print(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_print",
                                         "main"));
    usleep(1000);
}

TEST(liblog, __android_log_buf_write) {
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "radio"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_SYSTEM, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "system"));
    usleep(1000);
    EXPECT_LT(0, __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                         "TEST__android_log_buf_write",
                                         "main"));
    usleep(1000);
}

static void* ConcurrentPrintFn(void *arg) {
    int ret = __android_log_buf_print(LOG_ID_MAIN, ANDROID_LOG_INFO,
                                  "TEST__android_log_print", "Concurrent %" PRIuPTR,
                                  reinterpret_cast<uintptr_t>(arg));
    return reinterpret_cast<void*>(ret);
}

#define NUM_CONCURRENT 64
#define _concurrent_name(a,n) a##__concurrent##n
#define concurrent_name(a,n) _concurrent_name(a,n)

TEST(liblog, concurrent_name(__android_log_buf_print, NUM_CONCURRENT)) {
    pthread_t t[NUM_CONCURRENT];
    int i;
    for (i=0; i < NUM_CONCURRENT; i++) {
        ASSERT_EQ(0, pthread_create(&t[i], NULL,
                                    ConcurrentPrintFn,
                                    reinterpret_cast<void *>(i)));
    }
    int ret = 0;
    for (i=0; i < NUM_CONCURRENT; i++) {
        void* result;
        ASSERT_EQ(0, pthread_join(t[i], &result));
        int this_result = reinterpret_cast<uintptr_t>(result);
        if ((0 == ret) && (0 != this_result)) {
            ret = this_result;
        }
    }
    ASSERT_LT(0, ret);
}

log_time signal_time;

static const char max_payload_tag[] = "TEST_max_payload_XXXX";
static const char max_payload_buf[LOGGER_PAYLOAD_MAX_LEN - 1] = "LEONATO\n\
I learn in this letter that Don Peter of Arragon\n\
comes this night to Messina\n\
MESSENGER\n\
He is very near by this: he was not three leagues off\n\
when I left him\n\
LEONATO\n\
How many gentlemen have you lost in this action?\n\
MESSENGER\n\
But few of any sort, and none of name\n\
LEONATO\n\
A victory is twice itself when the achiever brings\n\
home full numbers. I find here that Don Peter hath\n\
bestowed much honour on a young Florentine called Claudio\n\
MESSENGER\n\
Much deserved on his part and equally remembered by\n\
Don Pedro: he hath borne himself beyond the\n\
promise of his age, doing, in the figure of a lamb,\n\
the feats of a lion: he hath indeed better\n\
bettered expectation than you must expect of me to\n\
tell you how\n\
LEONATO\n\
He hath an uncle here in Messina will be very much\n\
glad of it.\n\
MESSENGER\n\
I have already delivered him letters, and there\n\
appears much joy in him; even so much that joy could\n\
not show itself modest enough without a badge of\n\
bitterness.";

TEST(liblog, max_payload) {
    pid_t pid = getpid();
    char tag[sizeof(max_payload_tag)];
    memcpy(tag, max_payload_tag, sizeof(tag));
    snprintf(tag + sizeof(tag) - 5, 5, "%04X", pid & 0xFFFF);

    LOG_FAILURE_RETRY(__android_log_buf_write(LOG_ID_SYSTEM, ANDROID_LOG_INFO,
                                              tag, max_payload_buf));
    sleep(2);

    struct logger_list *logger_list;

    ASSERT_TRUE(NULL != (logger_list = android_logger_list_open(
        LOG_ID_SYSTEM, ANDROID_LOG_RDONLY, 100, 0)));

    bool matches = false;
    ssize_t max_len = 0;

    for(;;) {
        log_msg log_msg;
        if (android_logger_list_read(logger_list, &log_msg) <= 0) {
            break;
        }

        char *data = log_msg.msg();

        if (strncmp(data, tag, sizeof(tag) - 1)) {
            continue;
        }

        data += sizeof(tag) + 1;

        const char *left = data;
        const char *right = max_payload_buf;
        while (*left && *right && (*left == *right)) {
            ++left;
            ++right;
        }

        if (max_len <= (left - data)) {
            max_len = left - data + 1;
        }

        if (max_len > 512) {
            matches = true;
        }
        break;
    }

    android_logger_list_close(logger_list);

    EXPECT_EQ(true, matches);

    EXPECT_LE(strlen(max_payload_buf), static_cast<size_t>(max_len));
}

TEST(liblog, too_big_payload) {
    pid_t pid = getpid();
    static const char big_payload_tag[] = "TEST_big_payload_XXXX";
    char tag[sizeof(big_payload_tag)];
    memcpy(tag, big_payload_tag, sizeof(tag));
    snprintf(tag + sizeof(tag) - 5, 5, "%04X", pid & 0xFFFF);

    std::string longString(3266519, 'x');

    ssize_t ret = LOG_FAILURE_RETRY(__android_log_buf_write(LOG_ID_SYSTEM,
                                    ANDROID_LOG_INFO, tag, longString.c_str()));

    struct logger_list *logger_list;

    ASSERT_TRUE(NULL != (logger_list = android_logger_list_open(
        LOG_ID_SYSTEM, ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, 100, 0)));

    ssize_t max_len = 0;

    for(;;) {
        log_msg log_msg;
        if (android_logger_list_read(logger_list, &log_msg) <= 0) {
            break;
        }

        char *data = log_msg.msg();

        if (strncmp(data, tag, sizeof(tag) - 1)) {
            continue;
        }

        data += sizeof(tag) + 1;

        const char *left = data;
        const char *right = longString.c_str();
        while (*left && *right && (*left == *right)) {
            ++left;
            ++right;
        }

        if (max_len <= (left - data)) {
            max_len = left - data + 1;
        }
    }

    android_logger_list_close(logger_list);

    EXPECT_LE(LOGGER_PAYLOAD_MAX_LEN, static_cast<int>(max_len));

    EXPECT_GT(ret, max_len);
}

TEST(liblog, dual_reader) {
    struct logger_list *logger_list1;

    // >25 messages due to liblog.__android_log_buf_print__concurrentXX above.
    ASSERT_TRUE(NULL != (logger_list1 = android_logger_list_open(
        LOG_ID_MAIN, ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, 25, 0)));

    struct logger_list *logger_list2;

    if (NULL == (logger_list2 = android_logger_list_open(
            LOG_ID_MAIN, ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, 15, 0))) {
        android_logger_list_close(logger_list1);
        ASSERT_TRUE(NULL != logger_list2);
    }

    int count1 = 0;
    bool done1 = false;
    int count2 = 0;
    bool done2 = false;

    do {
        log_msg log_msg;

        if (!done1) {
            if (android_logger_list_read(logger_list1, &log_msg) <= 0) {
                done1 = true;
            } else {
                ++count1;
            }
        }

        if (!done2) {
            if (android_logger_list_read(logger_list2, &log_msg) <= 0) {
                done2 = true;
            } else {
                ++count2;
            }
        }
    } while ((!done1) || (!done2));

    android_logger_list_close(logger_list1);
    android_logger_list_close(logger_list2);

    EXPECT_EQ(25, count1);
    EXPECT_EQ(15, count2);
}
