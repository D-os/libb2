/******************************************************************************
/
/	File:			NetEndpoint.h
/
/	Description:	C++ api for networking
/
/	Copyright 1993-99, Be Incorporated
/
******************************************************************************/

#ifndef H_NETENDPOINT
#define H_NETENDPOINT

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

/*
 * BNetEndpoint
 *
 * BNetEndpoint is an object that implements a networking endpoint abstraction.
 * Think of it as an object-oriented socket. BNetEndpoint provides
 * various ways to send and receive data, establish network connections, bind
 * to local addresses, and listen for and accept new connections.
 *
 */

class BNetEndpoint : public BArchivable
{
public:

	BNetEndpoint(int proto = SOCK_STREAM);
	virtual ~BNetEndpoint();

	/*
	 * It is important to call InitCheck() after creating an instance of this class.
	 */	
	status_t InitCheck();

	BNetEndpoint &operator=(const BNetEndpoint &);
	BNetEndpoint(const BNetEndpoint &);

	/*
	 * When a BNetEndpoint is archived, it saves various state information
	 * which will allow reinstantiation later.  For example, if a BNetEndpoint
	 * that is connected to a remote FTP server is archived, when the archive
	 * is later instantiated the connection to the FTP server will be reopened 
	 * automatically.
	 */
	BNetEndpoint(BMessage *archive);
	virtual	status_t Archive(BMessage *into, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *archive);
		

	/*	
	 * BNetEndpoint::SetProtocol()
	 *
	 * allows the protocol type to be changed from stream 
	 * to datagram and vice-versa.  The current connection (if any) is closed and 
	 * the BNetEndpoint's state is reset.
	 *
	 *  Returns B_OK if successful, B_ERROR otherwise.
	 */
        
	status_t SetProtocol(int proto);

	/*
	 * These functions allow various options to be set for data communications. 
	 * The first allows any option to be set;  the latter two turn nonblocking 
	 * I/O on or off, and toggle reuseaddr respectively.  The default is for 
	 * *blocking* I/O and for reuseaddr to be OFF.
	 *
	 * Returns 0 if successful, -1 otherwise.
	 */	
	int SetOption(int32 opt, int32 lev, const void *data, unsigned int datasize);
	int SetNonBlocking(bool on = true);
	int SetReuseAddr(bool on = true);

	/*
	 * LocalAddr() returns a BNetAddress corresponding to the local address used by this 
	 * BNetEndpoint. RemoteAddr() returns a BNetAddress corresponding to the address of
	 * the remote peer we are connected to, if using a connected stream protocol.
	 */
	const BNetAddress &LocalAddr();
	const BNetAddress &RemoteAddr();
	
	/*
	 * BNetEndpoint::Socket() returns the actual socket used for data communications.
	 */
	int Socket() const;
	
	/*
	 * BNetEndpoint::Close()
	 * 
	 * Close the current BNetEndpoint, terminating any existing connection (if any) 
	 * and discarding unread data.
	 */

	virtual void Close();
	
	/*
	 * BNetEndpoint::Bind()
	 *
	 * Bind this BNetEndpoint to a local address.  Calling bind() without specifying 
	 * a BNetAddress or port number binds to a random local port.  The actual port 
	 * bound to can be determined by a subsequent call to BNetEndpoint::getAddr().
	 *
	 * Returns B_OK if successful, B_ERROR otherwise.
	 */
	virtual status_t Bind(const BNetAddress &addr);
	virtual status_t Bind(int port = 0);

	/*
	 * BNetEndpoint::Connect()
	 *
	 * Connect this BNetEndpoint to a remote address.  The first version takes 
	 * a BNetAddress already set to the remote address as an argument;  the other 
	 * one internally constructs a BNetAddress from the passed-in hostname and port 
	 * number.
	 *
	 * Returns B_OK if successful, B_ERROR otherwise.
	 */	
	virtual status_t  Connect(const BNetAddress &addr);
	virtual status_t  Connect(const char *addr, int port);
	
	/*
	 * BNetEndpoint::Listen()
	 *
	 * Specify the number of pending incoming connection attemps to queue. 
	 * This is the number of connection requests that will be allowed in between 
	 * calls to BNetEndpoint::accept().  When this number is exceeded, new 
	 * connection attempts are refused until BNetEndpoint::accept() is called. 
	 *
	 * Returns B_OK if successful, B_ERROR otherwise.
	 */
	
	virtual status_t Listen(int backlog = 5);
	
	/*
	 * BNetEndpoint::Accept()
	 *
	 * Accepts new connections to a bound stream protocol BNetEndpoint. 
	 * Blocks for timeout milleseconds (defaults to forever) waiting for 
	 * new connections.  Returns a BNetEndpoint with the same characteristics 
	 * as the one calling BNetEndpoint::Accept(), the new one representing the new 
	 * connection.  This returned BNetEndpoint is ready for communication and 
	 * must be deleted at a later time using delete.
	 */
		
