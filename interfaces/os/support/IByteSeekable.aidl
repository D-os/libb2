package os.support;

interface IByteSeekable
{
    long Seek(long position, int seek_mode);
}
