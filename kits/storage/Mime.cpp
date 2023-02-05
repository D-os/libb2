#include "Mime.h"

#include "Errors.h"
#include "StorageDefs.h"

const char* B_PEF_APP_MIME_TYPE	 = "application/x-be-executable";
const char* B_PE_APP_MIME_TYPE	 = "application/x-vnd.Be-peexecutable";
const char* B_ELF_APP_MIME_TYPE	 = "application/x-vnd.Be-elfexecutable";
const char* B_RESOURCE_MIME_TYPE = "application/x-be-resource";
const char* B_FILE_MIME_TYPE	 = "application/octet-stream";
// Might be defined platform depended, but ELF will certainly be the common
// format for all platforms anyway.
const char* B_APP_MIME_TYPE = B_ELF_APP_MIME_TYPE;

static bool isValidMimeChar(const char ch)
{
	// Handles white space and most CTLs
	return ch > 32
		   && ch != '/'
		   && ch != '<'
		   && ch != '>'
		   && ch != '@'
		   && ch != ','
		   && ch != ';'
		   && ch != ':'
		   && ch != '"'
		   && ch != '('
		   && ch != ')'
		   && ch != '['
		   && ch != ']'
		   && ch != '?'
		   && ch != '='
		   && ch != '\\'
		   && ch != 127;  // DEL
}

#pragma mark -

BMimeType::BMimeType()
	: fType{nullptr}, fCStatus{B_NO_INIT}
{
}

BMimeType::BMimeType(const char* MIME_type)
	: BMimeType()
{
	SetTo(MIME_type);
}

BMimeType::~BMimeType()
{
	Unset();
}

status_t BMimeType::SetTo(const char* MIME_type)
{
	if (!MIME_type) {
		Unset();
	}
	else if (!IsValid(MIME_type)) {
		fCStatus = B_BAD_VALUE;
	}
	else {
		Unset();
		fType = new char[strlen(MIME_type) + 1];
		if (fType) {
			strlcpy(fType, MIME_type, B_MIME_TYPE_LENGTH);
			fCStatus = B_OK;
		}
		else {
			fCStatus = B_NO_MEMORY;
		}
	}
	return fCStatus;
}

void BMimeType::Unset()
{
	delete[] fType;
	fType	 = nullptr;
	fCStatus = B_NO_INIT;
}

status_t BMimeType::InitCheck() const
{
	return fCStatus;
}

const char* BMimeType::Type() const
{
	return fType;
}

bool BMimeType::IsValid() const
{
	return InitCheck() == B_OK && IsValid(Type());
}

bool BMimeType::IsSupertypeOnly() const
{
	if (fCStatus != B_OK)
		return false;

	// We assume here fCStatus will be B_OK *only* if the MIME string is valid
	size_t len = strlen(fType);
	for (size_t i = 0; i < len; i++) {
		if (fType[i] == '/')
			return false;
	}

	return true;
}

status_t BMimeType::GetSupertype(BMimeType* super_type) const
{
	if (!super_type)
		return B_BAD_VALUE;

	super_type->Unset();

	status_t status = fCStatus == B_OK ? B_OK : B_BAD_VALUE;
	if (status == B_OK) {
		size_t len = strlen(fType);
		size_t i   = 0;
		for (; i < len; i++) {
			if (fType[i] == '/')
				break;
		}
		if (i == len) {
			// object is a supertype only
			status = B_BAD_VALUE;
		}
		else {
			char superMime[B_MIME_TYPE_LENGTH];
			strncpy(superMime, fType, i);
			superMime[i] = 0;
			status		 = super_type->SetTo(superMime) == B_OK ? B_OK : B_BAD_VALUE;
		}
	}

	return status;
}

bool BMimeType::operator==(const BMimeType& type) const
{
	if (InitCheck() == B_NO_INIT && type.InitCheck() == B_NO_INIT)
		return true;
	else if (InitCheck() == B_OK && type.InitCheck() == B_OK)
		return strcasecmp(Type(), type.Type()) == 0;

	return false;
}

bool BMimeType::operator==(const char* type) const
{
	BMimeType mime;
	if (type)
		mime.SetTo(type);

	return (*this) == mime;
}

bool BMimeType::Contains(const BMimeType* type) const
{
	if (!type)
		return false;

	if (*this == *type)
		return true;

	BMimeType super;
	if (type->GetSupertype(&super) == B_OK && *this == super)
		return true;
	return false;
}

bool BMimeType::IsValid(const char* string)
{
	if (!string)
		return false;

	bool   foundSlash = false;
	size_t len		  = strlen(string);
	if (len >= B_MIME_TYPE_LENGTH || len == 0)
		return false;

	for (size_t i = 0; i < len; i++) {
		char ch = string[i];
		if (ch == '/') {
			if (foundSlash || i == 0 || i == len - 1)
				return false;
			else
				foundSlash = true;
		}
		else if (!isValidMimeChar(ch)) {
			return false;
		}
	}
	return true;
}
