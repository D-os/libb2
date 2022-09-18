#ifndef _LOOPER_H
#define _LOOPER_H

#include <Handler.h>
#include <OS.h>

/// Port (Message Queue) Capacity
#define B_LOOPER_PORT_DEFAULT_CAPACITY 100

class BLooper : public BHandler
{
   public:
	BLooper(const char *name = NULL, int32 priority = B_NORMAL_PRIORITY,
			int32 port_capacity = B_LOOPER_PORT_DEFAULT_CAPACITY);
	virtual ~BLooper();
};

#endif /* _LOOPER_H */
