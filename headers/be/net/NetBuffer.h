/******************************************************************************
/
/	File:			NetBuffer.h
/
/	Description:	C++ api for networking
/
/	Copyright 1993-99, Be Incorporated
/
******************************************************************************/

#ifndef H_NETBUFFER
#define H_NETBUFFER

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

class NLPacket;

/*
 * Net API classes
 *
 * These classes wrap the Nettle library in a similar manner to the
 * Bridge or Proxy design patterns.
 *
 */


/*
 * BNetBuffer
 *
 * BNetBuffer is a dynamic buffer useful for storing data to be sent
 * across the network. Data is inserted into and removed from 
 * the object using one of the many insert and remove member functions.
 * Access to the raw stored data is possible. The BNetEndpoint class has a
 * send and recv function for use with BNetBuffer. Network byte order conversion
 * is done automatically for all appropriate integral types in the insert and remove
 * functions for that type.
 *
 */

class BNetBuffer : public BArchivable
{
public:

	BNetBuffer(size_t size = 0);
	virtual ~BNetBuffer();

	/*
	 * It is important to call InitCheck() after creating an instance of this class.
	 */	

	status_t InitCheck();
	
	BNetBuffer(BMessage *archive);
	virtual	status_t Archive(BMessage *into, bool deep = true) const;
	static BArchivable *Instantiate(BMessage *archive);

	BNetBuffer(const BNetBuffer &);
	BNetBuffer &operator=(const BNetBuffer &);
	

	/*
	 *  Data insertion member functions.  Data is inserted at the end of the internal 
	 *  dynamic buffer.  For the short, int, and long versions (and unsigned of 
	 *  each as well) byte order conversion is performed.
	 *
	 *  The terminating null of all strings are inserted as 
	 *  well.
	 *
	 *  Returns B_OK if successful, B_ERROR otherwise.
	 */
	status_t AppendInt8(int8);
	status_t AppendUint8(uint8);
	status_t AppendInt16(int16);
	status_t AppendUint16(uint16);
	status_t AppendInt32(int32);
	status_t AppendUint32(uint32);
	status_t AppendFloat(float);
	status_t AppendDouble(double);
	status_t AppendString(const char *);
	status_t AppendData(const void *, size_t);
	status_t AppendMessage(const BMessage &);
	status_t AppendInt64(int64);
	status_t AppendUint64(uint64);

	/*
	 *  Data extraction member functions.  Data is extracted from the start of the 
	 *  internal dynamic buffer.  For the short, int, and long versions (and 
	 *  unsigned of each as well) byte order conversion is performed.
	 *
	 *  Returns B_OK if successful, B_ERROR otherwise.
	 */
 	status_t RemoveInt8(int8 &);
	status_t RemoveUint8(uint8 &);
	status_t RemoveInt16(int16 &);
	status_t RemoveUint16(uint16 &);
	status_t RemoveInt32(int32 &);
	status_t RemoveUint32(uint32 &);
	status_t RemoveFloat(float &);
	status_t RemoveDouble(double &);
	status_t RemoveString(char *, size_t);
	status_t RemoveData(void *, size_t);
	status_t RemoveMessage(BMessage &);
	status_t RemoveInt64(int64 &);
	status_t RemoveUint64(uint64 &);
 
	/*
	 * GetData() returns a pointer to the internal data buffer.  This is useful 
	 * when wanting to pass the data to a function expecting a pointer to data. 
	 * GetSize() returns the size of the BNetPacket's data, in bytes. 
	 * GetBytesRemaining() returns the bytes remaining in the BNetPacket available 
	 * to be extracted via BNetPacket::Remove*().
	 */
	unsigned char *Data() const;
	size_t Size() const;
	size_t BytesRemaining() const;
	
	inline NLPacket *GetImpl() const;

protected:
	status_t m_init;

private:
	virtual	void		_ReservedBNetBufferFBCCruft1();
	virtual	void		_ReservedBNetBufferFBCCruft2();
	virtual	void		_ReservedBNetBufferFBCCruft3();
	virtual	void		_ReservedBNetBufferFBCCruft4();
	virtual	void		_ReservedBNetBufferFBCCruft5();
	virtual	void		_ReservedBNetBufferFBCCruft6();

	NLPacket *m_impl;
	int32				__ReservedBNetBufferFBCCruftData[8];

};


inline NLPacket *BNetBuffer::GetImpl() const
{
	return m_impl;
}


#endif