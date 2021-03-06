/*!	@file support/ICatalog.h
	@ingroup CoreSupportDataModel

	@brief Generic node modification interface.
*/

package os.support;

import os.support.IDatum;
import os.support.INode;
import os.support.SValue;

//!	Generic modification interface for INode objects.
/*!	The ICatalog interface provides a generic API for
	modifying nodes in the @ref BinderDataModel.  An object
	that implements ICatalog will always implement INode --
	ICatalog is essentially the write side of INode.

	Objects that implement 2d data structures will usually
	also implement ITable.  Some objects may also provide
	more specialized interfaces for manipulating their contents
	as well as or in addition to ICatalog. See, for example,
	IMenuData.

	When talking about the data model, we usually refer to
	objects implementing ICatalog as being "a catalog", and
	assuming that these objects also implement INode and IIterable.
	This is by far the most common configuration of a container
	object in a namespace.

	Note that, like INode, entries in a catalog are named by
	strings only &mdash; they do not use generic SValue keys.  All
	Unicode characters are valid in an entry name except for "/"
	(which is the path separator), and a ":" at the beginning
	of the path (which is used to identify the attributes namespace).

	@ingroup CoreSupportDataModel
*/
interface ICatalog
{
// methods:
	//!	Add or modify an entry in the catalog.
	/*! @param[in] name Desired name of the entry.
		@param[in] entry Data or object for the entry.
		@result B_OK on success, else an error code.

		AddEntry() is a general way to place new entries into a
		catalog.  There are two main ways it can be useful.  The
		first is as an equivalent to the INode::REQUEST_DATA hint
		for reading entries �- if you supply an SValue of data to
		ddEntry(), it can create a new entry with that data, without
		requiring an intermediate operation through an IDatum object.

		The second reason for using this API is to add new types of
		objects to the catalog � if the SValue contains an IBinder
		object, the given object will be directly added as the new
		entry.  This latter approach allows you to create places where
		the namespace crosses processes, mount new types of directories
		in the namespace, etc.  Note, however, that many catalogs
		(such as one representing a filesystem) will
		not be able to host references to external objects.

		Contrast this with CreateNode() and CreateDatum(), which creates
		new entry objects that are owned and managed by the catalog.

		@todo This API should not allow both creation of new entries
		and modification of existing entries.

		@todo This API should work more like CreateDatum(), where the
		name is an in/out parameter, and it should be able to return
		the created object as an optional output.  Perhaps this
		functionality should be moved to CreateNode() and CreateDatum(),
		and this method changed to ReplaceEntry() to only modify an
		existing entry.
	*/
	int AddEntry(String name, in SValue entry);

	//!	Remove an existing entry from the catalog.
	/*!	@param[in] name Name of the entry to be removed.
		@result B_OK if the entry was removed, some other error code
			on error.  In particular, B_ENTRY_NOT_FOUND if there is
			not an entry with the given name.
	*/
	int RemoveEntry(String name);

	//!	Change the name of an entry in the catalog.
	/*!	@param[in] entry Name of the entry to be renamed.
		@param[in] name New name for the entry.
		@result B_OK if the entry was removed, some other error code
			on error.  Most common errors are B_ENTRY_NOT_FOUND
			(@a entry does not exist), B_ENTRY_EXISTS (there is
			already an existing entry named @a name), B_UNSUPPORTED
			(this catalog does not support renaming).
	*/
	int RenameEntry(String entry, String name);

	//!	Create a new INode in this catalog.
	/*!	@param[in,out] name Incoming, the desired name for the
			new entry.  Outgoing, the actual name that was used.
			Some catalogs may completely ignore your desired
			name and use their own.  Alternatively, a catalog
			may use your name as-is, and return an error if an
			entry with that name already exists.
		@param[out] err B_OK on success, else an error code.
		@result The newly created INode, or NULL on failure.

		Use this API to create a new subdirectory kind of
		object inside of this catalog.  The returned object
		will be at least an INode that can itself contain
		one or more child entries.  However, the details of
		how the node INode works are entirely dependent
		on this containing catalog: it could start out
		empty and have its own ICatalog interface through
		which you can add any arbitrary entries, start out
		with a fixed set of entries that can not be changed,
		etc.

		This, along with CreateDatum(), is the mechanism you
		should normally use when entries in a particular catalog,
		because they allow the catalog to ensure that objects
		with the correct implementation are created.  For example,
		if the catalog is on a filesystem, it will need to modify
		the filesystem data to hold the new structure and create
		and return a proxy object for the new entry it just created.

		Even in a generic catalog the use of this function is
		important, since it ensures objects are created in the same
		process as the parent directory.  Consider, for example, an
		application that wishes to create a node with some data that
		will stay around after the application itself exits.

		Contrast this with AddEntry(), which if called with an IBinder
		object will place a reference to that object in the catalog.
	*/
	INode CreateNode(/*inout*/ String name/*, out int err*/);

	//!	Create a new IDatum object inside of this catalog.
	/*!	@param[in,out] name Incoming, the desired name for the
			new entry.  Outgoing, the actual name that was used.
			Some catalogs may completely ignore your desired
			name and use their own.  Alternatively, a catalog
			may use your name as-is, and return an error if an
			entry with that name already exists.
		@param[in] flags Additional options.  Always set to 0.
		@param[out] err B_OK on success, else an error code.
		@result The newly created IDatum, or NULL on failure.

		This is like CreateNode() (see that API for further details),
		but creates an object implementing the IDatum interface and
		thus allowing you to place actual data under this entry.

		Note that the returned object may very well support other
		interfaces (including INode), however its main purpose is to
		hold data for you.
	*/
	IDatum CreateDatum(/*inout*/ String name, int flags/*, out int err*/);
}
