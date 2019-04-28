/******************************************************************************
/
/	File:			NetDevice.h
/
/	Description:	Pure virtual BNetDevice class defines the fundamental 
/					protocol for network add-ons.
/
/					Pure virtual BNetConfig class defines the protocol for 
/ 					configuring the device. 
/
/					BCallbackHandler is part of the BNetConfig implementation.
/
/	Copyright 1995-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _NET_DEVICE_H
#define _NET_DEVICE_H

#include <BeBuild.h>
#include <add-ons/net_server/NetPacket.h>
#include <net_settings.h>
#include <stdio.h>

class BIpDevice;

/*-------- stuctures etc. -----------------------------*/

typedef enum {
 	B_NULL_NET_DEVICE = 0x00,  
	B_ETHER_NET_DEVICE = 0x06,
 	B_PPP_NET_DEVICE = 0x17,
 	B_LOOP_NET_DEVICE = 0x18
} net_device_type;
 
enum {
 	B_FLAGS_POINT_TO_POINT = 0x01,
 	B_FLAGS_LINK_DOWN      = 0x02
};


/*-------- BCallbackHandler Class -----------------------------*/

class BCallbackHandler {
public:
	virtual void Done(status_t status) = 0;
};

/*---------- BNetConfig Class ----------------------------------*/

class BNetConfig {
public:
	virtual bool IsIpDevice(void) = 0;

	virtual status_t Config(const char *ifname,
							net_settings *ncw,
							BCallbackHandler *callback,
							bool autoconfig = false) = 0;

	virtual int GetPrettyName(char *name, int len) = 0;

	virtual ~BNetConfig(void);
};

/*--------- BNetDevice Class -------------------------------------*/

class BNetDevice {
public:
	virtual BNetPacket *ReceivePacket(void) = 0;
	virtual BNetPacket *AllocPacket(void) = 0;
	virtual void SendPacket(BNetPacket *packet) = 0;
	virtual void Address(char *address) = 0;
	
	virtual status_t AddMulticastAddress(const char *address) = 0;
	virtual status_t RemoveMulticastAddress(const char *address) = 0;
	virtual status_t SetPromiscuous(bool yes) = 0;
	virtual unsigned MaxPacketSize(void) = 0;
	virtual net_device_type Type(void) = 0;

	virtual void Close(void) = 0;

	virtual BIpDevice *OpenIP(void) = 0;
	virtual void Statistics(FILE *f) = 0;

	virtual ~BNetDevice(void);
};

/*------------- Global Functions -----------------------------*/

_IMPEXP_NETDEV void deliver_packet(BNetPacket *buf, BNetDevice *dev);

extern "C" _EXPORT BNetDevice *open_device(const char *device);
extern "C" _EXPORT BNetConfig *open_config(const char *device);

/* network interface card info */

typedef struct netcard_info {
	net_device_type netcard_type;      /* card type, e.g. B_ETHER_NET_DEVICE */
	char  			netcard_addr[255]; /* type-specific format; for example,
                                        * for ethernet, the first 6 bytes will
                                        * be the card HW address.  See the BeBook
                                        * entry for BNetDevice::Address()
                                        */
} netcard_info_t;

_IMPEXP_NETDEV status_t get_nth_netcard_info(int16 n, netcard_info_t *ni);

/*-------------------------------------------------------------*/

#endif /* _NET_DEVICE_H */


