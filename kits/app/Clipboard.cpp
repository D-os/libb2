#include "Clipboard.h"

#include <Message.h>

BClipboard *be_clipboard = nullptr;

BClipboard::BClipboard(const char *name, bool transient)
	: fCount{0},
	  fData{new BMessage()},
	  fLock("clipboard"),
	  fSystemCount{0},
	  fName(name ? strdup(name) : strdup("system"))
{
}

BClipboard::~BClipboard()
{
	free(fName);
	delete fData;
}

const char *BClipboard::Name() const
{
	return fName;
}

uint32 BClipboard::LocalCount() const
{
	return fCount;
}

uint32 BClipboard::SystemCount() const
{
	return fSystemCount;
}

status_t BClipboard::StartWatching(BMessenger target)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BClipboard::StopWatching(BMessenger target)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

bool BClipboard::Lock()
{
	return fLock.Lock();
}

void BClipboard::Unlock()
{
	fLock.Unlock();
}

bool BClipboard::IsLocked() const
{
	return fLock.IsLocked();
}

status_t BClipboard::Clear()
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BClipboard::Commit()
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BClipboard::Revert()
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

BMessenger BClipboard::DataSource() const
{
	return fDataSource;
}

BMessage *BClipboard::Data() const
{
	return fData;
}
