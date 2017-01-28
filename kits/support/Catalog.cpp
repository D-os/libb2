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

#include <support/Catalog.h>

#include <support/Autolock.h>
//#include <support/MemoryStore.h>
//#include <support/StdIO.h>
//#include <support/Looper.h>

//#include <support/IIterator.h>

namespace os { namespace support {

// =================================================================================
//status_t WalkHelper::HelperWalk(String* path, uint32_t flags, Value* node)
//{
//	status_t err;

//	{
//		// Look up the next path element.  We are doing this
//		// inside a block so that we don't have to hold a
//		// reference on the temporary name here while recursing
//		// into the next catalog.  (Actually, it would be nice
//		// to completely remove this temporary...)
//		String name;
//		path->PathRemoveRoot(&name);

//		// check to see if this this part of the path begins with a ":"
//		if (name.ByteAt(0) == ':')
//		{
//			const sp<INode> attrs = HelperAttributesCatalog();
//			err = NAME_NOT_FOUND;
//			if (attrs != NULL)
//			{
//				if (name.Length() == 1)
//				{
//					// it was just the ":"
//					*node = attrs->as<sp<IBinder>>();
//					err = OK;
//				}
//				else
//				{
//					name.Remove(0, 1);
//					// we need to walk the attributes catalog
//					err = attrs->Walk(&name, flags, node);
//				}
//			}
//		}
//		else
//		{
//			err = HelperLookupEntry(name, flags, node);

//			if (err == NAME_NOT_FOUND)
//			{
//				// Before we lose the name...  if this entry didn't
//				// exist, we need to create it.
//				// XXX This code should be moved to another function
//				// to reduce instruction cache line usage here.

//				if ((flags & INode::CREATE_MASK))
//				{
//					if ((flags & INode::CREATE_DATUM) && path->Length() == 0)
//					{
//						sp<IDatum> datum = HelperCreateDatum(&name, flags, &err);

//						// someone could have created the entry before we could
//						// so just return that entry.
//						if (err == ALREADY_EXISTS)
//						{
//							err = HelperLookupEntry(name, flags, node);
//						}
//						else if (err == OK)
//						{
//							*node = Value::Binder(datum->as<sp<IBinder>>());
//						}
//					}
//					else
//					{
//						sp<ICatalog> catalog = HelperCreateCatalog(&name, &err);

//						if (err == ALREADY_EXISTS)
//						{
//							err = HelperLookupEntry(name, flags, node);
//						}
//						else if (err == OK)
//						{
//							*node = Value::Binder(catalog->as<sp<IBinder>>());
//						}
//					}
//				}
//				else
//				{
//					err = NAME_NOT_FOUND;
//				}
//			}
//		}
//	}

//	if (path->Length() > 0)
//	{
//		sp<INode> inode = interface_cast<INode>(*node);
//		if (inode == NULL)
//		{
//			// We haven't completed traversing the path,
//			// but we have reached the end of the namespace.
//			// That would be an error.
//			err = NAME_NOT_FOUND;
//		}
//		else
//		{
//			// only walk to the catalog if the path is not empty
//			// and if the next catalog to walk is not remote.
//			// if it is remote return the catalog since we should not
//			// cross process boundries.

//			sp<IBinder> binder = inode->as<sp<IBinder>>();

//			// none shall pass...the process boundary. just return
//			// what we found from lookup if the binder is remote.
//			if (binder->RemoteBinder() == NULL)
//			{
//				return inode->Walk(path, flags, node);
//			}
//		}
//	}

//	return err;
//}

//sp<INode> SWalkHelper::HelperAttributesCatalog() const
//{
//	return NULL;
//}

//sp<ICatalog> SWalkHelper::HelperCreateCatalog(String* name, status_t* err)
//{
//	*err = B_UNSUPPORTED;
//	return NULL;
//}

//sp<IDatum> SWalkHelper::HelperCreateDatum(String* name, uint32_t flags, status_t* err)
//{
//	*err = B_UNSUPPORTED;
//	return NULL;
//}

// =================================================================================

MetaDataCatalog::MetaDataCatalog(const Context& context)
    : IndexedCatalog(context)
    , m_attrs(NULL)
{
}

MetaDataCatalog::~MetaDataCatalog()
{
    delete m_attrs;
}

status_t MetaDataCatalog::lookupMetaEntry(const String& entry, uint32_t flags, Value* node)
{
    if (MetaDataNode::lookupMetaEntry(entry, flags, node) == OK) return OK;

    Autolock _l(lock());
    if (m_attrs && m_attrs->indexOfKey(entry)) {
        // XXX Need to generate datums.
        *node = m_attrs->valueFor(entry);
        return OK;
    }

    return NAME_NOT_FOUND;
}

status_t MetaDataCatalog::createMetaEntry(const String& name, const Value& initialValue, sp<IDatum>* outDatum)
{
    status_t err = MetaDataNode::createMetaEntry(name, initialValue, outDatum);
    if (err != B_UNSUPPORTED) return err;

    Autolock _l(lock());
    if (!m_attrs) {
        m_attrs = new KeyedVector<String, Value>();
        if (!m_attrs) return NO_MEMORY;
    }
    // XXX Need to generate datums.
    return m_attrs->addItem(name, initialValue);
}

status_t MetaDataCatalog::removeMetaEntry(const String& name)
{
    status_t err = MetaDataNode::removeMetaEntry(name);
    if (err != B_UNSUPPORTED) return err;

    Autolock _l(lock());
    if (m_attrs) {
        if (m_attrs->removeItemFor(name) >= OK) return OK;
    }
    return NAME_NOT_FOUND;
}

status_t MetaDataCatalog::renameMetaEntry(const String& old_name, const String& new_name)
{
    status_t err = MetaDataNode::renameMetaEntry(old_name, new_name);
    if (err != B_UNSUPPORTED) return err;

    Autolock _l(lock());
    bool found;
    Value val(m_attrs->valueFor(old_name, &found));
    if (found) {
        if (m_attrs->indexOfKey(new_name) >= 0) return ALREADY_EXISTS;
        ssize_t err = m_attrs->addItem(new_name, val);
        if (err >= OK) {
            m_attrs->removeItemFor(old_name);
            return OK;
        }
        return err;
    }
    return NAME_NOT_FOUND;
}

status_t MetaDataCatalog::metaEntryAtLocked(ssize_t index, uint32_t flags, Value* key, Value* entry)
{
    if (index < 0) return MetaDataNode::metaEntryAtLocked(index, flags, key, entry);

    if (m_attrs == NULL || index >= (ssize_t)m_attrs->size()) return NOT_ENOUGH_DATA;
    // XXX Need to generate datums.
    *key = Value(m_attrs->keyAt(index));
    *entry = m_attrs->valueAt(index);
    return OK;
}

size_t MetaDataCatalog::countMetaEntriesLocked() const
{
    return m_attrs ? m_attrs->size() : 0;
}

// =================================================================================

Catalog::Catalog()
    :	MetaDataCatalog(Context())
{
}

Catalog::Catalog(const Context& context)
    :	MetaDataCatalog(context)
//    ,	SDatumGeneratorInt(context)
{
}

Catalog::~Catalog()
{
}

void Catalog::onFirstRef()
{
}

lock_status_t Catalog::lock() const
{
    return MetaDataCatalog::lock();
}

void Catalog::unlock() const
{
    MetaDataCatalog::unlock();
}

ssize_t Catalog::addEntryLocked(const String& name, const Value& entry, sp<IBinder>* outEntry, bool* replaced)
{
    ssize_t index = m_entries.indexOfKey(name);
    Value* val;
    if (index >= 0) {
        setValueAtLocked(index, entry);		// use this so we send out change notifications.
        val = &m_entries.editValueAt(index);
        if (replaced) *replaced = true;
    } else {
        index = m_entries.addItem(name, entry);
        if (index < 0) return index;
        updateIteratorIndicesLocked(index, 1);
//        updateDatumIndicesLocked(index, 1);
        val = &m_entries.editValueAt(index);
        if (replaced) *replaced = false;
    }

    if (outEntry) {
        *outEntry = val->as<sp<IBinder>>();
        if (*outEntry == NULL) {
            // This entry is not an object; we need to generate a datum to wrap it.
            *outEntry = datumAtLocked(index)->as<sp<IBinder>>();
        }
    }

    return index;
}

Status Catalog::addEntry(const ::android::String& name, const Value& entry)
{
    sp<IBinder> binder;

    lock();
    bool found;
    ssize_t index = addEntryLocked(name, entry, &binder, &found);
    if (index >= 0) touchLocked();
    unlock();

    //! @todo Fix this by hooking in to ReportChangeAtLocked()!!!

    status_t err = NO_MEMORY;
    if (index >= 0)
    {
        // send the modified or creation event
//        if (found)
//        {
//            OnEntryModified(name, binder);
//            if (BnNode::IsLinked()) {
//                PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//                PushEntryModified(this, name, binder);
//            }
//        }
//        else
//        {
//            OnEntryCreated(name, binder);
//            if (BnNode::IsLinked()) {
//                PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//                PushEntryCreated(this, name, binder);
//            }
//        }

        err = OK;
    }

    return err;
}

Status Catalog::removeEntry(const ::android::String& name)
{
    lock();
    ssize_t index = m_entries.removeItemFor(name);
    if (index >= 0) {
        updateIteratorIndicesLocked(index, -1);
        updateDatumIndicesLocked(index, -1);
        touchLocked();
    }
    unlock();

    status_t err = NAME_NOT_FOUND;
    if (index >= 0)
    {
        OnEntryRemoved(name);
//        if (BnNode::IsLinked()) {
//            PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//            PushEntryRemoved(this, name);
//        }
        err = OK;
    }

    return err;
}

Status Catalog::renameEntry(const ::android::String& old_name, const ::android::String& new_name)
{
    status_t err;
    sp<IBinder> binder;

    {
        Autolock _l(lock());

        bool old = has_entry_l(old_name);
        bool nuu = has_entry_l(new_name);

        // can't rename a entry that doesn't exist or rename
        // an entry that exists to another entry that exists.
        if (!old) return NAME_NOT_FOUND;
        if (nuu) return ALREADY_EXISTS;

        //! @todo Need to keep the same IDatum object.

        ssize_t pos;
        const Value val = m_entries.valueFor(old_name);
        pos = m_entries.removeItemFor(old_name);
        if (pos >= 0) {
            updateIteratorIndicesLocked(pos, -1);
            updateDatumIndicesLocked(pos, -1);
            pos = m_entries.addItem(new_name, val);
            if (pos >= 0) {
                updateIteratorIndicesLocked(pos, 1);
                updateDatumIndicesLocked(pos, 1);
                binder = val.as<sp<IBinder>>();
                if (binder == NULL) binder = datumAtLocked(pos)->as<sp<IBinder>>();
                touchLocked();
            }
        }

        err = (pos >= 0) ? OK : NO_MEMORY;
    }

    //! @todo Fix to only get the entry object if linked!!!

    // send the renamed event;
    if (err == OK)
    {
        OnEntryRenamed(old_name, new_name, binder);
//        if (BnNode::IsLinked()) {
//            PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//            PushEntryRenamed(this, old_name, new_name, binder);
//        }
    }

    return err;
}

Status Catalog::createNode(const ::android::String& name, sp<INode>* _aidl_return)
{
    sp<INode> catalog;
    status_t err = OK;

    {
        Autolock _l(lock());

        if (has_entry_l(*name))
        {
            err = ALREADY_EXISTS;
            catalog = NULL;
        }
        else
        {
            err = OK;
            catalog = instantiateNodeLocked(*name, &err);
            if (catalog == NULL && err == OK) err = NO_MEMORY;
            if (err == OK)
            {
                sp<IBinder> binder;
                bool found;
                addEntryLocked(*name, Value::Binder(catalog->as<sp<IBinder>>()), &binder, &found);
                touchLocked();
            }
        }
    }

    if (catalog != NULL)
    {
        // send the creation event
        OnEntryCreated(*name, catalog->as<sp<IBinder>>());
//        if (BnNode::IsLinked()) {
//            PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//            PushEntryCreated(this, *name, catalog->as<sp<IBinder>>());
//        }
    }

    *_aidl_return = catalog;
    return Status::fromStatusT(err);
}

sp<INode> Catalog::instantiateNodeLocked(const String &, status_t *err)
{
    return new Catalog(Context());
}

Status Catalog::createDatum(const ::android::String& name, int32_t flags, sp<IDatum>* _aidl_return)
{
    sp<IBinder> binder;
    sp<IDatum> datum;
    status_t err;

    {
        Autolock _l(lock());
        if (has_entry_l(*name))
        {
            *err = ALREADY_EXISTS;
        }
        else
        {
            bool found;
            ssize_t pos = addEntryLocked(*name, Value(B_RAW_TYPE, "", 0), &binder, &found);
            if (pos >= 0) pos = binder != NULL ? OK : NO_MEMORY;
            *err = pos;
            datum = interface_cast<IDatum>(binder);
            touchLocked();
        }
    }

    if (datum != NULL)
    {
        // send the entry created event
        OnEntryCreated(*name, datum->as<sp<IBinder>>());
//        if (BnNode::IsLinked()) {
//            PushNodeChanged(this, INode::CHANGE_DETAILS_SENT, B_UNDEFINED_VALUE);
//            PushEntryCreated(this, *name, datum->as<sp<IBinder>>());
//        }
    }

    *_aidl_return = datum;
    return Status::fromStatusT(err);
}

void Catalog::OnEntryCreated(const String &, const sp<IBinder> &)
{
}

void Catalog::OnEntryModified(const String &, const sp<IBinder> &)
{
}

void Catalog::OnEntryRenamed(const String &, const String &, const sp<IBinder> &)
{
}

void Catalog::OnEntryRemoved(const String &)
{
}


bool Catalog::has_entry_l(const String& name)
{
    if (m_entries.indexOfKey(name) >= 0)
        return true;

    return false;
}

status_t Catalog::entryAtLocked(size_t index, uint32_t flags, Value* key, Value* entry)
{
    if (index >= m_entries.size()) return NOT_ENOUGH_DATA;

//#if BUILD_TYPE == BUILD_TYPE_DEBUG
//	printf("EntryAt() flags = %p\n", flags);
//#endif

    *key = Value(m_entries.keyAt(index));
    *entry = m_entries.valueAt(index);

    if ((flags & INode::REQUEST_DATA) == 0 && !entry->IsObject())
    {
        *entry = Value(datumAtLocked(index)->as<sp<IBinder>>());
    }

    return OK;
}

size_t Catalog::countEntriesLocked() const
{
    return m_entries.size();
}

status_t Catalog::lookupEntry(const String& entry, uint32_t flags, Value* node)
{
    Autolock _l(lock());

    ssize_t index = m_entries.indexOfKey(entry);
    if (index < 0) return NAME_NOT_FOUND;

    *node = m_entries.valueAt(index);
    if ((flags & INode::REQUEST_DATA) == 0 && !node->IsObject())
    {
        *node = Value(datumAtLocked(index)->as<sp<IBinder>>());
    }

    return OK;
}

Value Catalog::valueAtLocked(size_t index) const
{
    return m_entries.valueAt(index);
}

status_t Catalog::storeValueAtLocked(size_t index, const Value& value)
{
    m_entries.editValueAt(index) = value;
    return OK;
}

// ==================================================================================

GenericCatalog::GenericCatalog()
{
}

GenericCatalog::GenericCatalog(const Context& context)
    :	/*BnCatalog(context)
    ,*/   MetaDataNode(context)
//    ,   BnIterable(context)
{
}

GenericCatalog::~GenericCatalog()
{
}

Value GenericCatalog::inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags)
{
    return Value(BnCatalog::inspect(caller, which, flags))
            .join(BnNode::inspect(caller, which, flags))
            .join(BnIterable::inspect(caller, which, flags));
}

sp<INode> GenericCatalog::createNode(String* name, status_t* err)
{
    return MetaDataNode::createNode(name, err);
}

sp<IDatum> GenericCatalog::createDatum(String* name, uint32_t flags, status_t* err)
{
    return MetaDataNode::createDatum(name, flags, err);
}

// ==================================================================================

IndexedCatalog::IndexedCatalog()
{
}

IndexedCatalog::IndexedCatalog(const Context& context)
    : /*BnCatalog(context)
    ,*/ MetaDataNode(context)
//    , BIndexedIterable(context)
{
}

IndexedCatalog::~IndexedCatalog()
{
}

Value IndexedCatalog::inspect(const sp<IBinder>& caller, const Value& which, uint32_t flags)
{
    return Value(BnCatalog::inspect(caller, which, flags))
            .join(BnNode::inspect(caller, which, flags))
            .join(BIndexedIterable::inspect(caller, which, flags));
}

lock_status_t IndexedCatalog::lock() const
{
    return MetaDataNode::lock();
}

void IndexedCatalog::unlock() const
{
    MetaDataNode::unlock();
}

status_t IndexedCatalog::entryAtLocked(const sp<IndexedIterator>& , size_t index,
    uint32_t flags, Value* key, Value* entry)
{
    return entryAtLocked(index, flags, key, entry);
}

sp<INode> IndexedCatalog::createNode(String* name, status_t* err)
{
    return MetaDataNode::createNode(name, err);
}

sp<IDatum> IndexedCatalog::createDatum(String* name, uint32_t flags, status_t* err)
{
    return MetaDataNode::createDatum(name, flags, err);
}

void IndexedCatalog::entryAddedAt(uint32_t index)
{
    Autolock _l(lock());
    updateIteratorIndicesLocked(index, 1);
}

void IndexedCatalog::entryRemovedAt(uint32_t index)
{
    Autolock _l(lock());
    updateIteratorIndicesLocked(index, -1);
}

} } // namespace os::support
