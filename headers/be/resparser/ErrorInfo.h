/***************************************************************************
//
//	File:			ErrorInfo.h
//
//	Description:	BPrivate::ErrorInfo class
//
//	Copyright 2000, Be Incorporated, All Rights Reserved.
//
***************************************************************************/

/**************************************************************************
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
// This code is experimental.  Please do not use it in your own
// applications, as future versions WILL break this interface.
//
***************************************************************************/

#ifndef _ERROR_INFO_H
#define _ERROR_INFO_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif

#include <SupportDefs.h>
#include <String.h>

#include <stdarg.h>

// Note: This class needs to be made fully public, in some form.

namespace BPrivate {

	class ErrorInfo {
	public:
		ErrorInfo();
		ErrorInfo(const ErrorInfo& o);
		~ErrorInfo();
		
		ErrorInfo& operator=(const ErrorInfo& o);
		
		void Init();
		
		status_t Code() const		{ return fCode; }
		const char* Message() const	{ return fMessage.String(); }
		const char* File() const	{ return fFile.String(); }
		int32 Line() const			{ return fLine; }
		
		bool IsError() const;
		bool IsWarning() const;
		
		void SetTo(status_t code);
		void SetTo(status_t code, const char* msg, ...);
		void SetToAbs(status_t code, const char* file, int32 line,
					  const char* msg, ...);
		void SetToAbs(status_t code, const char* file, int32 line);
		
		void SetToV(status_t code, const char* msg, va_list arg);
		void SetToV(status_t code, const char* file, int32 line,
				   const char* msg, va_list arg);
		void SetToV(status_t code, const char* file, int32 line,
				   const char* msg);
		
		void HintLocation(const char* file, int32 line);
		
	private:
		status_t fCode;
		BString fMessage;
		BString fFile;
		int32 fLine;
	};
	
}

using namespace BPrivate;

#endif