	virtual BNetEndpoint *Accept(int32 timeout = -1);

	/*
	 * BNetEndpoint::Error() will return the integer error code (set by the OS)
	 * for the last send/recv error.  Likewise, BNetEndpoint::ErrorStr() returns
	 * a string describing the error.
	 */
	int Error() const;
	char *ErrorStr() const;

	/*
	 * BNetEndpoint::Send()
	 *
	 * Send data to the remote end of the connection.  If not connected, fails. 
	 * If using a datagram protocol, fails unless BNetEndpoint::Connect() 
	 * has been called, which caches the address we are sending to.  The first 
	 * version sends an arbitrary buffer of size size; the second sends the 
	 * contents of a BNetBuffer. Both return the number of bytes actually 
	 * sent, or -1 on failure.
	 */	
	virtual int32 Send(const void *buf, size_t size, int flags = 0);
	virtual int32 Send(BNetBuffer &pack, int flags = 0);

	/*
	 * BNetEndpoint::SendTo()
	 *
	 * Send data to the address specified by the passed-in BNetAddress object. 
	 * Fails if not using a datagram protocol.  The first version sends 
	 * an arbitrary buffer of size size; the second sends the contents of a 
	 * BNetBuffer.  Both return the number of bytes actually sent, 
	 * and return -1 on failure.
	 */
	
	virtual int32 SendTo(const void *buf, size_t size, const BNetAddress &to, int flags = 0);
	virtual int32 SendTo(BNetBuffer &pack, const BNetAddress &to, int flags = 0);

	/*
	 * BNetEndpoint::SetTimeout()
	 *
	 * Sets an optional timeout (in microseconds) for calls to BNetEndpoint::Receive() 
	 * and BNetEndpoint::ReceiveFrom().  Causes these calls to time out after the specified 
	 * number of microseconds when in blocking mode.
	 */
	void SetTimeout(bigtime_t usec);
	
	/*
	 * BNetEndpoint::Receive()
	 *
	 * Receive data pending on the BNetEndpoint's connection.  If in the default 
	 * blocking mode, blocks until data is available or a timeout (set by
	 * BNetEndpoint::SetTimeout). The first call receives into a preallocated buffer 
	 * of the specified size; the second call receives (at most) size bytes into 
	 * an BNetBuffer. The data is appended to the end of the BNetBuffer;  this allows 
	 * a BNetBuffer to be used in a loop to buffer incoming data read in chunks 
	 * until a certain size is read. Both return the number of bytes actually
	 * received, and return -1 on error.
	 */
	virtual int32 Receive(void *buf, size_t size, int flags = 0);
	virtual int32 Receive(BNetBuffer &pack, size_t size, int flags = 0);

	/*
	 * BNetEndpoint::ReceiveFrom()
	 *
	 * Receive datagrams from an arbitrary source and fill in the passed-in BNetAddress
	 * with the address from whence the datagram came.  As usual, both return the
	 * actual number of bytes read.  In the BNetBuffer version, the data is appended
	 * to the end of the BNetBuffer;  this allows a BNetBuffer to be used in a loop to
	 * buffer incoming data read in chunks until a certain amount is read.
	 */
	virtual int32 ReceiveFrom(void *buf, size_t size, BNetAddress &from, int flags = 0);
	virtual int32 ReceiveFrom(BNetBuffer &pack, size_t size, BNetAddress &from, int flags = 0);

	/*
	 * BNetEndpoint::IsDataPending()
	 *
	 * Returns true if there is data waiting to be received on this BNetEndpoint. 
	 * Will block for usec_timeout microseconds if specified, waiting for data.
	 */
	virtual bool IsDataPending(bigtime_t usec_timeout = 0);

	inline NLEndpoint *GetImpl() const;

protected:
	status_t m_init;
	
private:
	virtual	void		_ReservedBNetEndpointFBCCruft1();
	virtual	void		_ReservedBNetEndpointFBCCruft2();
	virtual	void		_ReservedBNetEndpointFBCCruft3();
	virtual	void		_ReservedBNetEndpointFBCCruft4();
	virtual	void		_ReservedBNetEndpointFBCCruft5();
	virtual	void		_ReservedBNetEndpointFBCCruft6();

	int32				__ReservedBNetEndpointFBCCruftData[8];
	NLEndpoint *m_impl;
	BNetAddress m_peer;
	BNetAddress m_addr;
	BNetAddress m_conaddr;
};


inline NLEndpoint *BNetEndpoint::GetImpl() const
{
	return m_impl;
}

#endif