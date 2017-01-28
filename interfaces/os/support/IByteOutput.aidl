package os.support;
import os.support.Value;

interface IByteOutput
{
    long Write(in Value buffer, long size, int flags);

    void Sync();
}
