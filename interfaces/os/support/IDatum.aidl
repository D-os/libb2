package os.support;

interface IDatum
{
    const int READ_ONLY			= 0x0000;
    const int WRITE_ONLY			= 0x0001;
    const int READ_WRITE			= 0x0002;
    const int READ_WRITE_MASK		= 0x0003;
    const int ERASE_DATUM			= 0x0200;
    const int OPEN_AT_END			= 0x0400;

    const int NO_COPY_REDIRECTION     = 0x0001;

    IBinder open(int mode, IBinder editor, int newType);

    void copyFrom(IDatum src, int flags);
}
