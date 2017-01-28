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

#ifndef _STORAGE_INDEXEDITERABLE_H
#define _STORAGE_INDEXEDITERABLE_H

/*!	@file storage/IndexedIterable.h
    @ingroup CoreSupportDataModel

    @brief Base class for IIterable implementations that contain an array of items.
*/

#include <storage/GenericIterable.h>

//#include <support/Vector.h>

namespace os { namespace storage {

/*!	@addtogroup CoreSupportDataModel
    @{
*/

// ==========================================================================
// ==========================================================================

//!	Base class for IIterable implementations that contain an array of items.
/*!	This class builds from BGenericIterable a kind of iterable that contains
    an indexed array of items.  It introduces a new IIterator class,
    IndexedIterator, that maintains a position in the array.

    Derived classes can usually simply fill in the new virtuals defined here
    in order to have a working iterable; there is no need to create another
    class derived from IndexedIterator.

    @nosubgrouping
*/
class IndexedIterable : public GenericIterable
{
public:
    // --------------------------------------------------------------
    /*!	@name Bookkeeping
        Creation, destruction, locking, etc. */
    //@{
                                    IndexedIterable();
                                    IndexedIterable(const Context& context);
    //@}

    // ---------------------------------------------------------------------
    //! @name Generic Iterable Virtuals
    /*!	Provide new implementation of some BGenericIterable virtuals. */
    //@{

    class IndexedIterator;

            //!	Implemented to create an IndexedIterator object.
    virtual	sp<GenericIterator>	newGenericIterator(const Value& args);

    //@}

    // ---------------------------------------------------------------------
    //! @name Indexed Iterable
    /*!	Subclasses must override some virtuals to supply the remaining iterable
        implementation, and can override other virtuals to customize its
        behavior, and take advantage of helper functions here. */
    //@{

            //!	Subclasses must implement this to return an item from the iterator.
            /*!	It is up to the implementor to deal with the REQUEST_DATA flag if they
                want to provide that optimization.  This class guarantees it will never
                call EntryAtLocked() with an index that is out of range, though other
                classes may not be so kind. */
    virtual	status_t				entryAtLocked(	const sp<IndexedIterator>& it, size_t index,
                                                    uint32_t flags, Value* key, Value* entry) = 0;
            //!	Remove the item at 'index' from the iterable.
            /*!	The default implementation returns B_UNSUPPORTED. */
    virtual	status_t				removeEntryAtLocked(size_t index);
            //!	Subclasses must implement this to return the total number of items available.
    virtual	size_t					countEntriesLocked() const = 0;
            //!	Optionally implement this to provide support for BV_ITERABLE_SELECT.
            /*!	The default implementation simply makes inoutSelection undefined and outCookie
                empty, meaning it will do no slection.  You can implement this yourself to
                modify inoutSelection to match the exact keys you have available, and optionally
                fill in outCookie with any information useful to perform the selection. */
    virtual	void					createSelectionLocked(Value* inoutSelection, Value* outCookie) const;
            //!	Optionally implement this to provide support for BV_ITERABLE_ORDER_BY.
            /*!	The default implementation simply makes inoutSortBy undefined and outOrder
                empty, meaning it will do no sorting.  You can implement this yourself to
                modify inoutSortBy to match the exact keys you are sorting by, and fill
                in outOrder with a permutation vector that maps positions in the IndexedIterator
                to indices in BIndexedIterable. */
    virtual	void					createSortOrderLocked(Value* inoutSortBy, Vector<ssize_t>* outOrder) const;

            //!	Adjust all active iterators due to items being added/removed from the iterable.
            /*!	Your subclass must call this when it changes the contents of the iterable.
                Note that this must be done -after- you have finished the changed, since
                active iterators may need to recompute their sort order at this point. */
            void					updateIteratorIndicesLocked(size_t index, ssize_t delta);

