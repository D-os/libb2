package os.support;
import os.support.IIterator;
import os.support.Value;

interface IIterable
{
    IIterator newIterator(in @nullable Value args);
}
