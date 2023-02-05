/*
 * Copyright 2002-2009, Ingo Weinhold, bonefish@users.sf.net.
 * Distributed under the terms of the MIT License.
 */
#ifndef _APP_MISC_H
#define _APP_MISC_H

#include <OS.h>
#include <SupportDefs.h>

struct entry_ref;

namespace BPrivate {

/*!	\brief Returns the path to an application's executable.
	\param team The application's team ID.
	\param buffer A pointer to a pre-allocated character array of at least
		   size B_PATH_NAME_LENGTH to be filled in by this function.
	\return
	- \c B_OK: Everything went fine.
	- \c B_BAD_VALUE: \c NULL \a buffer.
	- another error code
*/
status_t get_app_path(team_id team, char *buffer);

/*!	\brief Returns the path to the application's executable.
	\param buffer A pointer to a pre-allocated character array of at least
		   size B_PATH_NAME_LENGTH to be filled in by this function.
	\return
	- \c B_OK: Everything went fine.
	- \c B_BAD_VALUE: \c NULL \a buffer.
	- another error code
*/
status_t get_app_path(char *buffer);

/*!	\brief Returns an entry_ref referring to an application's executable.
	\param team The application's team ID.
	\param ref A pointer to a pre-allocated entry_ref to be initialized
		   to an entry_ref referring to the application's executable.
	\param traverse If \c true, the function traverses symbolic links.
	\return
	- \c B_OK: Everything went fine.
	- \c B_BAD_VALUE: \c NULL \a ref.
	- another error code
*/
status_t get_app_ref(team_id team, entry_ref *ref, bool traverse = true);

/*!	\brief Returns an entry_ref referring to the application's executable.
	\param ref A pointer to a pre-allocated entry_ref to be initialized
		   to an entry_ref referring to the application's executable.
	\param traverse If \c true, the function traverses symbolic links.
	\return
	- \c B_OK: Everything went fine.
	- \c B_BAD_VALUE: \c NULL \a ref.
	- another error code
*/
status_t get_app_ref(entry_ref *ref, bool traverse = true);

/*!	\brief Returns the ID of the current team.
	\return The ID of the current team.
*/
team_id current_team();

/*!	Returns the ID of the supplied team's main thread.
	\param team The team.
	\return
	- The thread ID of the supplied team's main thread
	- \c B_BAD_TEAM_ID: The supplied team ID does not identify a running team.
	- another error code
*/
thread_id main_thread_for(team_id team);

}  // namespace BPrivate

#endif	// _APP_MISC_H
