/*
 * Copyright (c) 2005 Palmsource, Inc.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 *
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef _SUPPORT_NODE_H
#define _SUPPORT_NODE_H

/*!	@file support/Node.h
    @ingroup CoreSupportDataModel
    @brief Helper class for calling the INode interface.
*/

#include <os/support/INode.h>
#include <binder/Binder.h>
#include <support/Context.h>
#include <support/String.h>
//#include <support/INodeObserver.h>

namespace os { namespace support {

/*!	@addtogroup CoreSupportDataModel
    @{
*/

//B_CONST_STRING_VALUE_LARGE(BV_ENTRY_CREATED,		"EntryCreated", );
//B_CONST_STRING_VALUE_LARGE(BV_ENTRY_MODIFIED,		"EntryModified", );
//B_CONST_STRING_VALUE_LARGE(BV_ENTRY_REMOVED,		"EntryRemoved", );
//B_CONST_STRING_VALUE_LARGE(BV_ENTRY_RENAMED,		"EntryRenamed", );

//!	Convenience class for operating on the INode interface.
/*!	This class provides some convenience methods for making
    calls on an INode, to provide a more convenience API.  In
    general you will create an Node wrapper around an INode
    you have and call the Node methods instead of making
    direct calls on the INode.  For example:

@code
    void get_something(const sp<INode>& node)
    {
        Node n(node).
        Value v = n.Walk(String("some/path"));
    }
@endcode

    When using the constructor that takes an Value, the
    class will automatically try to retrieve an INode from
    the value.  If that fails, and the Value contains
    mappings, then Walk() will look up the mappings inside
    of it.  This allows you to use INode::COLLAPSE_CATALOG without
    worrying about whether or not the returned item is
    collapsed.

    @todo Need to clean up the Walk() methods to get rid of
    amiguities between them.

    @nosubgrouping
*/
class Node
{
public:
    // --------------------------------------------------------------
    /*!	@name Bookkeeping
        Creation, destruction, copying, comparing, etc. */
    //@{
            //!	Create a new, empty node.
                        Node();
            //!	Retrieve a node from the given @a path in @a context.
            /*!	The @a node_flags are as per the INode::Walk() flags. */
                        Node(const Context& context, const String& path, uint32_t node_flags=0);
            //!	Retrieve a node from a generic Value.
            /*!	If the Value contains an INode object, that will be used
                directly.  Otherwise, if the Value contains mappings,
                Walk() will perform a lookup on it.  This allows convenient
                manipulation of INode::COLLAPSE_NODE results. */
                        Node(const Value& value);
            //!	Retrieve a node from an IBinder, casting to an INode interface.
                        Node(const sp<IBinder>& binder);
            //!	Initialize directly from an INode.
                        Node(const sp<INode>& node);
            //!	Copy from another Node.
                        Node(const Node& node);
            //!	Release reference on INode.
                        ~Node();

            //!	Replace this Node with @a o.
    Node&				operator=(const Node& o);

    bool				operator<(const Node& o) const;
    bool				operator<=(const Node& o) const;
    bool				operator==(const Node& o) const;
    bool				operator!=(const Node& o) const;
    bool				operator>=(const Node& o) const;
    bool				operator>(const Node& o) const;

            //!	Returns B_OK if we hold a value INode or Value of mappings.
    status_t			StatusCheck() const;

            //!	Retrieve the INode object being used.
    sp<INode>			node() const;

            //!	Retrieve the Value mappings being used.
    Value				CollapsedNode() const;

            //!	@deprecated Use StatusCheck() instead.
    status_t			ErrorCheck() const;

    //@}

    // --------------------------------------------------------------
    /*!	@name Path Walking
        Call INode::Walk() to resolve a path.  If this Node contains
        a collapsed Value, walk through that instead.  These functions
        take care of repeatedly calling INode::Walk() until the path
        is fully resolved or an error occurs. */
    //@{

    Value				Walk(const String& path, uint32_t flags = INode::REQUEST_DATA) const;
    Value				Walk(String* path, uint32_t flags = INode::REQUEST_DATA) const;
    Value				Walk(const String& path, status_t* outErr, uint32_t flags = INode::REQUEST_DATA) const;
    Value				Walk(String* path, status_t* outErr, uint32_t flags = INode::REQUEST_DATA) const;

    //@}

private:
    Value				m_value;
    sp<INode>			m_node;
};

//class BNodeObserver : public BnNodeObserver
//{
//public:
//    BNodeObserver();
//    BNodeObserver(const Context& context);

//    virtual ~BNodeObserver();

//    virtual void NodeChanged(const sp<INode>& node, uint32_t flags, const Value& hints);
//    virtual void EntryCreated(const sp<INode>& node, const String& name, const sp<IBinder>& entry);
//    virtual void EntryModified(const sp<INode>& node, const String& name, const sp<IBinder>& entry);
//    virtual void EntryRemoved(const sp<INode>& node, const String& name);
//    virtual void EntryRenamed(const sp<INode>& node, const String& old_name, const String& new_name, const sp<IBinder>& entry);
//};

/*!	@} */

} } // namespace os::support

#endif // _SUPPORT_NODE_H
