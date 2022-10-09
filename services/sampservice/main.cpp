/*
 * Copyright 2018 The Android Open Source Project
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

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <signal.h>
#include <sys/resource.h>

#include "SampService.h"

using namespace android;
using namespace os::services::sampservice;

int main(int /* argc */, char** /* argv */)
{
	signal(SIGPIPE, SIG_IGN);

	// publish SampService
	sp<SampService>		sampservice = sp<SampService>::make();
	sp<IServiceManager> sm(defaultServiceManager());
	sm->addService(String16(SampService::SERVICE_NAME), sampservice, false);

	// limit the number of binder threads to 4.
	ProcessState::self()->setThreadPoolMaxThreadCount(4);

	// start the thread pool
	sp<ProcessState> ps(ProcessState::self());
	ps->startThreadPool();
	ps->giveThreadPoolName();
	IPCThreadState::self()->joinThreadPool();

	return 0;
}
