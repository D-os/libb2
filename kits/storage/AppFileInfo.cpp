/*
 * Based on Haiku implementation.
 * Authors:
 *              Ingo Weinhold, ingo_weinhold@gmx.de
 */

#include "AppFileInfo.h"

#include <File.h>

// type codes
enum {
	B_APP_FLAGS_TYPE	= 'APPF',
	B_VERSION_INFO_TYPE = 'APPV',
};

// attributes
// static const char* kTypeAttribute			= "BEOS:TYPE";
static const char* kSignatureAttribute = "BEOS:APP_SIG";
static const char* kAppFlagsAttribute  = "BEOS:APP_FLAGS";
// static const char* kSupportedTypesAttribute = "BEOS:FILE_TYPES";
// static const char* kVersionInfoAttribute	= "BEOS:APP_VERSION";
// static const char* kMiniIconAttribute		= "BEOS:M:";
// static const char* kLargeIconAttribute		= "BEOS:L:";
// static const char* kIconAttribute			= "BEOS:";
// static const char* kStandardIconType		= "STD_ICON";
// static const char* kIconType				= "ICON";
// static const char* kCatalogEntryAttribute	= "SYS:NAME";

// resource IDs
// static const int32 kTypeResourceID			   = 2;
static const int32 kSignatureResourceID = 1;
static const int32 kAppFlagsResourceID	= 1;
// static const int32 kSupportedTypesResourceID   = 1;
// static const int32 kMiniIconResourceID		   = 101;
// static const int32 kLargeIconResourceID		   = 101;
// static const int32 kIconResourceID			   = 101;
// static const int32 kVersionInfoResourceID	   = 1;
// static const int32 kMiniIconForTypeResourceID  = 0;
// static const int32 kLargeIconForTypeResourceID = 0;
// static const int32 kIconForTypeResourceID	   = 0;
// static const int32 kCatalogEntryResourceID	   = 1;

BAppFileInfo::BAppFileInfo()
	: BNodeInfo(),
	  fWhere(B_USE_BOTH_LOCATIONS)
{
}

BAppFileInfo::BAppFileInfo(BFile *file)
	: BAppFileInfo()
{
	SetTo(file);
}

BAppFileInfo::~BAppFileInfo()
{
}

status_t BAppFileInfo::SetTo(BFile *file)
{
	// unset the old file
	BNodeInfo::SetTo(nullptr);
	// if (fResources) {
	// 	delete fResources;
	// 	fResources = nullptr;
	// }

	// check param
	status_t error = file && file->InitCheck() == B_OK ? B_OK : B_BAD_VALUE;

	info_location where = B_USE_BOTH_LOCATIONS;

	// // create resources
	// if (error == B_OK) {
	// 	fResources = new (std::nothrow) BResources();
	// 	if (fResources) {
	// 		error = fResources->SetTo(file);
	// 		if (error != B_OK) {
	// 			// no resources - this is no critical error, we'll just use
	// 			// attributes only, then
	// 			where = B_USE_ATTRIBUTES;
	// 			error = B_OK;
	// 		}
	// 	}
	// 	else
	// 		error = B_NO_MEMORY;
	// }

	// set node info
	if (error == B_OK)
		error = BNodeInfo::SetTo(file);

	// if (error != B_OK || (where & B_USE_RESOURCES) == 0) {
	// 	delete fResources;
	// 	fResources = nullptr;
	// }

	// clean up on error
	if (error != B_OK) {
		if (InitCheck() == B_OK)
			BNodeInfo::SetTo(nullptr);
	}

	// set data location
	if (error == B_OK)
		SetInfoLocation(where);

	// set error
	fCStatus = error;
	return error;
}

status_t BAppFileInfo::GetType(char *type) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::GetSignature(char* signature) const
{
	// check param and initialization
	status_t error = (signature ? B_OK : B_BAD_VALUE);
	if (error == B_OK && InitCheck() != B_OK)
		error = B_NO_INIT;
	// read the data
	size_t read = 0;
	if (error == B_OK) {
		error = _ReadData(kSignatureAttribute, kSignatureResourceID,
						  B_MIME_STRING_TYPE, signature, B_MIME_TYPE_LENGTH, read);
	}
	// check the read data -- null terminate the string
	if (error == B_OK && signature[read - 1] != '\0') {
		if (read == B_MIME_TYPE_LENGTH)
			error = B_ERROR;
		else
			signature[read] = '\0';
	}
	return error;
}

status_t BAppFileInfo::GetAppFlags(uint32 *flags) const
{
	// check param and initialization
	status_t error = flags != NULL ? B_OK : B_BAD_VALUE;
	if (error == B_OK && InitCheck() != B_OK)
		error = B_NO_INIT;
	// read the data
	size_t read = 0;
	if (error == B_OK) {
		error = _ReadData(kAppFlagsAttribute, kAppFlagsResourceID,
						  B_APP_FLAGS_TYPE, flags, sizeof(uint32), read);
	}
	// check the read data
	if (error == B_OK && read != sizeof(uint32))
		error = B_ERROR;
	return error;
}

status_t BAppFileInfo::GetIcon(BBitmap *icon, icon_size which) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::SetType(const char *type)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::SetIcon(const BBitmap *icon, icon_size which)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

void BAppFileInfo::SetInfoLocation(info_location location)
{
	// // if the resources failed to initialize, we must not use them
	// if (fResources == NULL)
	// 	location = info_location(location & ~B_USE_RESOURCES);

	fWhere = location;
}

bool BAppFileInfo::IsUsingAttributes() const
{
	return (fWhere & B_USE_ATTRIBUTES) != 0;
}

bool BAppFileInfo::IsUsingResources() const
{
	return (fWhere & B_USE_RESOURCES) != 0;
}

/*!     Reads data from an attribute or resource.

		\note The data is read from the location specified by \a fWhere.

		\warning The object must be properly initialized. The parameters are
				\b NOT checked.

		\param name The name of the attribute/resource to be read.
		\param id The resource ID of the resource to be read. It is ignored
				   when < 0.
		\param type The type of the attribute/resource to be read.
		\param buffer A pre-allocated buffer for the data to be read.
		\param bufferSize The size of the supplied buffer.
		\param bytesRead A reference parameter, set to the number of bytes
				   actually read.
		\param allocatedBuffer If not \c NULL, the method allocates a buffer
				   large enough too store the whole data and writes a pointer to it
				   into this variable. If \c NULL, the supplied buffer is used.

		\returns A status code.
		\retval B_OK Everything went fine.
		\retval B_ENTRY_NOT_FOUND The entry was not found.
		\retval B_NO_MEMORY Ran out of memory allocating the buffer.
		\retval B_BAD_VALUE \a type did not match.
*/
status_t BAppFileInfo::_ReadData(const char* name, int32 id, type_code type,
								 void* buffer, size_t bufferSize, size_t& bytesRead, void** allocatedBuffer)
	const
{
	static auto NOT_IMPLEMENTED = "not implemented";
	bytesRead					= std::min(strlen(NOT_IMPLEMENTED) + 1, bufferSize);
	memcpy(buffer, NOT_IMPLEMENTED, bytesRead);
	return B_OK;
}
