/*
 * Copyright 2001-2019, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Dörfler, axeld@pinc-software.de
 *		Ingo Weinhold, bonefish@@users.sf.net
 *		Jacob Secunda
 */

#include "AppMisc.h"

#include <Entry.h>

#include <string>

namespace BPrivate {

status_t get_app_path(team_id team, char* buffer)
{
	if (!buffer || team < 0)
		return B_BAD_VALUE;

	std::string exe("/proc/");
	if (team > 0)
		exe += team;
	else
		exe += "self";
	exe += "/exe";

	ssize_t ret = readlink(exe.c_str(), buffer, B_PATH_NAME_LENGTH);

	if (ret < 0) {
		buffer[0] = '\0';

		switch (errno) {
			case EACCES:
				return B_PERMISSION_DENIED;
			case EFAULT:
			case EINVAL:
			case ENOTDIR:
				return B_BAD_VALUE;
			case ENOENT:
				return B_ENTRY_NOT_FOUND;
			case ENOMEM:
				return B_NO_MEMORY;
			case EIO:
			case ELOOP:
			case ENAMETOOLONG:
			default:
				return B_ERROR;
		}
	}
	buffer[B_PATH_NAME_LENGTH - 1] = '\0';
	return B_OK;
}

status_t get_app_path(char* buffer)
{
	return get_app_path(0, buffer);
}

status_t get_app_ref(team_id team, entry_ref* ref, bool traverse)
{
	status_t error = (ref ? B_OK : B_BAD_VALUE);
	char	 appFilePath[B_PATH_NAME_LENGTH];

	if (error == B_OK)
		error = get_app_path(team, appFilePath);

	if (error == B_OK) {
		BEntry entry(appFilePath, traverse);
		error = entry.GetRef(ref);
	}

	return error;
}

status_t get_app_ref(entry_ref* ref, bool traverse)
{
	return get_app_ref(0, ref, traverse);
}

team_id current_team()
{
	thread_info tinfo;
	status_t	ret = get_thread_info(find_thread(nullptr), &tinfo);
	return ret == B_OK ? tinfo.team : ret;
}

thread_id main_thread_for(team_id team)
{
	// The team ID is equal to it's main thread ID. We just get
	// a team info to verify the existence of the team.
	team_info info;
	status_t  error = get_team_info(team, &info);
	return error == B_OK ? team : error;
}

}  // namespace BPrivate
