/*
 * Copyright 2001-2011, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ingo Weinhold (bonefish@users.sf.net)
 */

//!	Global library initialization/termination routines.


#include <stdio.h>
#include <stdlib.h>

#include <AppMisc.h>
#include <LooperList.h>
#include <MessagePrivate.h>
//#include <RosterPrivate.h>
#include <TokenSpace.h>

//extern void __initialize_locale_kit();


// debugging
#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif
#define OUT	printf


static void
initialize_forked_child()
{
    DBG(OUT("initialize_forked_child()\n"));

    BMessage::Private::StaticReInitForkedChild();
    BPrivate::gLooperList.InitAfterFork();
    BPrivate::gDefaultTokens.InitAfterFork();
    BPrivate::init_team_after_fork();

    DBG(OUT("initialize_forked_child() done\n"));
}


extern "C" void
initialize_before()
{
    DBG(OUT("initialize_before()\n"));

    BMessage::Private::StaticInit();
//    BRoster::Private::InitBeRoster();

    atfork(initialize_forked_child);

    DBG(OUT("initialize_before() done\n"));
}
__attribute__((section(".init_array"))) void (* p_lib_be_init)(void) = &initialize_before;


extern "C" void
initialize_after()
{
    DBG(OUT("initialize_after()\n"));

//    __initialize_locale_kit();

    DBG(OUT("initialize_after() done\n"));
}


extern "C" void
terminate_after()
{
    DBG(OUT("terminate_after()\n"));

//    BRoster::Private::DeleteBeRoster();
    BMessage::Private::StaticCleanup();
    BMessage::Private::StaticCacheCleanup();

    DBG(OUT("terminate_after() done\n"));
}

