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

#include <signal.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <log/log.h>
#include <log/logger.h>
#include <log/log_read.h>

#include "benchmark.h"

// enhanced version of LOG_FAILURE_RETRY to add support for EAGAIN and
// non-syscall libs. Since we are benchmarking, or using this in the emergency
// signal to stuff a terminating code, we do NOT want to introduce
// a syscall or usleep on EAGAIN retry.
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

/*
 *	Measure the fastest rate we can reliabley stuff print messages into
 * the log at high pressure. Expect this to be less than double the process
 * wakeup time (2ms?)
 */
static void BM_log_maximum_retry(int iters) {
    StartBenchmarkTiming();

    for (int i = 0; i < iters; ++i) {
        LOG_FAILURE_RETRY(
            __android_log_print(ANDROID_LOG_INFO,
                                "BM_log_maximum_retry", "%d", i));
    }

    StopBenchmarkTiming();
}
BENCHMARK(BM_log_maximum_retry);

/*
 *	Measure the fastest rate we can stuff print messages into the log
 * at high pressure. Expect this to be less than double the process wakeup
 * time (2ms?)
 */
static void BM_log_maximum(int iters) {
    StartBenchmarkTiming();

    for (int i = 0; i < iters; ++i) {
        __android_log_print(ANDROID_LOG_INFO, "BM_log_maximum", "%d", i);
    }

    StopBenchmarkTiming();
}
BENCHMARK(BM_log_maximum);

/*
 *	Measure the time it takes to submit the android logging call using
 * discrete acquisition under light load. Expect this to be a pair of
 * syscall periods (2us).
 */
static void BM_clock_overhead(int iters) {
    for (int i = 0; i < iters; ++i) {
       StartBenchmarkTiming();
       StopBenchmarkTiming();
    }
}
BENCHMARK(BM_clock_overhead);

/*
 *	Measure the time it takes to submit the android logging call using
 * discrete acquisition under light load. Expect this to be a dozen or so
 * syscall periods (40us).
 */
static void BM_log_overhead(int iters) {
    for (int i = 0; i < iters; ++i) {
       StartBenchmarkTiming();
       __android_log_print(ANDROID_LOG_INFO, "BM_log_overhead", "%d", i);
       StopBenchmarkTiming();
       usleep(1000);
    }
}
BENCHMARK(BM_log_overhead);

/*
 *	Measure the time it takes for __android_log_is_loggable.
 */
static void BM_is_loggable(int iters) {
    StartBenchmarkTiming();

    for (int i = 0; i < iters; ++i) {
        __android_log_is_loggable(ANDROID_LOG_WARN, "logd", ANDROID_LOG_VERBOSE);
    }

    StopBenchmarkTiming();
}
BENCHMARK(BM_is_loggable);
