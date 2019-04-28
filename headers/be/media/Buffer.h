/*******************************************************************************
/
/	File:			Buffer.h
/
/   Description:   A BBuffer is a container of media data in the Media Kit
/
/	Copyright 1997-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#if !defined(_BUFFER_H)
#define _BUFFER_H

#include <MediaDefs.h>


/*** A BBuffer is not the same thing as the area segment that gets cloned ***/
/*** For each buffer that gets created, a BBuffer object is created to represent ***/
/*** it in each participant address space. ***/


struct _shared_buffer_list;

struct buffer_clone_info {
	buffer_clone_info();
	~buffer_clone_info();
	media_buffer_id	buffer;
	area_id		area;
	size_t		offset;
	size_t		size;
	int32		flags;
private:
	uint32 _reserved_[4];
};


class BBuffer
{
public:

		void * Data();	/* returns NULL if buffer not correctly initialized */
		size_t SizeAvailable();	//	total size of buffer (how much data can it hold)
		size_t SizeUsed();		//	how much was written (how much data does it hold)
		void SetSizeUsed(
				size_t size_used);
		uint32 Flags();

		void Recycle();
		buffer_clone_info CloneInfo() const;

		media_buffer_id ID();	/* 0 if not registered */
		media_type Type();
		media_header * Header();
		media_audio_header * AudioHeader();
		media_video_header * VideoHeader();

		enum {	/* for flags */
			B_F1_BUFFER = 0x1,
			B_F2_BUFFER = 0x2,
			B_SMALL_BUFFER = 0x80000000
		};

		size_t Size();			//	deprecated; use SizeAvailable()

private:

		friend class 			_BMediaRosterP;
		friend class 			BMediaRoster;
		friend class 			BBufferProducer;
		friend class 			BBufferConsumer;	/* for buffer receiving */
		friend class 			BBufferGroup;
		friend class			BSmallBuffer;

		BBuffer(area_id area, size_t offset, size_t size, int32 flags = 0);
		BBuffer(media_header * _mHeader);	//	for "small buffer" placement new
		~BBuffer();	/* BBuffer is NOT a virtual class!!! */

		BBuffer();
		BBuffer(const BBuffer & clone);
		BBuffer & operator=(const BBuffer & clone);

		void					SetOwnerArea(area_id owner);
		void					SetHeader(media_header *header);

		media_header			_mHeader;
		area_id 				_mArea;
		area_id 				_mOrigArea;
		area_id 				_mListArea;
		area_id 				_mOrigListArea;
		_shared_buffer_list *	_mList;
		
		void * 			_mData;
		size_t 			_mOffset;
		size_t 			_mSize;
		media_buffer_id _mBufferID;
		int32 			_mFlags;
		int32			_mRefCount;
		int32			_m_listOffset;
		uint32 			_reserved_buffer_[6];

explicit	BBuffer(
				const buffer_clone_info & info);

		void			SetGroupOwnerPort(
								port_id port);
		void			SetCurrentOwner(
								port_id port);
};


class BSmallBuffer : public BBuffer
{
public:
							BSmallBuffer();

static	size_t				SmallBufferSizeLimit();

};

#endif /* _BUFFER_H */

