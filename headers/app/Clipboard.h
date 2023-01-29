#ifndef _CLIPBOARD_H
#define _CLIPBOARD_H

#include <Locker.h>
#include <Messenger.h>

class BMessage;

enum {
	B_CLIPBOARD_CHANGED = 'CLCH'
};

class BClipboard
{
   public:
	BClipboard(const char *name, bool transient = false);
	virtual ~BClipboard();

	const char *Name() const;

	uint32	 LocalCount() const;
	uint32	 SystemCount() const;
	status_t StartWatching(BMessenger target);
	status_t StopWatching(BMessenger target);

	bool Lock();
	void Unlock();
	bool IsLocked() const;

	status_t Clear();
	status_t Commit();
	status_t Revert();

	BMessenger DataSource() const;
	BMessage  *Data() const;

   private:
	BClipboard(const BClipboard &);
	BClipboard &operator=(const BClipboard &);

	uint32	  fCount;
	BMessage *fData;
	BLocker	  fLock;
	// BMessenger fClipHandler;
	BMessenger fDataSource;
	uint32	   fSystemCount;
	char	  *fName;
};

/// Global Clipboard
extern BClipboard *be_clipboard;

#endif /* _CLIPBOARD_H */
