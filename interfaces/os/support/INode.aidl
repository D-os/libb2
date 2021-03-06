/*!	@file support/INode.idl
	@ingroup CoreSupportDataModel

	@brief Basic interface to a container in the Binder namespace.
*/

package os.support;

import os.support.SValue;

//!	Basic interface to a container in the Binder namespace.
/*!	Traversal through the @ref BinderDataModel revolves around the INode interface.
	Its key method is Walk(), which is used to resolve a path string
	into an object in a namespace. An INode also provides separate
	access to meta-data associated with its object.

	Clients will usually use the SNode convenience
	class instead of making direct calls on an INode.  Implementations
	should never directly implement INode, instead deriving from BGenericNode
	or one of its subclasses.

	@ingroup CoreSupportDataModel
*/
interface INode
{
	//!	Flags used in Walk() and other catalog functions as well.
	/*!	There is a copy of these flags in IIterator, if you change
		these don't forget to change those as well. */

		//!	Return the contents of an IDatum instead of the object itself?
		/*!	Can be supplied to IIterator::Next() and various INode functions.
			Implementations of IIterator are not required to respect this
			flag -- clients must deal with them ignoring it.
		*/
		const int REQUEST_DATA        = 0x1000;

		//!	Collapse catalog entries to SValue mappings?
		/*!	Can be supplied to IIterator::Next() and various INode functions.
			Implementations of IIterator are not required to respect this
			flag -- clients must deal with then ignoring it.  Often used
			in conjunction with REQUEST_DATA so that you get back a collapsed
			node containing the data instead of IDatum objects.
		*/
		const int COLLAPSE_NODE		= 0x2000;

		//!	Ignore any projections when creating the output value/node?
		/*!	Can be supplied to IIterator::Next() and various INode functions.
			This is only useful when an iterator actually implements
			projections; if it does, it must also implement this flag
			(though implementing the flag may mean just returning the INode
			object and ignore the COLLAPSE_NODE flag).
		*/
		const int IGNORE_PROJECTION	= 0x4000;

	//!	Flags for Walk()
	/*!	When these flags are passed to Walk(), the walk
		will create either a datum or a node when it
		reaches the leaf in the specified path. */
		const int CREATE_DATUM		= 0x0100;	//!< Create IDatum object at leaf, if needed.
		const int CREATE_NODE			= 0x0200;	//!< Create INode objects in path as needed.
		const int CREATE_MASK			= 0x0300;	//!< Set of possible create flags.

