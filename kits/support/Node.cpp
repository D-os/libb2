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

#include <support/Node.h>
#include <support/SupportDefs.h>

//#include <support/StdIO.h>

namespace os { namespace support {

// =================================================================================

Node::Node()
{
}

//Node::Node(const Context& context, const String& path, uint32_t node_flags)
//{
//	if (path.Length() > 0)
//	{
//		m_node = context.Root().ptr();
//		if (path != "/")
//		{
//			// now we walk to the path. Note that we have to have
//			// m_node set to the root or this walk will fail.
//			Value value = this->Walk(path, node_flags);
//			m_node = INode::AsInterface(value);
//		}
//	}
//}

Node::Node(const Value& value)
    : m_value(value)
    , m_node(interface_cast<INode>(value.as<sp<IBinder>>()))
{
}

Node::Node(const sp<IBinder>& binder)
    : m_node(interface_cast<INode>(binder))
{
}

Node::Node(const sp<INode>& node)
    : m_node(node)
{
}

Node::Node(const Node& node)
    : m_value(node.m_value)
    , m_node(node.m_node)
{
}

Node::~Node()
{
}

Node& Node::operator=(const Node& o)
{
    m_value = o.m_value;
    m_node = o.m_node;
    return *this;
}

bool Node::operator<(const Node& o) const
{
    return m_node != NULL ? m_node < o.m_node : m_value < o.m_value;
}

bool Node::operator<=(const Node& o) const
{
    return m_node != NULL ? m_node <= o.m_node : m_value <= o.m_value;
}

bool Node::operator==(const Node& o) const
{
    return m_node != NULL ? m_node == o.m_node : m_value == o.m_value;
}

bool Node::operator!=(const Node& o) const
{
    return m_node != NULL ? m_node != o.m_node : m_value != o.m_value;
}

bool Node::operator>=(const Node& o) const
{
    return m_node != NULL ? m_node >= o.m_node : m_value >= o.m_value;
}

bool Node::operator>(const Node& o) const
{
    return m_node != NULL ? m_node > o.m_node : m_value > o.m_value;
}

//status_t Node::StatusCheck() const
//{
//	return m_node != NULL ? OK : (m_value.IsSimple() ? B_ERROR : OK);
//}

//status_t Node::ErrorCheck() const
//{
//    return StatusCheck();
//}

sp<INode> Node::node() const
{
    return m_node;
}

Value Node::CollapsedNode() const
{
    return m_value;
}

//status_t FindLeaf(sp<INode> *node, String *pathName)
//{
//	Value entry;
//	status_t outErr = OK;
//	String path;

//	pathName->PathGetParent(&path);

//	while (true)
//	{
//		entry.Undefine();
//		outErr = (*node)->Walk(&path, 0, &entry);
//		if (outErr != OK || path.Length() == 0) {
//			// Success or failure, terminate and return result.
//			*pathName = String(pathName->PathLeaf());
//			break;
//		}
//		(*node) = INode::AsInterface(entry);
//		if ((*node) == NULL) {
//			// We were not able to completely traverse the path,
//			// but the node hierarchy didn't return an error
//			// code.  This should not happen...  but if it does,
//			// generate a reasonable error.
//			outErr = NAME_NOT_FOUND;
//			break;
//		}
//	}

//	return outErr;
//}

//status_t Node::AddEntry(const String& name, const Value& entry) const
//{
//	if (name.FindFirst(*name.PathDelimiter()) < 0) {
//		if (m_catalog != NULL) return m_catalog->AddEntry(name, entry);
//		get_catalog();
//		return (m_catalog != NULL) ? m_catalog->AddEntry(name, entry) : B_UNSUPPORTED;
//	}

//	sp<INode> node = m_node;
//	String path(name);

//	if (node != NULL) {
//		status_t err = FindLeaf(&node,&path);
//		if (err != OK) return err;

//		sp<ICatalog> catalog = ICatalog::AsInterface(m_node->AsBinder());
//		return (catalog != NULL) ? catalog->AddEntry(path, entry) : B_UNSUPPORTED;
//	}

//	return B_UNSUPPORTED; // We don't yet support updating values in an Value...
//}

//status_t Node::RemoveEntry(const String& name) const
//{
//	if (name.FindFirst(*name.PathDelimiter()) < 0) {
//		if (m_catalog != NULL) return m_catalog->RemoveEntry(name);
//		get_catalog();
//		return (m_catalog != NULL) ? m_catalog->RemoveEntry(name) : B_UNSUPPORTED;
//	}

//	sp<INode> node = m_node;
//	String path(name);

//	if (node != NULL) {
//		status_t err = FindLeaf(&node,&path);
//		if (err != OK) return err;

//		sp<ICatalog> catalog = ICatalog::AsInterface(m_node->AsBinder());
//		return (catalog != NULL) ? catalog->RemoveEntry(path) : B_UNSUPPORTED;
//	}

//	return B_UNSUPPORTED; // We don't yet support removing values in an Value...
//}

//status_t Node::RenameEntry(const String& name, const String& rename) const
//{
//	if (name.FindFirst(*name.PathDelimiter()) < 0) {
//		if (m_catalog != NULL) return m_catalog->RenameEntry(name, rename);
//		get_catalog();
//		return (m_catalog != NULL) ? m_catalog->RenameEntry(name, rename) : B_UNSUPPORTED;
//	}

//	String parentName,parentRename;
//	name.PathGetParent(&parentName);
//	rename.PathGetParent(&parentRename);
//	if (parentName != parentRename) {
//		/*	We don't "yet" support renaming (really moving) entries
//			across different nodes. */
//		return B_UNSUPPORTED;
//	}

//	sp<INode> node = m_node;
//	String path(name);

//	if (node != NULL) {
//		status_t err = FindLeaf(&node,&path);
//		if (err != OK) return err;

//		sp<ICatalog> catalog = ICatalog::AsInterface(m_node->AsBinder());
//		return (catalog != NULL) ? catalog->RenameEntry(path, String(rename.PathLeaf())) : B_UNSUPPORTED;
//	}

//	return B_UNSUPPORTED; // We don't yet support renaming values in an Value...
//}

Value Node::Walk(const String& path, uint32_t flags) const
{
    String tmp(path);
    status_t err;
    return Walk(&tmp, &err, flags);
}

Value Node::Walk(String* path, uint32_t flags) const
{
    status_t err;
    return Walk(path, &err, flags);
}

Value Node::Walk(const String& path, status_t* outErr, uint32_t flags) const
{
    String tmp(path);
    return Walk(&tmp, outErr, flags);
}

Value Node::Walk(String* path, status_t* outErr, uint32_t flags) const
{
    if (path->length() == 0 || *path == "/")
    {
        // Requesting root node.
        *outErr = OK;
        if (m_node != NULL)
            return Value(IInterface::asBinder(m_node));
        return m_value;
    }

    Value entry;
    sp<INode> node = m_node;

    if (node != NULL) {
walk_path:
        while (true)
        {
            entry = Value();
            *outErr = node->Walk(*path, flags, &entry).transactionError();
            if (*outErr != OK || path->length() == 0) {
                // Success or failure, terminate and return result.
                break;
            }
            node = INode::asInterface(entry.as<sp<IBinder>>());
            if (node == NULL) {
                // We were not able to completely traverse the path,
                // but the node hierarchy didn't return an error
                // code.  This should not happen...  but if it does,
                // generate a reasonable error.
                *outErr = NAME_NOT_FOUND;
                return Value();
            }
        }

    } else if (m_value.valid()) {
        entry = m_value[*path];
        if (entry.valid()) {
            *outErr = OK;
        } else {
            // Try to walk a path.
            {
                String name = path->walkPath();
                entry = m_value[name];
            }

            // If this is a path with multiple names, then we can
            // try to walk into it.
            if (path->length() > 0) {
                node = interface_cast<INode>(entry.as<sp<IBinder>>());
                if (node != NULL) {
                    goto walk_path;
                }
            }

            *outErr = NAME_NOT_FOUND;
        }
    } else {
        *outErr = NO_INIT;
    }

    // we do not want to return an Value that
    // has garbage in it if we had an error
    if (*outErr != OK) return Value();

    return entry;
}

} } // namespace os::support
