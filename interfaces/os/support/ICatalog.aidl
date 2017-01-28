package os.support;
import os.support.INode;
import os.support.IDatum;
import os.support.Value;

interface ICatalog
{
    void addEntry(String name, in Value entry);
    void removeEntry(String name);
    void renameEntry(String entry, String name);
    INode createNode(String name);
    IDatum createDatum(String name, int flags);
}
