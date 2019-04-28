/***************************************************************************
//
//	File:			ResourceParser.h
//
//	Description:	High-level interface to parsing and writing resources.
//
//	Copyright 1999, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

/**************************************************************************
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
// This code is experimental.  Please do not use it in your own
// applications, as future versions WILL break this interface.
//
***************************************************************************/

#ifndef _RESOURCE_PARSER_H
#define _RESOURCE_PARSER_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif

#include <Path.h>
#include <List.h>
#include <Locker.h>
#include <String.h>

#include <ResourceItem.h>
#include <ErrorInfo.h>

namespace BPrivate {
	class ResourceParserState;
	class ResourceParserContext;
}

using namespace BPrivate;

class BResourceParser;

class BResourceContext
{
public:
	BResourceContext();
	BResourceContext(const BResourceContext& o);
	~BResourceContext();
	
	BResourceContext& operator=(const BResourceContext& o);

private:
	friend class BResourceParser;
	
	// For internal use.
	BResourceContext(const ResourceParserContext& o);
	BResourceContext& operator=(const ResourceParserContext& o);
	
	ResourceParserContext* fContext;
};

class BResourceParser
{
public:
	BResourceParser();
	virtual ~BResourceParser();
	
	status_t Init();
	status_t SetTo(const char* file, bool include_from_here=true);
	status_t InitCheck() const;
	
	status_t GetContext(BResourceContext* context) const;
	status_t SetContext(const BResourceContext* context);
	
	const char* Path() const;
	
	status_t PreInclude(const char* dir);
	status_t PostInclude(const char* dir);
	
	status_t Define(const char* name);
	status_t Define(const char* name, const char* val);
	status_t Define(const char* name, int val);
	
	status_t Run();

	// Call-backs when running.
	virtual status_t ReadItem(BResourceItem* item);
	virtual void Error(const ErrorInfo& info);
	virtual void Warn(const ErrorInfo& info);
	
	// Methods for writing a resource definition file
	// from raw data.
	status_t StartWriting(BDataIO* resource, BDataIO* header = 0,
						  bool buffered = true, bool owns_inputs = false);
	status_t WriteHeader(const char* fileName, const char* headerName);
	status_t StartWritingHeader(const char* file);
	status_t WriteItem(const BResourceItem* item);
	status_t StopWriting();
	
	const char* LevelIndent(int32 level) const;
	/* Note: doesn't add error information if fails. */
	status_t WriteValue(BDataIO* to, type_code type,
						const void* data, size_t size,
						int32 level) const;
						
	static bool LooksLikeString(const void* data, size_t size, bool* isArray=0);
	
	static const char* TypeToString(type_code type, BString* out,
									bool fullContext = true,
									bool strict = false);
	static const char* TypeIDToString(type_code type, int32 id, BString* out);
	static type_code StringToType(const char* str);

	const char* TypeToString(type_code type,
							 bool fullContext = true,
							 bool strict = false);
	const char* TypeIDToString(type_code type, int32 id);
	
	void AddError(const ErrorInfo& info);
	size_t CountErrors() const;
	const ErrorInfo& ErrorAt(size_t idx=0) const;
	
	status_t AddItem(BResourceItem* item);
	const BResourceItem* ItemAt(int32 idx) const;
	
	BPath FindResourceFile(const char* name) const;

protected:
	virtual BResourceItem* NewItem(type_code type = 0, int32 id = 0,
									const char* name = 0) const;
	
private:
	friend class BPrivate::ResourceParserState;
	
	void FreeData();
	
	/* Note: none of these add error information if they fail. */
	status_t WriteCommentBlock(BDataIO* to, const char* fileName) const;
	static const char* EscapeForString(const char* str, BString* out);
	const char* EscapeForString(const char* str);
	
	status_t WriteMessageValue(BDataIO* to, type_code type,
							   const BMessage* msg, int32 level) const;
	status_t WriteStringValue(BDataIO* to, type_code type,
							  const char* data, size_t size,
							  int32 level, bool force_array = false) const;
	status_t WriteStringData(BDataIO* to,
							 const char* data, size_t size,
							 int32 level, bool as_array) const;
	status_t WriteHexValue(BDataIO* to, type_code type,
						   const void* data, size_t size,
						   int32 level,
						   int32 hint_line_length = 0) const;
	status_t WriteHexData(BDataIO* to,
						  const void* data, size_t size,
						  int32 level, int32 line_length=32) const;
	
	static inline char makehexdigit(uint32 val)
	{
		return "0123456789ABCDEF"[val&0xF];
	}

	BPrivate::ResourceParserState* fParser;
	
	BPath fFile;
	int32 fErrorCount;
	ErrorInfo fError;
	ErrorInfo fNoError;
	
	BList fItems;
	
	BString fTypeBuffer;
	
	// Context for writing items.
	BDataIO* fWriteDest;
	BDataIO* fWriteHeader;
	bool fOwnWriteIO;
	bool fFoundSymbols;
	BString fEscapeBuffer;
};

#endif
