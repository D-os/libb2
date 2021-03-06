/*!	@file support/IDatum.idl
	@ingroup CoreSupportDataModel

	@brief Interface to a data item.
*/

/*!	@addtogroup CoreSupportDataModel Data Model
	@ingroup CoreSupport
	@brief These Binder interfaces and classes provide a rich, standard
	data model for the rest of the system to use.

	The core data model
	API is expressed in four interfaces: IDatum, INode, IIterable,
	and ICatalog.  In additional, helper classes are provided to with
	the use and implementation of these basic interfaces.  See
	@ref BinderDataModel for more extensive documentation on these
	classes.
*/

package os.support;

//!	Interface to a single piece of data, such as a file.
/*!	An IDatum represents a single, indivisible piece of data in the
	@ref BinderDataModel.  Clients will usually use the SDatum convenience
	class instead of making direct calls on an IDatum.  Implementations
	should never directly implement IDatum, instead deriving from BGenericDatum
	or one of its subclasses.

	The IDatum API is designed to enable two approaches to accessing data.
	The first maps directly to a traditional file, where you perform an "open"
	operation to retrieve one or more interfaces (of IStorage, IByteInput,
	IByteOutput, IByteSeekable) that read and/or write a raw stream of bytes.
	You can also view it as a container for a single piece of data, in which
	case you retrieve and write it as a single SValue object type.

	In general you will always be able to Open() the contents of a datum, but
	there will be many times when you can't retrieve it directly as an SValue.
	For example, if the datum is more than 4KB, it will exceed the current IPC
	data size limit.

	The value of a datum should never contain a Binder object.  If you want to
	place a Binder object at a location in the namespace, it can be put there
	directly without wrapping it in a datum.  The role of the datum is really
	to provide a Binder object for data that otherwise is not itself an object.

	The CopyTo() and CopyFrom() methods allow the datum to implement optimized
	opying paths.  The BGenericDatum base class implements both of these by either
	manually copying the data (reading from the source and writing to the
	destination) if NO_COPY_REDIRECTION is set, or calling the matching version
	on the other datum with NO_COPY_REDIRECTION.  Subclasses can implement
	specialized versions of these functions in cases where they can perform copies
	between datums more efficiently.

	@ingroup CoreSupportDataModel
*/
interface IDatum
{
		//!	Retrieve a read-only stream from Open().
		const int READ_ONLY			= 0x0000;
		//!	Retrive a write-only stream from Open().
		const int WRITE_ONLY			= 0x0001;
		//!	Retrive a read/write stream from Open().
		const int READ_WRITE			= 0x0002;
		//!	Bits for Open() flags to control read/write access.
		const int READ_WRITE_MASK		= 0x0003;

		//!	Use with Open() to get an empty stream.
		const int ERASE_DATUM			= 0x0200;
		//!	Use with Open() for the stream position to start at the end.
		const int OPEN_AT_END			= 0x0400;

		//!	Used with CopyTo() and CopyFrom() to prevent one from calling the other.
		/*!	You will not normally use this flag yourself.  Instead,
			implementations of IDatum will use it when redirecting
			between their CopyTo() and CopyFrom() implementation, to
			prevent recursion from happening.  When this flag is
			set, the CopyTo() or CopyFrom() implementation being
			called must do the copy itself.
		*/
		const int NO_COPY_REDIRECTION     = 0x0001;

// properties:
// 	//! Current type code of this datum.  Default is B_RAW_TYPE.
// 	uint32_t valueType;
// 	//! Number of bytes in datum.
// 	off_t size;

// 	//! Access datum as an SValue.
// 	/*! You can link to this for notifications when the datum
// 		changes.  If the datum can not be represented as a value,
// 		the value here is "undefined" OR an error value, and it
// 		is not pushed when the datum changes.  To ensure you
// 		always get notified when it changes, you should use the
// 		DatumChanged event.
// 	*/
// 	SValue value;

// methods:
	//! Access data as stream/storage.
	/*!	@param[in] mode The desired open mode: READ_ONLY,
			WRITE_ONLY, READ_WRITE, ERASE_DATUM, OPEN_AT_END.
		@param[in] editor An optional IBinder object providing
			the identity of modifications made through this
			Open() call.  Will be propagated through DatumChanged
			so that you can identify changes due to your own
			modifications.
		@param[in] newType Optional new type code for the
			datum.  If 0 (the default), the current type will
			remain unchanged.
		@result A new object through which you can manipulate
			the data inside of the IDatum.  Use interface_cast<T>
			to retrieve the IByteInput, IByteOutput, IByteSeekable,
			and IStorage interfaces as desired.  Returns NULL
			if there was an error.

		@todo Add status_t output to retrieve error codes.
	*/
	IBinder Open(int mode, @nullable IBinder editor, int newType);

	//!	Write this datum in to @a dest.
	/*!	Upon a succeessful return, @a dest will contain the same
		data as the source datum.  The implementation may
		accomplish this by calling CopyFrom() on @a dest, unless
		the NO_COPY_REDIRECTION flag is set.
	*/
	int CopyTo(IDatum dest, int flags);

	//!	Read @a src in to this datum.
	/*!	Upon a succeessful return, this datum will hold the
		same data as @a src.  The implementation may
		accomplish this by calling CopyTo() on @a src, unless
		the NO_COPY_REDIRECTION flag is set.
	*/
	int CopyFrom(IDatum src, int flags);

// events:
// 	//!	Pushed when the contents of the datum changes.
// 	/*!	@param[in] who The datum that changed.
// 		@param[in] editor The editor that made the changed (as supplied
// 			to Open()).
// 		@param[in] start The location in the datum where the change started.
// 		@param[in] length The number of bytes from "start" that changed.

// 		If the contents of the datum is small enough, the "value"
// 		property may be pushed at the same time as this event.
// 	*/
// 	void DatumChanged(IDatum who, IBinder editor, off_t start, off_t length);
}
