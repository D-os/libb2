package os.support;
import os.support.Value;

interface IIterator
{
    const int BINDER_IPC_LIMIT = 0x05;

    const int REQUEST_DATA        = 0x1000;
    const int COLLAPSE_NODE		= 0x2000;
    const int IGNORE_PROJECTION	= 0x4000;

    void next(out Value[] keys, out Value[] values, int flags, int count);
}
