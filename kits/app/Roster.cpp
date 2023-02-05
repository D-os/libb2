#include "Roster.h"

#define LOG_TAG "BRoster"

#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <log/log.h>
#include <os/services/IRegistrarService.h>
#include <pimpl.h>
#include <utils/String16.h>

#include <system_error>

#include "RegistrarDefs.h"

using android::sp;
using android::String16;

static const String16 registrarServiceName{"registrar"};

#pragma mark - app_info

app_info::app_info()
	: thread{-1},
	  team{-1},
	  port{-1},
	  flags{B_REG_DEFAULT_APP_FLAGS},
	  ref(),
	  signature{'\0'}
{
}

app_info::~app_info()
{
}

#pragma mark - BRoster

class BRoster::impl
{
   public:
	sp<os::services::IRegistrarService> registrar_service;
};

BRoster::BRoster()
{
#ifndef RUN_WITHOUT_REGISTRAR
	sp<android::IServiceManager> sm = android::defaultServiceManager();
	if (!sm) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unable to reach service manager");
	}

	sp<android::IBinder> reg = sm->getService(registrarServiceName);
	if (!reg) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unable to get registrar service");
	}

	m->registrar_service = android::interface_cast<os::services::IRegistrarService>(reg);
	if (!m->registrar_service) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unknown registrar service");
	}
#else
	m->registrar_service = nullptr;
#endif
}

BRoster::~BRoster() = default;

bool BRoster::IsRunning(const char* mime_sig) const
{
	return false;
}

bool BRoster::IsRunning(entry_ref* ref) const
{
	return false;
}

team_id BRoster::TeamFor(const char* mime_sig) const
{
	return B_ERROR;
}

team_id BRoster::TeamFor(entry_ref* ref) const
{
	return B_ERROR;
}

void BRoster::GetAppList(BList* team_id_list) const
{
}

void BRoster::GetAppList(const char* sig, BList* team_id_list) const
{
}

status_t BRoster::GetAppInfo(const char* sig, app_info* _info) const
{
	if (!_info) return B_BAD_VALUE;

	return B_ERROR;
}

status_t BRoster::GetAppInfo(entry_ref* ref, app_info* _info) const
{
	if (!_info) return B_BAD_VALUE;

	return B_ERROR;
}

status_t BRoster::GetRunningAppInfo(team_id team, app_info* _info) const
{
	if (!_info) return B_BAD_VALUE;

	return B_ERROR;
}

status_t BRoster::GetActiveAppInfo(app_info* _info) const
{
	if (!_info) return B_BAD_VALUE;

	return B_ERROR;
}

status_t BRoster::FindApp(const char* mime_type, entry_ref* app) const
{
	return B_ERROR;
}

status_t BRoster::FindApp(entry_ref* ref, entry_ref* app) const
{
	return B_ERROR;
}

uint32 BRoster::_AddApplication(const char* mime_sig,
								entry_ref*	ref,
								uint32		flags,
								team_id		team,
								thread_id	thread,
								port_id		port,
								bool		full_reg) const
{
	debugger(__PRETTY_FUNCTION__);
	return -1;
}
