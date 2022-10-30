#include "Roster.h"

#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <os/services/IRegistrarService.h>
#include <pimpl.h>
#include <utils/String16.h>

#include <system_error>

using android::sp;
using android::String16;

static const String16 registrarServiceName{"registrar"};

class BRoster::impl
{
   public:
	sp<os::services::IRegistrarService> service;
};

BRoster::BRoster()
{
#ifndef RUN_WITHOUT_APP_SERVER
	sp<android::IServiceManager> sm = android::defaultServiceManager();
	if (sm == nullptr) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unable to reach service manager");
	}

	sp<android::IBinder> reg = sm->getService(registrarServiceName);
	if (reg == nullptr) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unable to get registrar service");
	}

	m->service = android::interface_cast<os::services::IRegistrarService>(reg);
	if (m->service == nullptr) {
		throw std::system_error(std::error_code(ENOENT, std::system_category()), "Unknown registrar service");
	}
#else
	m->service = nullptr;
#endif
}

BRoster::~BRoster() = default;

bool BRoster::IsRunning(const char *mime_sig) const
{
	return false;
}

bool BRoster::IsRunning(entry_ref *ref) const
{
	return false;
}

team_id BRoster::TeamFor(const char *mime_sig) const
{
	return B_ERROR;
}

team_id BRoster::TeamFor(entry_ref *ref) const
{
	return B_ERROR;
}

void BRoster::GetAppList(BList *team_id_list) const
{
}

void BRoster::GetAppList(const char *sig, BList *team_id_list) const
{
}

status_t BRoster::GetAppInfo(const char *sig, app_info *info) const
{
	return B_ERROR;
}

status_t BRoster::GetAppInfo(entry_ref *ref, app_info *info) const
{
	return B_ERROR;
}

status_t BRoster::GetRunningAppInfo(team_id team, app_info *info) const
{
	return B_ERROR;
}

status_t BRoster::GetActiveAppInfo(app_info *info) const
{
	return B_ERROR;
}

status_t BRoster::FindApp(const char *mime_type, entry_ref *app) const
{
	return B_ERROR;
}

status_t BRoster::FindApp(entry_ref *ref, entry_ref *app) const
{
	return B_ERROR;
}
