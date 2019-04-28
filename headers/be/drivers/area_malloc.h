/*******************************************************************************
/
/	File:			area_malloc.h
/
/	Description:	Kernel area_malloc module API
/
/	Copyright 1999, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _AREA_MALLOC_H_
#define _AREA_MALLOC_H_

#include <module.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	area_malloc is a wrapper around the general purpose internal malloc()
	routines that arranges for memory to come from areas rather than the
	heap.  malloc(), calloc(), realloc() and free() behave like their
	user-space counterparts, except they require a pool to work from as
	their first argument.

	Note that these routines are *NOT* safe to call inside an interrupt
	handler.  They may block on semaphores.
	
	SMP Safety
		malloc(), calloc(), realloc() and free() are SMP-safe with respect
		to one another, but not in regards to delete_pool().  In other words,
		don't call delete_pool() until you know the other routines are not in
		use.  create_pool() and delete_pool() are safe with respect to each
		other.

	When the last user of the module puts it away, any remaining pools are
	deleted.

	const void * create_pool(uint32 address_spec, size_t size, uint32 lock_spec, uint32 protection)
		Creates a new pool of memory to allocate from.  The parameters are
		the same as used for create_area(), so that you have complete
		control over the characteristics (if not the name) of the area used
		as the memory pool.  Returns an opaque pool identifier, or NULL if
		the creation failed.  The sharability of the resources allocated
		from this pool are determined by the permisions and protections used
		to create the area.  In other words, the rules for sharing areas
		apply.  It is possible to find out the area_id of the area from
		which the memory is allocated, but I'm not going tell you how.

	status_t delete_pool(const void *pool_id);
		Deletes the specified pool, returning all related resources to the
		kernel.  All pointers returned from malloc(), calloc() or realloc()
		when this pool was specified are immediately invalid.  Returns B_OK
		if the pool was deleted, or B_ERROR if an invalid pool_id was
		passed.


	These routines behave exactly like their libroot.so counterparts, except
	they operate on a particular pool.

	void * malloc(const void *pool_id, size_t size)
		Allocates size bytes from the specified pool, or returns NULL if
		there is no more available memory.

	void * calloc(const void *pool_id, size_t nmemb, size_t size)
		Allocates nmemb * size bytes from the pool and initializes the
		memory to 0.  Returns a pointer to the memory, or NULL if the
		allocation failed.

	void * realloc(const void *pool_id, void *ptr, size_t size)
		Changes the size of the memory block pointed to by ptr to size
		bytes.  Returns a pointer to the memory, which may be NULL
		indicating failure, the same as ptr indicating the memory block
		was resized in place, or different indicating that the contents
		of the memory block were copied to a new location of the
		appropriate size.

	void free(const void *pool_id, void *ptr)
		Releases the memory pointed at by ptr back to the pool.

*/

#define B_AREA_MALLOC_MODULE_NAME "generic/area_malloc/v1"

typedef struct {
	module_info		minfo;
	const void *	(*create_pool)(uint32 address_spec, size_t size, uint32 lock_spec, uint32 protection);
	status_t		(*delete_pool)(const void *pool_id);
	void *			(*malloc)(const void *pool_id, size_t size);
	void *			(*calloc)(const void *pool_id, size_t nmemb, size_t size);
	void *			(*realloc)(const void *pool_id, void *ptr, size_t size);
	void			(*free)(const void *pool_id, void *ptr);
} area_malloc_module_info;

#ifdef __cplusplus
}
#endif

#endif

