/******************************************************************************
/
/	File:			NetPacket.h
/
/	Description:	Pure virtual BNetPacket class represents network packets.
/
/					BStandardPacket is an implementation of BNetPacket. 
/
/					BTimeoutHandler is a mix-in class that provides a 
/					convenient protocol for handling timeouts.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _NET_PACKET_H
#define _NET_PACKET_H

#include <BeBuild.h>
#include <SupportDefs.h>

/*----------- BNetPacket Class -------------------------------*/

class BNetPacket {

public:
	virtual unsigned Size(void) = 0;
	virtual void SetSize(unsigned size) = 0;
	virtual unsigned Base(void) = 0;		
	virtual void SetBase(int offset) = 0;	
	virtual char *DataBlock(unsigned offset, unsigned *size) = 0;
	virtual ~BNetPacket(void) = 0;

	virtual void Read(unsigned offset, char *data, unsigned size);
	virtual void Write(unsigned offset, const char *data, unsigned size);

/* Obsolete... */
	char *Data(void)
	{
		char *data;
		unsigned size;
		unsigned retsize;

		size = Size();
		retsize = size;
		data = DataBlock(0, &retsize);
		if (retsize != size) {return (NULL);}
		return (data);
	}
/* ...down to here */

protected:
	BNetPacket(void);

};

/*-------- BStandardPacket Class ----------------------------------*/

class BStandardPacket : public BNetPacket {
public:
	BStandardPacket(unsigned size = 0);
	~BStandardPacket(void);
	unsigned Size(void);
	void SetSize(unsigned size);
	unsigned Base(void);
	void SetBase(int offset);
	char *DataBlock(unsigned offset, unsigned *size);
	
	// These dont do anything but call ::new and ::delete.  They're
	// here to ensure binary compaitibility with older add-ons.
	void *operator new(size_t size);
	void operator delete(void *ptr);

private:
	char *store;
	unsigned size;
	unsigned maxsize;
	unsigned offset;
};

/*---------- copy_packet() Function ----------------------------*/

_IMPEXP_NETDEV void copy_packet(BNetPacket *srcpacket, unsigned srcoffset,
		   				BNetPacket *dstpacket, unsigned dstoffset, 
		   				unsigned size);

/*--------- BTimeoutHandler Class and Timeout Functions ---------*/

class BTimeoutHandler {
public:
	virtual void TimedOut(uint32 receipt) = 0;
};

_IMPEXP_NETDEV bool cancel_timeout(uint32 receipt);
_IMPEXP_NETDEV uint32 set_timeout(BTimeoutHandler *handler, bigtime_t howlong);

/*-------------------------------------------------------------*/

#endif /* _NET_PACKET_H */
