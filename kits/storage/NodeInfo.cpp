#include "NodeInfo.h"

#include <Node.h>

BNodeInfo::BNodeInfo()
	: fNode{nullptr},
	  fCStatus{B_NO_INIT} {}

BNodeInfo::BNodeInfo(BNode *node)
	: BNodeInfo()
{
	fCStatus = SetTo(node);
}

BNodeInfo::~BNodeInfo()
{
}

status_t BNodeInfo::SetTo(BNode *node)
{
	fCStatus = (node && node->InitCheck() == B_OK) ? B_OK : B_BAD_VALUE;
	fNode	 = fCStatus == B_OK ? node : nullptr;
	return fCStatus;
}

status_t BNodeInfo::InitCheck() const
{
	return fCStatus;
}

status_t BNodeInfo::GetType(char *type) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BNodeInfo::SetType(const char *type)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BNodeInfo::GetIcon(BBitmap *icon, icon_size k) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BNodeInfo::SetIcon(const BBitmap *icon, icon_size k)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}
