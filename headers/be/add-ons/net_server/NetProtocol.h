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

#ifndef _NET_PROTOCOL_H
#define _NET_PROTOCOL_H

#include <BeBuild.h>
#include <add-ons/net_server/NetDevice.h>

/*---------- BPacketHandler Class ---------------------------*/

class BPacketHandler {
public:
	virtual bool PacketReceived(BNetPacket *buf, BNetDevice *dev) = 0;
};

/*----------- BNetProtocol Class ----------------------------*/

class BNetProtocol {
public:
	virtual void AddDevice(BNetDevice *dev, const char *name) = 0;
	virtual ~BNetProtocol(void);
};

/*------------- Global Functions ----------------------------*/

_IMPEXP_NETDEV void register_packet_handler(BPacketHandler *handler, 
								BNetDevice *dev, int priority = 0);

_IMPEXP_NETDEV void unregister_packet_handler(BPacketHandler *handler, 
								BNetDevice *dev);

extern "C" _EXPORT BNetProtocol *open_protocol(const char *device);

/*-------------------------------------------------------------*/

#endif /* _NET_PROTOCOL_H */


