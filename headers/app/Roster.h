#ifndef _ROSTER_H
#define _ROSTER_H

#include <Entry.h>
#include <OS.h>

/// app_info Struct and Values
struct app_info
{
	app_info();
	~app_info();

	thread_id thread;
	team_id	  team;
	port_id	  port;
	uint32	  flags;
	entry_ref ref;
	char	  signature[B_MIME_TYPE_LENGTH];
};

#define B_LAUNCH_MASK (0x3)

#define B_SINGLE_LAUNCH (0x0)
#define B_MULTIPLE_LAUNCH (0x1)
#define B_EXCLUSIVE_LAUNCH (0x2)

#define B_BACKGROUND_APP (0x4)
#define B_ARGV_ONLY (0x8)
#define _B_APP_INFO_RESERVED1_ (0x10000000)

enum {
	B_REQUEST_LAUNCHED		   = 0x00000001,
	B_REQUEST_QUIT			   = 0x00000002,
	B_REQUEST_APP_ACTIVATED	   = 0x00000004,
	B_REQUEST_WINDOW_ACTIVATED = 0x00000008,

	// Synonym for backwards compatibility
	B_REQUEST_ACTIVATED = B_REQUEST_APP_ACTIVATED
};

enum {
	B_SOME_APP_LAUNCHED		= 'BRAS',
	B_SOME_APP_QUIT			= 'BRAQ',
	B_SOME_APP_ACTIVATED	= 'BRAW',
	B_SOME_WINDOW_ACTIVATED = 'BRAZ'
};

class BRoster
{
   public:
	BRoster();
	~BRoster();
};

/// Global be_roster
extern const BRoster *be_roster;

#endif /* _ROSTER_H */