    //@}

protected:
    /*!	@name Bookkeeping */
    //@{
    virtual							~IndexedIterable();
    //@}

private:
                                    IndexedIterable(const IndexedIterable& copyFrom);
                                    IndexedIterable& operator=(const IndexedIterable& copyFrom);

    static	bool					update_indices_func(	const sp<GenericIterable>& iterable,
                                                            const sp<GenericIterator>& iterator,
                                                            void* cookie);
};

// --------------------------------------------------------------------------

//!	Iterator over an BIndexedIterable's data set.
/*!
    @nosubgrouping
*/
class IndexedIterable::IndexedIterator : public GenericIterable::GenericIterator
{
public:
    // --------------------------------------------------------------
    /*!	@name Bookkeeping
        Creation, destruction, locking, etc. */
    //@{
                                IndexedIterator(const Context& context, const sp<GenericIterable>& owner);

protected:
    virtual						~IndexedIterator();
public:

            //!	This iterator supports for IIterator and IRandomIterator.
    virtual Value				inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags);

    //@}

    // ---------------------------------------------------------------------
    //! @name IIterator/IRandomIterator interfaces.
    /*!	Add implementation for IRandomIterator. */
    //@{

            //!	Add in BV_ITERABLE_SELECT and BV_ITERABLE_ORDER_BY arguments.
            /*!	If the subclass has implemented CreateSortOrderLocked() to support
                sorting, this function will add that data in to GenericIterable::Options(). */
    virtual	Value				options() const;

            //!	Calls back to BIndexedIterable::CountEntriesLocked().
    virtual size_t				count() const;
            //!	Returns the current iterator position.
    virtual	size_t				position() const;
            //!	Changes the current iterator position.
    virtual void				setPosition(size_t p);

    //@}

    // ---------------------------------------------------------------------
    //! @name GenericIterator.
    /*!	Finish implementation of GenericIterator base class. */
    //@{

            //!	Parse out BV_ITERATOR_SELECT and BV_ITERABLE_ORDER_BY arguments.
            /*!	Calls CreateSelectionLocked() to allow subclasses to implement
                project and CreateSortOrderLocked() to allow subclasses to implement sorting. */
    virtual	status_t			parseArgs(const Value& args);

            //!	Implemented to call step the iterator and then call EntryAtLocked() with the previous index.
    virtual	status_t			nextLocked(uint32_t flags, Value* key, Value* entry);
            //!	Implemented to call RemoveEntryAtLocked() with the current index.
    virtual	status_t			removeLocked();

    //@}

    // ---------------------------------------------------------------------
    //! @name Positions and Indices.
    /*!	Conveniences for managing the iterator position, and mapping that
        to indices in the iterable. */
    //@{

            //!	Return the select argument in effect for this iterator.
            /*!	B_UNDEFINED_VALUE means to return all items. */
    inline	const Value&		selectArgsLocked() const { return m_selectArgs; }

            //!	Returns the iterator's current index in the iterable.
            /*!	This is different than Position() -- it is the actual
                index in the physical data, after the sort order has been
                applied.  Note that the returned index can be B_END_OF_DATA
                if we have reached the end of the iterator, or B_ENTRY_NOT_FOUND
                if this item in the iterator has been deleted from the
                iterable. */
            ssize_t				currentIndexLocked() const;
            //!	Moves the iterator forward by one and returns its previous \e index.
            /*!	This is equivalent to calling CurrentIndexLocked(), and then
                SetPosition(Position()+1). */
            ssize_t				moveIndexLocked();
    //@}

private:
            friend class IndexedIterable;

            void				update_indices_l(size_t index, ssize_t delta);

            size_t				m_count;
            Value				m_selectArgs;
            Value				m_selectCookie;
            Value				m_orderArgs;
            Vector<ssize_t>	m_order;
            size_t				m_position;
};

// ==========================================================================
// ==========================================================================

/*!	@} */

} } // namespace os::storage

#endif // _STORAGE_INDEXEDITERABLE_H
