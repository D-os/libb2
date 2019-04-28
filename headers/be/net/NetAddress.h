/******************************************************************************
/
/	File:			NetAddress.h
/
/	Description:	C++ api for networking
/
/	Copyright 1993-99, Be Incorporated
/
******************************************************************************/

#ifndef H_NETADDRESS
#define H_NETADDRESS

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

class NLAddress;

/*
 * Net API classes
 *
 * These classes wrap the Nettle library in a similar manner to the
 * Bridge or Proxy design patterns.
 *
 */


/*
 * BNetAddress
 *
 * This class is used to represent network addresses, and provide access
 * to a network address in a variety of formats. BNetAddress provides various ways
 * to get and set a network address, converting to or from the chosen representation
 * into a generic internal one.
 */

class BNetAddress : public BArchivable
{
public:

	BNetAddress(BMessage *archive);
	virtual ~BNetAddress();
	
	virtual	status_t Archive(BMessage *into, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *archive);
	 
	BNetAddress(const char *hostname = 0, unsigned short port = 0);
	BNetAddress(const struct sockaddr_in &sa);
	BNetAddress(in_addr addr, int port = 0);
	BNetAddress(uint32 addr, int port = 0 );
	BNetAddress(const BNetAddress &);
	BNetAddress(const char *hostname, const char *protocol, const char *service);
	
	BNetAddress &operator=(const BNetAddress &);

	/*
	 * It is important to call InitCheck() after creating an instance of this class.
	 */	
	status_t InitCheck();
	
	/*
	 * BNetAddress::set() sets the internal address representation to refer
	 * to the internet address specified by the passed-in data.
	 * Returns true if successful, false otherwise.
	 */
	status_t SetTo(const char *hostname, const char *protocol, const char *service);
	status_t SetTo(const char *hostname = 0, unsigned short port = 0);
	status_t SetTo(const struct sockaddr_in &sa);
	status_t SetTo(in_addr addr, int port = 0);
	status_t SetTo(uint32 addr=INADDR_ANY, int port = 0);

	/*
	 * BNetAddress::get() converts the internal internet address representation
	 * into the specified form and returns it by filling in the passed-in parameters.
	 * Returns true if successful, false otherwise.
	 */
	status_t GetAddr(char *hostname = 0, unsigned short *port = 0) const;
	status_t GetAddr(struct sockaddr_in &sa) const;
	status_t GetAddr(in_addr &addr, unsigned short *port = 0) const;

	inline NLAddress *GetImpl() const;

protected:
	status_t m_init;
	
private:
	virtual	void		_ReservedBNetAddressFBCCruft1();
	virtual	void		_ReservedBNetAddressFBCCruft2();
	virtual	void		_ReservedBNetAddressFBCCruft3();
	virtual	void		_ReservedBNetAddressFBCCruft4();
	virtual	void		_ReservedBNetAddressFBCCruft5();
	virtual	void		_ReservedBNetAddressFBCCruft6();

	int32				__ReservedBNetAddressFBCCruftData[8];
	NLAddress *m_impl;

};




inline NLAddress *BNetAddress::GetImpl() const
{
	return m_impl;
}


#endif