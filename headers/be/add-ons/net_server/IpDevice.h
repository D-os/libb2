/******************************************************************************
/
/	File:			IpDevice.h
/
/	Description:	Pure virtual BIpDevice and BIpHandler classes define the
/					IP packet translation protocol for network add-ons.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _IP_DEVICE_H
#define _IP_DEVICE_H

#include <BeBuild.h>
#include <add-ons/net_server/NetProtocol.h>

class BIpHandler;

/*----------- BIpDevice Class -------------------------------------*/

class BIpDevice {
public:
	virtual status_t SendPacket(uint32 dst, BNetPacket *buf) = 0;
	virtual unsigned Flags(void) = 0;
	virtual unsigned MaxPacketSize(void) = 0;
	virtual BNetPacket *AllocPacket(void) = 0;
	virtual void Run(BIpHandler *ip) = 0;
	virtual void Close(void) = 0;
	virtual void Statistics(FILE *f) = 0;
	virtual ~BIpDevice(void);
};

/*----------- BIpHandler Class -------------------------------------*/

class BIpHandler {
public:
	virtual void PacketReceived(BNetPacket *buf, BIpDevice *dev) = 0;
	virtual uint32 Address(void) = 0;
	virtual void SetAddress(uint32 address) = 0;
	virtual uint32 NetMask(void) = 0;
};

/*-------------------------------------------------------------*/

#endif /* _IP_DEVICE_H */


