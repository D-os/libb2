/******************************************************************************
/
/	File:			NetworkKit.h
/
/	Description:	C++ api for networking
/
/	Copyright 1993-99, Be Incorporated
/
******************************************************************************/

#ifndef H_NETAPI
#define H_NETAPI

#include <BeBuild.h>
#include <SupportDefs.h>
#include <Archivable.h>
#include <socket.h>


/*
 * Nettle forward declarations
 *
 * The Nettle networking object library is compiled into the netapi kit
 * and used by these kit classes.  We use forward declarations here to
 * eliminate Nettle header dependencies from people who use the net kit,
 * since Nettle uses things like exceptions which we do not allow to
 * propagate beyond the library boundary.
 *
 */

class NLEndpoint;
class NLAddress;
class NLPacket;

/*
 * Net API classes
 *
 * These classes wrap the Nettle library in a similar manner to the
 * Bridge or Proxy design patterns.
 *
 */

#include <NetAddress.h>
#include <NetBuffer.h>
#include <NetEndpoint.h>
#include <NetDebug.h>

#endif