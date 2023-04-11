/*
 * Copyright 2005-2014 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Stefano Ceccherini, burton666@libero.it
 */

#include "DataIO.h"

#include <stdio.h>

BDataIO::BDataIO()
{
}

BDataIO::~BDataIO()
{
}

ssize_t BDataIO::Read(void* buffer, size_t size)
{
	return B_NOT_SUPPORTED;
}

ssize_t BDataIO::Write(const void* buffer, size_t size)
{
	return B_NOT_SUPPORTED;
}

#pragma mark -

BPositionIO::BPositionIO()
{
}

BPositionIO::~BPositionIO()
{
}

ssize_t BPositionIO::Read(void* buffer, size_t size)
{
	off_t	curPos = Position();
	ssize_t result = ReadAt(curPos, buffer, size);
	if (result > 0)
		Seek(result, SEEK_CUR);

	return result;
}

ssize_t BPositionIO::Write(const void* buffer, size_t size)
{
	off_t	curPos = Position();
	ssize_t result = WriteAt(curPos, buffer, size);
	if (result > 0)
		Seek(result, SEEK_CUR);

	return result;
}

status_t BPositionIO::SetSize(off_t size)
{
	return B_ERROR;
}

#pragma mark -

BMemoryIO::BMemoryIO(void* buffer, size_t length)
	: fReadOnly{false},
	  fBuffer{static_cast<char*>(buffer)},
	  fLength{length},
	  fBufferSize{length},
	  fPosition{0}
{
}

BMemoryIO::BMemoryIO(const void* buffer, size_t length)
	: fReadOnly{true},
	  fBuffer{const_cast<char*>(static_cast<const char*>(buffer))},
	  fLength{length},
	  fBufferSize{length},
	  fPosition{0}
{
}

BMemoryIO::~BMemoryIO()
{
}

ssize_t BMemoryIO::ReadAt(off_t pos, void* buffer, size_t size)
{
	if (!buffer || pos < 0)
		return B_BAD_VALUE;

	ssize_t sizeRead = 0;
	if (pos < (off_t)fLength) {
		sizeRead = min_c((off_t)size, (off_t)fLength - pos);
		memcpy(buffer, fBuffer + pos, sizeRead);
	}

	return sizeRead;
}

ssize_t BMemoryIO::WriteAt(off_t pos, const void* buffer, size_t size)
{
	if (fReadOnly)
		return B_NOT_ALLOWED;

	if (!buffer || pos < 0)
		return B_BAD_VALUE;

	ssize_t sizeWritten = 0;
	if (pos < (off_t)fBufferSize) {
		sizeWritten = min_c((off_t)size, (off_t)fBufferSize - pos);
		memcpy(fBuffer + pos, buffer, sizeWritten);
	}

	if (pos + sizeWritten > (off_t)fLength)
		fLength = pos + sizeWritten;

	return sizeWritten;
}

off_t BMemoryIO::Seek(off_t position, uint32 seek_mode)
{
	switch (seek_mode) {
		case SEEK_SET:
			fPosition = position;
			break;
		case SEEK_CUR:
			fPosition += position;
			break;
		case SEEK_END:
			fPosition = fLength + position;
			break;
		default:
			break;
	}

	return fPosition;
}

off_t BMemoryIO::Position() const
{
	return fPosition;
}

status_t BMemoryIO::SetSize(off_t size)
{
	if (fReadOnly)
		return B_NOT_ALLOWED;

	if (size > (off_t)fBufferSize)
		return B_ERROR;

	fLength = size;

	return B_OK;
}

#pragma mark -

BMallocIO::BMallocIO()
	: fBlockSize{256},
	  fMallocSize{0},
	  fLength{0},
	  fData{nullptr},
	  fPosition{0}
{
}

BMallocIO::~BMallocIO()
{
	free(fData);
}

ssize_t BMallocIO::ReadAt(off_t pos, void* buffer, size_t size)
{
	if (!buffer)
		return B_BAD_VALUE;

	ssize_t sizeRead = 0;
	if (pos < (off_t)fLength) {
		sizeRead = min_c((off_t)size, (off_t)fLength - pos);
		memcpy(buffer, fData + pos, sizeRead);
	}

	return sizeRead;
}

ssize_t BMallocIO::WriteAt(off_t pos, const void* buffer, size_t size)
{
	if (!buffer)
		return B_BAD_VALUE;

	size_t	 newSize = max_c(pos + (off_t)size, (off_t)fLength);
	status_t error	 = B_OK;

	if (newSize > fMallocSize)
		error = SetSize(newSize);

	if (error == B_OK) {
		memcpy(fData + pos, buffer, size);
		if (pos + size > fLength)
			fLength = pos + size;
	}

	return error != B_OK ? error : size;
}

off_t BMallocIO::Seek(off_t position, uint32 seekMode)
{
	switch (seekMode) {
		case SEEK_SET:
			fPosition = position;
			break;
		case SEEK_END:
			fPosition = fLength + position;
			break;
		case SEEK_CUR:
			fPosition += position;
			break;
		default:
			break;
	}
	return fPosition;
}

off_t BMallocIO::Position() const
{
	return fPosition;
}

status_t BMallocIO::SetSize(off_t size)
{
	status_t error = B_OK;
	if (size == 0) {
		// size == 0, free the memory
		free(fData);
		fData		= nullptr;
		fMallocSize = 0;
	}
	else {
		// size != 0, see, if necessary to resize
		size_t newSize = (size + fBlockSize - 1) / fBlockSize * fBlockSize;
		if (size != (off_t)fMallocSize) {
			// we need to resize
			if (char* newData = static_cast<char*>(realloc(fData, newSize))) {
				// set the new area to 0
				if (newSize > fMallocSize)
					memset(newData + fMallocSize, 0, newSize - fMallocSize);
				fData		= newData;
				fMallocSize = newSize;
			}
			else  // couldn't alloc the memory
				error = B_NO_MEMORY;
		}
	}

	if (error == B_OK)
		fLength = size;

	return error;
}

void BMallocIO::SetBlockSize(size_t blockSize)
{
	if (blockSize == 0)
		blockSize = 1;

	if (blockSize != fBlockSize)
		fBlockSize = blockSize;
}

const void* BMallocIO::Buffer() const
{
	return fData;
}

size_t BMallocIO::BufferLength() const
{
	return fLength;
}
