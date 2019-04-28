/******************************************************************************
/
/	File:			NetDebug.h
/
/	Description:	C++ api for networking
/
/	Copyright 1993-99, Be Incorporated
/
******************************************************************************/

#ifndef H_NETDEBUG
#define H_NETDEBUG


/*
 * Net debugging class
 *
 * Use this class to print informative messages and dump data
 * to stderr and provide control over whether debug messages
 * get printed or not.
 */

class BNetDebug
{
public:
	/*
	 * turn debugging message output on or off
	 */
    static void Enable(bool);

	/*
	 * test debugging message output state
	 */
    static bool IsEnabled();

	/*
	 * print a debugging message
	 */
    static void Print(const char *msg);

	/*
	 * dump raw data in a nice hd-like format
	 */
    static void Dump(const char *data, size_t size, const char *title);
};

#endif