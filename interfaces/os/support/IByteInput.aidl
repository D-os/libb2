package os.support;
import os.support.Value;

interface IByteInput
{
    long Read(out Value buffer, long size, int flags);
}
