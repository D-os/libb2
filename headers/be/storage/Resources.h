/***************************************************************************
//
//	File:			Resources.h
//
//	Description:	BResources class
//
//	Copyright 1992-98, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

#ifndef RESOURCES_H
#define RESOURCES_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <SupportDefs.h>
#include <File.h>



class	BResources {
public:
					BResources();				/* An empty BResources can be added to; then use WriteTo() to save. */
												/* You can load all resources from a file into memory without hanging */
												/* on to the file by doing new BResources(); MergeFrom(file); PreloadResourceType(0). */
					BResources(const BFile *file, bool truncate = false);
virtual				~BResources();

		status_t	SetTo(const BFile *file, bool truncate = false);

const	BFile &		File() const;

		const void *	LoadResource(				/* The resource file owns this pointer, it is valid until */
						type_code type,			/* this particular resource gets removed or changed. */
						int32 id,				/* AddResource(), RemoveResource(), WriteResource() */
						size_t * out_size);		/* and MergeFrom() all could accomplish this. */
		const void * LoadResource(
						type_code type,
						const char * name,
						size_t * out_size);
		status_t PreloadResourceType(			/* If you know you're going to use all your resources, you can preload them for faster access. */
						type_code type = 0);	/* Pre-loads all resources into memory if 0.*/
		status_t Sync();						/* Writes back all data to file from whence it came. */
		status_t MergeFrom(						/* Adds resources in from_file to this object. Makes copy of "from_file" for getting at the data when necessary. */
						BFile * from_file);		/* Thus, don't overwrite that file while this object is still live. */
		status_t WriteTo(						/* Like a "SetTo()" with truncate without flushing data followed by a "Sync()" */
						BFile * new_file);

		status_t	AddResource(type_code type, 
								int32 id, 
								const void *data,
								size_t data_size, 
								const char *name=NULL);

		bool		HasResource(type_code type, int32 id);

		bool		HasResource(type_code type, const char *name);

		bool		GetResourceInfo(int32 resIndex,
									type_code* typeFound,
									int32* idFound,
									const char **nameFound,
									size_t* size);

		bool		GetResourceInfo(type_code type,
									int32 resIndex,
									int32* idFound,
									const char **nameFound,
									size_t* size);

		bool		GetResourceInfo(type_code type,
									int32 id,
									const char **nameFound,
									size_t* size);

		bool		GetResourceInfo(type_code type,
									const char *name,
									int32* idFound,
									size_t* size);

		bool GetResourceInfo(					/* If you got a resource pointer from LoadResource(), */
						const void * resource,	/* you can find your way back to it using this function! */
						type_code * out_type,
						int32 * out_id,
						size_t * out_size,
						const char ** out_name);
		status_t RemoveResource(				/* Same thing here; you can use the actual pointer as an ID. */
						const void * resource);

		int			RemoveResource( type_code type,  int32 id);

	/*** DEPRECATED API STARTS HERE ***/

		status_t	WriteResource(type_code type, 	/*** DEPRECATED ***/
								  int32 id,
								  const void *data,
								  off_t offset, 
								  size_t data_size);

		status_t	ReadResource(type_code type, 	/*** DEPRECATED ***/
								 int32 id, 
								 void *data, 
								 off_t offset, 
								 size_t data_size);

		void		*FindResource(type_code type, 		/*** DEPRECATED ***/
								  int32 id, 
								  size_t *data_size);

		void		*FindResource(type_code type, 		/*** DEPRECATED ***/
								  const char *name, 
								  size_t *data_size);

private:

#if !_PR3_COMPATIBLE_
virtual	void			_ReservedResources1();
virtual	void			_ReservedResources2();
virtual	void			_ReservedResources3();
virtual	void			_ReservedResources4();
virtual	void			_ReservedResources5();
virtual	void			_ReservedResources6();
virtual	void			_ReservedResources7();
virtual	void			_ReservedResources8();
#endif

		BFile		fFile;
		struct _res_map * m_map;
		void			*unused_1;
		bool		fReadOnly;
		bool		fDirty;
		bool		m_pad_0;
		bool		m_pad_1;

#if !_PR3_COMPATIBLE_
		uint32			_reserved[3];
#endif
};

#endif
