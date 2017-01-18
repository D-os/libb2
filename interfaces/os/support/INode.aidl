package os.support;
import os.support.Value;

interface INode
{
    const int REQUEST_DATA      = 0x1000;
    const int COLLAPSE_NODE		= 0x2000;
    const int IGNORE_PROJECTION	= 0x4000;

    const int CREATE_DATUM  		= 0x0100;
    const int CREATE_NODE			= 0x0200;	//!< Create INode objects in path as needed.
    const int CREATE_MASK			= 0x0300;	//!< Set of possible create flags.

    const int CHANGE_DETAILS_SENT	= 0x0001;

    void Walk(String path, int flags, out Value node);
}