	//!	Flags for INode::NodeChanged event.
		//!	Used in INode::NodeChanged when a detailed event was also sent.
		/*!	You can use this flag to determine if the node has also pushed
			one of the most detailed change events.  If it is not sent, then
			this is the ONLY change notifications you will get, and you can't
			make any more assumptions about the contents of the node.  If it
			is set and you have linked to the other change events, you can
			safely ignore this event.
		*/
		const int CHANGE_DETAILS_SENT	= 0x0001;

// properties:
// 	//!	Retrieve the meta-data catalog associated with this node, or NULL if it doesn't exist.
// 	/*! The INode interface also supplies access to meta-data associated with the object.
// 		The "attributes" read-only property provides direct access to the meta-data node,
// 		which is a pointer to another separate INode holding the meta-data.  A node may not
// 		support meta-data, in which case the attributes property will return NULL.

// 		You will not usually use this property directly, instead using the mimeType,
// 		creationDate, and modifiedDate properties to read/write the standard attributes.
// 		Also see Walk() for how you can retrieve these and other attributes through the
// 		normal path traversal mechanism.
// 	*/
// 	[readonly]INode attributes;

// 	//!	Retrieve the "mimeType" meta-data entry, or "" if it doesn't exist.
// 	SString mimeType;
// 	//!	Retrieve the "creationDate" meta-data entry, or 0 of it doesn't exist.
// 	nsecs_t creationDate;
// 	//!	Retrieve the "modifiedDate" meta-data entry, or 0 of it doesn't exist.
// 	nsecs_t modifiedDate;

// methods:
	//!	Walk through the namespace based on the given path.
	/*!	@param[in,out] path Incoming, the '/'-separated path to traverse.
			Outgoing, if result is B_OK then this is either the remaining
			path to be walked or "" if the full path has been processed.
			Otherwise, the output value is not defined.
		@param[in] flags Options controlling the traversal: REQUEST_DATA,
			COLLAPSE_NODE, CREATE_DATUM, CREATE_NODE.
		@param[out] node The entry found at the given path.  If REQUEST_DATA
			and COLLAPSE_NODE are not provides, this is always an IBinder
			object; otherwise, it may be some SValue data structure as
			described below.
		@return B_OK if the Walk() was successful (the @a node contains the
			result); else an error code on failure.  A common error is
			B_ENTRY_NOT_FOUND if the path could not be resolved.

		The Walk() method is used to find an object below the node, by traversing
		the given path through the namespace. You use separators ('/' -- forward
		slash) to separate components of the path. If your path contains no
		separators, you are requesting an object directly under the node.

		If the node supports attributes then the Walk() method will allow you to
		traverse directly to them.  This is accomplished by reserving path names
		whose first character is ":" to indicate that the name is part of the
		attribute namespace.  Thus a Walk() of just the path ":" will return the
		attributes catalog (and continue walking into it if needed); for a name
		with ':' as a prefix and additional text the node strips off the ':' and
		then calls Walk() on the attributes catalog with the remaining text.

		Clients will generally not call Walk() directly, instead relying
		on SNode::Walk(). The latter hides a lot of the complexity of
		INode::Walk() that we will discuss later.

		The flags parameter allows you to control how the namespace walks your
		supplied path. These allow you to request optimizations to how INode
		returns its result (REQUEST_DATA, COLLAPSE_NODE), and what it should do
		if segments of the path don't exist (CREATE_DATUM, CREATE_NODE).

		The REQUEST_DATA and COLLAPSE_NODE flags are optimization hints that allow
		you to bypass the "everything is an object" property of the namespace in
		certain situations.  An INode is not required to honor these requests
		(and indeed may be entirely unable to do so), so when using them it is
		the client's responsibility to deal with such a failure.

		- REQUEST_DATA asks, if the final node in the path is an IDatum, for it
		to return the actual data under its entry (as an SValue) instead of the IDatum
		object holding that data.  For simple read access of small pieces of data,
		this allows you to bypass the intermediate object. The node is free to ignore
		this request and return the IDatum anyway.

		- COLLAPSE_NODE asks, if the final node in the path is an INode, for it to
		return the name/value mappings it contains, as an SValue mapping, instead
		of the object itself.  Again the node is free to ignore this request and return
		the object anyway.

		You can combine REQUEST_DATA and COLLAPSE_NODE to ask for the final node to
		return an SValue mapping, where each value is actually data.  If you use
		COLLAPSE_NODE without REQUEST_DATA, you will receive SValue mappings
		where the values are IDatum objects.

		- CREATE_CATALOG flag asks for the nodes parsing your path to automatically
		create intermediate objects if the requested entries along the path don't exist.

		- CREATE_DATUM flags asks that the final node in the path create a datum for
		you if one doesn't already exist.

		@note Never implement Walk() directly yourself.  Instead, derive from
		BGenericNode which will take care of most of the following details for you.

		@par Implementation Details
		All implementations of INode must correctly handle paths in their Walk()
		implementation by finding the first path separator, looking up the node for the
		entry named by the text before the separator, and recursing into that node with
		the remaining path string.  They must also correctly parse attribute names as
		described below.
		@par
		This API is slightly complicated because a Walk() operation does not have to be
		completed in a single call. The path parameter is also an output parameter, so the
		node hierarchy can evaluate only a portion of the path, and return the node and
		remaining path up to that point.  Thus clients must repeatedly call Walk() until
		the path is fully resolved or an error occurs. For example, the node structure
		will normally return early from Walk() when it hits a point that hops across
		processes.
		@par
		Making the path an in/out parameter also has a nice performance benefit: when a
		node is modifying the path to resolve it for the current level, this can be done
		in-place directly in the string argument, instead of having to construct a new
		string to hold the sub-path.
		@par
		If an object is both an INode and an IDatum and REQUEST_DATA is used, the object
		should be treated as a datum (as its primary identity), and the REQUEST_DATA
		flag honored if possible.
		@par
		Why is Walk() defined this way, instead of simply returning the final leaf?
		The primary reason is to give the node hierarchy the ability to control how much
		recursion happens. If it couldn't break the walk up into separate steps, then
		you may be required to recurse (potentially across many processes) extremely
		deep in order to perform the walk. In addition, performance should be better by
		splitting up the walk at each process boundary (though it is completely up to
		the implementation of a particular INode to decide whether or not to return
		early from walk).

		@todo COLLAPSE_NODE should probably be removed.
	*/
	int Walk(/*inout*/ String path, int flags, out SValue node);

// events:
// 	//!	This event is sent whenever some changes happens in the node.
// 	/*!	@param[in] who The node that has changed.
// 		@param[in] flags Provides more information about the change.
// 		@param[in] hints May provide even more info, but is currently unnused.

// 		This event indicates that something has changed in the node,
// 		but not what that change is.  If the node is not also pushing
// 		another change event (as indicated by the CHANGE_DETAILS_SENT
// 		flag), then you can't assuming anything about the current
// 		contents of the node.
// 	*/
// 	void NodeChanged(INode who, uint32_t flags, SValue hints);

// 	//! This event is sent when a new entry appears in the node.
// 	/*! @param[in] who The parent node in which this change occured.
// 		@param[in] name The name of the entry that changed.
// 		@param[in] entry The entry itself, usually either an INode or IDatum.

// 		This informs you of a structural change to the node, where a
// 		new entry is appearing under it.  The IBinder @a entry identifying
// 		the new entry is the same IBinder that is returned by Walk()
// 		and IIterator, and follows the same identity rules as defined
// 		for those APIs.
// 	*/
//     void EntryCreated(INode who, SString name, IBinder entry);

// 	//! This event is sent when an entry in a catalog is modified.
// 	/*! @param[in] who The parent node in which this change occured.
// 		@param[in] name The name of the entry that changed.
// 		@param[in] entry The entry itself, either an INode or IDatum.

// 		This informs you of a non-structural change to the node, where
// 		the data inside one of its items has changed.  The IBinder @a entry identifying
// 		the changed entry is the same IBinder that is returned by Walk()
// 		and IIterator, and follows the same identity rules as defined
// 		for those APIs.
// 	*/
// 	void EntryModified(INode who, SString name, IBinder entry);

// 	//! This event is sent when an existing entry is removed from the node.
// 	/*! @param[in] who The parent node in which this change occured.
// 		@param[in] name The name of the entry that was removed.

// 		This informs you of a structural change to the node, where an
// 		existing entry has been removed from it.

// 		@todo Should this also return the IBinder of the old entry?
// 		For correctness, probably so, though that would cause some
// 		implementation difficulties.
// 	*/
//     void EntryRemoved(INode who, SString name);

// 	//! This event is sent when an entry in the catalog is renamed.
// 	/*! @param[in] who The parent node in which this change occured.
// 		@param[in] old_name The previous name of the entry.
// 		@param[in] new_name The new name of the entry.
// 		@param[in] entry The entry itself, either an INode or IDatum.

// 		This informs you of a structural change to the node, where the
// 		name of one of its entries has changed.  The IBinder @a entry identifying
// 		the renamed entry is the same IBinder that is returned by Walk()
// 		and IIterator, and follows the same identity rules as defined
// 		for those APIs.

// 		@note This is NOT sent when an entry moves between different
// 		nodes!  In that case you will get separate EntryCreated()
// 		and EntryRemoved() notifications from the two nodes.
// 	*/
//     void EntryRenamed(INode who, SString old_name, SString new_name, IBinder entry);
}
