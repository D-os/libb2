/*!	@file support/INodeObserver.idl
	@ingroup CoreSupportDataModel

	@brief Convenience for linking to INode events.
*/

package os.support;

import os.support.INode;
import os.support.SValue;

//!	Convenience for linking to INode events.
/*! @ingroup CoreSupportDataModel */
interface INodeObserver
{
// methods:
	//!	See INode::NodeChanged.
	void NodeChanged(INode who, int flags, in SValue hints);

	//!	See INode::EntryCreated.
	void EntryCreated(INode node, String name, IBinder entry);

	//!	See INode::EntryModified.
	void EntryModified(INode node, String name, IBinder entry);

	//!	See INode::EntryRemoved.
	void EntryRemoved(INode node, String name);

	//!	See INode::EntryRenamed.
	void EntryRenamed(INode node, String old_name, String new_name, IBinder entry);
}
