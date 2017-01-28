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

#ifndef _SUPPORT_CATALOG_H
#define _SUPPORT_CATALOG_H

/*!	@file support/Catalog.h
    @ingroup CoreSupportDataModel
    @brief Helpers for implementing ICatalog and its associated interfaces.
*/

#include <support/SupportDefs.h>
//#include <storage/DatumGeneratorInt.h>
#include <storage/IndexedIterable.h>
#include <storage/MetaDataNode.h>

#include <support/Context.h>
#include <os/support/BnCatalog.h>
#include <os/support/BnIterable.h>
#include <utils/KeyedVector.h>
//#include <support/Locker.h>
#include <support/Node.h>
#include <support/String.h>
#include <support/Value.h>


namespace os { namespace support {

/*!	@addtogroup CoreSupportDataModel
    @{
*/

//class SWalkHelper
//{
//public:
//    status_t	HelperWalk(String* path, uint32_t flags, Value* node);

//    virtual sp<INode> 	HelperAttributesCatalog() const;
//    virtual status_t		HelperLookupEntry(const String& entry, uint32_t flags, Value* node) = 0;
//    virtual	sp<ICatalog>	HelperCreateCatalog(String* name, status_t* err);
//    virtual	sp<IDatum>	HelperCreateDatum(String* name, uint32_t flags, status_t* err);
//};

class GenericCatalog : public BnCatalog, public storage::MetaDataNode, public BnIterable
{
public:
    GenericCatalog();
    GenericCatalog(const Context& context);

    // Deal with multiple interfaces
//    inline const Context& Context() const { return BnCatalog::Context(); }
    virtual Value inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags);

    // Stub these out for ICatalog (call to BMetaDataNode implementation).
    virtual Status createNode(const ::android::String& name, sp<INode>* _aidl_return) override;
    virtual Status createDatum(const ::android::String& name, int32_t flags, sp<IDatum>* _aidl_return) override;

protected:
    virtual ~GenericCatalog();
};

class IndexedCatalog : public BnCatalog, public storage::MetaDataNode, public storage::IndexedIterable
{
public:
    IndexedCatalog();
    IndexedCatalog(const Context& context);

    // Deal with multiple interfaces
//    inline const Context& Context() const { return BnCatalog::Context(); }
    virtual Value inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags);

    //!	Tie together the locks.
    virtual	lock_status_t			lock() const override;
    virtual	void					unlock() const override;

    //!	Implement for compatibility.
    virtual	status_t				entryAtLocked(	const sp<IndexedIterator>& it, size_t index,
                                                    uint32_t flags, Value* key, Value* entry) override;
    //!	Old BIndexedCatalog API.
    virtual status_t				entryAtLocked(size_t index, uint32_t flags, Value* key, Value* entry) = 0;

    // Stub these out for ICatalog (call to BMetaDataNode implementation).
    virtual Status createNode(const ::android::String& name, sp<INode>* _aidl_return) override;
    virtual Status createDatum(const ::android::String& name, int32_t flags, sp<IDatum>* _aidl_return) override;

protected:
    virtual ~IndexedCatalog();

//    //! XXX do not use these!  Use BIndexedIterable::UpdateIteratorIndicesLocked() instead!!
//    void EntryAddedAt(uint32_t index);
//    void EntryRemovedAt(uint32_t index);
};

class MetaDataCatalog : public IndexedCatalog
{
public:
    MetaDataCatalog(const Context& context);

    // Implement BGenericNode attributes.
    virtual status_t			lookupMetaEntry(const String& entry, uint32_t flags, Value* node);
    virtual	status_t			createMetaEntry(const String& name, const Value& initialValue, sp<IDatum>* outDatum = NULL);
    virtual	status_t			removeMetaEntry(const String& name);
    virtual	status_t			renameMetaEntry(const String& old_name, const String& new_name);
    virtual	status_t			metaEntryAtLocked(ssize_t index, uint32_t flags, Value* key, Value* entry);
    virtual	size_t				countMetaEntriesLocked() const;

protected:
    virtual ~MetaDataCatalog();

private:
    KeyedVector<String, Value>*	m_attrs;
};

class Catalog : public MetaDataCatalog//, public SDatumGeneratorInt
{
public:
    Catalog();
    Catalog(const Context& context);

    virtual void onFirstRef() override;

    //!	Tie together the locks.
    virtual	lock_status_t			lock() const override;
    virtual	void					unlock() const override;

    virtual Status addEntry(const ::android::String& name, const Value& entry) override;
    virtual Status removeEntry(const ::android::String& name) override;
    virtual Status renameEntry(const ::android::String& old_name, const ::android::String& new_name) override;

    virtual Status createNode(const ::android::String& name, sp<INode>* _aidl_return) override;
    virtual Status createDatum(const ::android::String& name, int32_t flags, sp<IDatum>* _aidl_return) override;

    //! Return the entry at the given index.
    virtual status_t entryAtLocked(size_t index, uint32_t flags, Value* key, Value* entry) override;
    //! Return the number of entries in this catalog
    virtual size_t	countEntriesLocked() const override;
    //! Lookup an entry in this catalog.
    virtual status_t lookupEntry(const String& entry, uint32_t flags, Value* node) override;
    //!	Return the current value in the catalog.
    virtual	Value valueAtLocked(size_t index) const;
    //!	Change a value in the catalog.
    virtual	status_t storeValueAtLocked(size_t index, const Value& value);

    virtual	ssize_t addEntryLocked(const String& name, const Value& entry, sp<IBinder>* outEntry, bool* replaced);

protected:
    //! You can override this to create something other than a BCatalog.
    virtual sp<INode> instantiateNodeLocked(const String &name, status_t *err);

    //! these are hook functions that get called when things happen
    virtual void OnEntryCreated(const String &name, const sp<IBinder> &entry);
    virtual void OnEntryModified(const String &name, const sp<IBinder> &entry);
    virtual void OnEntryRenamed(const String &old_name, const String &new_name, const sp<IBinder> &entry);
    virtual void OnEntryRemoved(const String &name);

    virtual ~Catalog();

private:
    bool has_entry_l(const String& name);

    KeyedVector<String, Value> m_entries;
};

/*!	@} */

} } // namespace os::support

#endif // _SUPPORT_CATALOG_H

