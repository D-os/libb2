/*******************************************************************************
/
/	File:			BufferGroup.h
/
/   Description:   A BBufferGroup organizes sets of BBuffers so that you can request
/	and reclaim them.
/
/	Copyright 1997-98, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#if !defined(_BUFFER_GROUP_H)
#define _BUFFER_GROUP_H

#include <MediaDefs.h>

struct _shared_buffer_list;
class _buffer_id_cache;

class BBufferGroup
{
public:

		BBufferGroup(
				size_t size,
				int32 count = 3,
				uint32 placement = B_ANY_ADDRESS,
				uint32 lock = B_FULL_LOCK);
explicit	BBufferGroup();
		BBufferGroup(
				int32 count,
				const media_buffer_id * buffers);
		~BBufferGroup();	/* BBufferGroup is NOT a virtual class!!! */

		status_t InitCheck();

			/* use this function to add buffers you created on your own */
		status_t AddBuffer(
				const buffer_clone_info & info,
				BBuffer ** out_buffer = NULL);

		BBuffer * RequestBuffer(
				size_t size,
				bigtime_t timeout = B_INFINITE_TIMEOUT);
		status_t RequestBuffer(
				BBuffer * buffer,
				bigtime_t timeout = B_INFINITE_TIMEOUT);
		status_t RequestError();	/* return last RequestBuffer error, useful if NULL is returned */

		status_t CountBuffers(
				int32 * out_count);
		status_t GetBufferList(
				int32 buf_count,
				BBuffer ** out_buffers);

		status_t WaitForBuffers();
		status_t ReclaimAllBuffers();

private:

static	status_t _entry_reclaim(void *);

		friend class _BMediaRosterP;
		friend class BMediaRoster;
		friend class BBufferProducer;
		friend class BBufferConsumer;	//	for SetOwnerPort()

		BBufferGroup(const BBufferGroup &);	/* not implemented */
		BBufferGroup& operator=(const BBufferGroup&); /* not implemented */

		status_t 				IBufferGroup();
		status_t 				AddToList(BBuffer *buffer);
		status_t 				AddBuffersTo(BMessage * message, const char * name, bool needLock=true);
		status_t 				_RequestBuffer(	size_t size, media_buffer_id wantID,
												BBuffer **buffer, bigtime_t timeout);

		void					SetOwnerPort(
									port_id owner);

		bool					CanReclaim();
		void 					WillReclaim();

		status_t				Lock();
		status_t				Unlock();

		status_t 				_m_init_error;
		uint32 					_mFlags;
		int32					_mBufferCount;
		area_id 				_mBufferListArea;
		_shared_buffer_list *	_mBufferList;
		_buffer_id_cache * 		_mBufferCache;
		status_t				_m_local_err;
		uint32 					_reserved_buffer_group_[7];
};

#endif /* _BUFFER_GROUP_H */

