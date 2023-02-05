#include "AppFileInfo.h"

#include "NodeInfo.h"

BAppFileInfo::BAppFileInfo()
	: BNodeInfo()
{
}

BAppFileInfo::BAppFileInfo(BFile *file)
	: BAppFileInfo()
{
}

BAppFileInfo::~BAppFileInfo()
{
}

status_t BAppFileInfo::SetTo(BFile *file)
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::GetType(char *type) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::GetSignature(char *sig) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
}

status_t BAppFileInfo::GetAppFlags(uint32 *flags) const
{
	debugger(__PRETTY_FUNCTION__);
	return B_ERROR;
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
