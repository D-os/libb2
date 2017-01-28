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

#include <storage/GenericNode.h>
#include <storage/ValueDatum.h>
#include <support/Autolock.h>
#include <support/Catalog.h>

namespace os {
namespace storage {

// *********************************************************************************
// ** LOCAL DEFINITIONS ************************************************************
// *********************************************************************************

static const String kCatalogMimeType("application/vnd.palm.catalog");
static const String kMimeType("mimeType");
static const String kCreationDate("creationDate");
static const String kModifiedDate("modifiedDate");

enum {
    kMimeTypeIndex = 0,
    kCreationDateIndex,
    kModifiedDateIndex,
    kNumMetaData
};

// *********************************************************************************
// ** GenericNode *****************************************************************
// *********************************************************************************

class GenericNode::node_state : public IndexedCatalog
{
public:
    node_state(const sp<GenericNode>& data);

    virtual lock_status_t lock() const;
    virtual void          unlock() const;

    virtual status_t addEntry(const String& name, const Value& entry);
    virtual status_t removeEntry(const String& name);
    virtual status_t renameEntry(const String& entry, const String& name);

    virtual Status createNode(const ::android::String& name, sp<INode>* _aidl_return) override;
    virtual Status createDatum(const ::android::String& name, int32_t flags, sp<IDatum>* _aidl_return) override;

    //! Return the entry at the given index.
    virtual status_t entryAtLocked(size_t index, uint32_t flags, Value* key, Value* entry);
    //! Return the number of entries in this catalog
    virtual size_t countEntriesLocked() const;
    //! Lookup an entry in this catalog.
    virtual status_t lookupEntry(const String& entry, uint32_t flags, Value* node) override;

    sp<ValueDatum> getDatumLocked(size_t index, bool create = false);
    void removeDatumLocked(size_t index, ValueDatum* datum);

protected:
    virtual ~node_state();

private:
    const sp<GenericNode> m_data;
    wp<ValueDatum>        m_datums[kNumMetaData];
};

// =================================================================================
// =================================================================================
// =================================================================================

class GenericNode::meta_datum : public ValueDatum
{
public:
    meta_datum(const sp<GenericNode>& data, size_t index, const Value& val, uint32_t mode = IDatum::READ_WRITE);

    virtual void reportChangeLocked(const sp<IBinder>& editor, uint32_t changes, off_t start = -1, off_t length = -1);

    virtual lock_status_t lock() const;
    virtual void          unlock() const;

    virtual status_t storeValueTypeLocked(uint32_t type);
    virtual status_t storeSizeLocked(off_t size);
    virtual status_t storeValueLocked(const Value& value);

protected:
    virtual ~meta_datum();

private:
    const sp<GenericNode> m_data;
    const size_t          m_index;
};

// =================================================================================
// =================================================================================
// =================================================================================

GenericNode::GenericNode()
    : m_lock("GenericNode::m_lock")
{
}

GenericNode::~GenericNode()
{
}

sp<INode> GenericNode::attributes() const
{
    Autolock _l(lock());
    const sp<node_state> state = get_state_l();
    return state.get();
}

String GenericNode::mimeType() const
{
    Autolock _l(lock());
    return mimeTypeLocked();
}

void GenericNode::setMimeType(const String& value)
{
    Autolock _l(lock());
    setMimeTypeLocked(value);
}

nsecs_t GenericNode::creationDate() const
{
    Autolock _l(lock());
    return creationDateLocked();
}

void GenericNode::setCreationDate(nsecs_t value)
{
    Autolock _l(lock());
    setCreationDateLocked(value);
}

nsecs_t GenericNode::modifiedDate() const
{
    Autolock _l(lock());
    return modifiedDateLocked();
}

void GenericNode::setModifiedDate(nsecs_t value)
{
    Autolock _l(lock());
    setModifiedDateLocked(value);
}

status_t GenericNode::walk(String* path, uint32_t flags, Value* node)
{
    status_t err;

    {
        // Look up the next path element.  We are doing this
        // inside a block so that we don't have to hold a
        // reference on the temporary name here while recursing
        // into the next catalog.  (Actually, it would be nice
        // to completely remove this temporary...)
        String name = path->walkPath(path);
        const bool atLeaf = path->length() == 0;

        // check to see if this this part of the path begins with a ":"
        if (name.ByteAt(0) == ':') {
            // XXX OPTIMIZE: Don't go through attributes() node
            // for the simple ":blah" construct.
            const sp<INode> attrs = attributes();
            err = NAME_NOT_FOUND;
            if (attrs != NULL) {
                if (name.length() == 1) {
                    // it was just the ":"
                    *node = IInterface::asBinder(attrs);
                    err = OK;
                }
                else {
                    name.Remove(0, 1);
                    // we need to walk the attributes catalog
                    err = attrs->walk(&name, flags, node);
                }
            }
        }
        else {
            err = lookupEntry(name, atLeaf ? flags : (flags & ~(REQUEST_DATA | COLLAPSE_NODE)), node);

            if (err == NAME_NOT_FOUND) {
                // Before we lose the name...  if this entry didn't
                // exist, we need to create it.
                // XXX This code should be moved to another function
                // to reduce instruction cache line usage here.

                if ((flags & INode::CREATE_MASK)) {
                    if ((flags & INode::CREATE_DATUM) && atLeaf) {
                        sp<IDatum> newdatum = createDatum(&name, flags, &err);

                        // someone could have created the entry before we could
                        // so just return that entry.
                        if (err == ALREADY_EXISTS) {
                            err = lookupEntry(name, flags, node);
                        }
                        else if (err == OK) {
                            *node = Value(IInterface::asBinder(newdatum));
                        }
                    }
                    else {
                        sp<INode> newnode = createNode(&name, &err);

                        if (err == ALREADY_EXISTS) {
                            err = lookupEntry(name, flags & ~(REQUEST_DATA | COLLAPSE_NODE), node);
                        }
                        else if (err == OK) {
                            *node = Value(IInterface::asBinder(newnode));
                        }
                    }
                }
                else {
                    err = NAME_NOT_FOUND;
                }
            }
        }
    }

    if (path->length() > 0) {
        sp<INode> inode = interface_cast<INode>(node->as<sp<IBinder>>());
        if (inode == NULL) {
            // We haven't completed traversing the path,
            // but we have reached the end of the namespace.
            // That would be an error.
            err = NAME_NOT_FOUND;
        }
        else {
            // only walk to the catalog if the path is not empty
            // and if the next catalog to walk is not remote.
            // if it is remote return the catalog since we should not
            // cross process boundries.

            sp<IBinder> binder = inode->AsBinder();

            // none shall pass...the process boundary. just return
            // what we found from lookup if the binder is remote.
            if (binder->RemoteBinder() == NULL) {
                return inode->walk(path, flags, node);
            }
        }
    }

    return err;
}

status_t GenericNode::setMimeTypeLocked(const String& value)
{
    if (value != mimeTypeLocked()) PushMimeType(value);
    const status_t err = StoreMimeTypeLocked(value);
    sp<node_state> state = m_state.promote();
    if (state == NULL) return err;
    sp<ValueDatum> datum = state->GetDatumLocked(kMimeTypeIndex);
    datum->StoreValueLocked(Value::String(value));
    return err;
}

status_t GenericNode::setCreationDateLocked(nsecs_t value)
{
    if (value != creationDateLocked()) PushCreationDate(value);
    const status_t err = StoreCreationDateLocked(value);
    sp<node_state> state = m_state.promote();
    if (state == NULL) return err;
    sp<ValueDatum> datum = state->GetDatumLocked(kCreationDateIndex);
    if (datum == NULL) return err;
    datum->StoreValueLocked(Value::Time(value));
    return err;
}

status_t GenericNode::setModifiedDateLocked(nsecs_t value)
{
    if (value != modifiedDateLocked()) PushModifiedDate(value);
    const status_t err = StoreModifiedDateLocked(value);
    sp<node_state> state = m_state.promote();
    if (state == NULL) return err;
    sp<ValueDatum> datum = state->GetDatumLocked(kModifiedDateIndex);
    if (datum == NULL) return err;
    datum->StoreValueLocked(Value::Time(value));
    return err;
}

sp<INode> GenericNode::CreateNode(String* /*name*/, status_t* err)
{
    *err = B_UNSUPPORTED;
    return NULL;
}

sp<IDatum> GenericNode::CreateDatum(String* /*name*/, uint32_t /*flags*/, status_t* err)
{
    *err = B_UNSUPPORTED;
    return NULL;
}

String GenericNode::MimeTypeLocked() const
{
    return String(kCatalogMimeType);
}

status_t GenericNode::StoreMimeTypeLocked(const String& /*value*/)
{
    return B_UNSUPPORTED;
}

nsecs_t GenericNode::CreationDateLocked() const
{
    return 0;
}

status_t GenericNode::StoreCreationDateLocked(nsecs_t /*value*/)
{
    return B_UNSUPPORTED;
}

nsecs_t GenericNode::ModifiedDateLocked() const
{
    return 0;
}

status_t GenericNode::StoreModifiedDateLocked(nsecs_t /*value*/)
{
    return B_UNSUPPORTED;
}

static ssize_t meta_name_to_index(const String& name)
{
    if (name == kMimeType) {
        return kMimeTypeIndex;
    }
    if (name == kCreationDate) {
        return kCreationDateIndex;
    }
    if (name == kModifiedDate) {
        return kModifiedDateIndex;
    }
    return B_ERROR;
}

status_t GenericNode::LookupMetaEntry(const String& entry, uint32_t flags, Value* node)
{
    ssize_t i = meta_name_to_index(entry);
    if (i >= 0) {
        Autolock _l(lock());
        Value dummy;
        return MetaEntryAtLocked(-1 - i, flags, &dummy, node);
    }
    return NAME_NOT_FOUND;
}

status_t GenericNode::CreateMetaEntry(const String& name, const Value& initialValue, sp<IDatum>* outDatum)
{
    ssize_t i = meta_name_to_index(name);
    return i >= 0 ? ALREADY_EXISTS : B_UNSUPPORTED;
}

status_t GenericNode::RemoveMetaEntry(const String& name)
{
    ssize_t i = meta_name_to_index(name);
    return i >= 0 ? B_NOT_ALLOWED : B_UNSUPPORTED;
}

status_t GenericNode::RenameMetaEntry(const String& old_name, const String& new_name)
{
    ssize_t i = meta_name_to_index(old_name);
    if (i >= 0) return B_NOT_ALLOWED;
    i = meta_name_to_index(new_name);
    return i >= 0 ? ALREADY_EXISTS : B_UNSUPPORTED;
}

status_t GenericNode::MetaEntryAtLocked(ssize_t index, uint32_t flags, Value* key, Value* entry)
{
    index = -1 - index;

    if (flags & REQUEST_DATA) {
        return get_meta_value_l(index, key, entry);
    }

    // First get the name of this entry.
    status_t err = get_meta_value_l(index, key, NULL);
    if (err != OK) return B_END_OF_DATA;

    sp<node_state> state = get_state_l();
    if (state != NULL) {
        sp<ValueDatum> datum = state->GetDatumLocked(index, true);
        if (datum != NULL) {
            *entry = Value::Binder(datum->AsBinder());
            return OK;
        }
    }

    return B_NO_MEMORY;
}

size_t GenericNode::CountMetaEntriesLocked() const
{
    return 0;
}

lock_status_t GenericNode::lock() const
{
    return m_lock.lock();
}

void GenericNode::Unlock() const
{
    m_lock.Unlock();
}

sp<GenericNode::node_state> GenericNode::get_state_l() const
{
    sp<node_state> state = m_state.promote();
    if (state == NULL) {
        state = new node_state(const_cast<GenericNode*>(this));
        m_state = state;
    }
    return state;
}

status_t GenericNode::get_meta_value_l(ssize_t index, Value* key, Value* value)
{
    switch (index) {
    case kMimeTypeIndex:
        if (key) *key     = kMimeType;
        if (value) *value = Value::String(MimeTypeLocked());
        return OK;
    case kCreationDateIndex:
        if (key) *key     = kCreationDate;
        if (value) *value = Value::Time(CreationDateLocked());
        return OK;
    case kModifiedDateIndex:
        if (key) *key     = kModifiedDate;
        if (value) *value = Value::Time(ModifiedDateLocked());
        return OK;
    }
    return B_BAD_INDEX;
}

status_t GenericNode::datum_changed_l(size_t index, const Value& newValue)
{
    status_t err;
    if (index == kMimeTypeIndex) {
        String v(newValue.AString(&err));
        if (err == OK) err = setMimeTypeLocked(v);
    }
    else {
        nsecs_t v(newValue.AsTime(&err));
        if (err == OK) {
            if (index == kCreationDateIndex)
                err = setCreationDateLocked(v);
            else if (index == kModifiedDateIndex)
                err = setModifiedDateLocked(v);
        }
    }
    return err;
}

// =================================================================================
// =================================================================================
// =================================================================================

GenericNode::node_state::node_state(const sp<GenericNode>& data)
    : m_data(data)
{
}

GenericNode::node_state::~node_state()
{
    Autolock _l(m_data->lock());
    if (m_data->m_state == this) m_data->m_state = NULL;
}

lock_status_t GenericNode::node_state::lock() const
{
    return m_data->lock();
}

void GenericNode::node_state::Unlock() const
{
    m_data->Unlock();
}

status_t GenericNode::node_state::AddEntry(const String& name, const Value& entry)
{
    return m_data->CreateMetaEntry(name, entry);
}

status_t GenericNode::node_state::RemoveEntry(const String& name)
{
    return m_data->RemoveMetaEntry(name);
}

status_t GenericNode::node_state::RenameEntry(const String& old_name, const String& new_name)
{
    return m_data->RenameMetaEntry(old_name, new_name);
}

sp<INode> GenericNode::node_state::CreateNode(String* name, status_t* err)
{
    *err = B_PERMISSION_DENIED;
    return NULL;
}

sp<IDatum> GenericNode::node_state::CreateDatum(String* name, uint32_t flags, status_t* err)
{
    *err = B_UNSUPPORTED;
    return NULL;
}

status_t GenericNode::node_state::EntryAtLocked(size_t index, uint32_t flags, Value* key, Value* entry)
{
    return m_data->MetaEntryAtLocked(ssize_t(index) - kNumMetaData, flags, key, entry);
}

size_t GenericNode::node_state::CountEntriesLocked() const
{
    return m_data->CountMetaEntriesLocked() + kNumMetaData;
}

status_t GenericNode::node_state::LookupEntry(const String& entry, uint32_t flags, Value* node)
{
    return m_data->LookupMetaEntry(entry, flags, node);
}

sp<ValueDatum> GenericNode::node_state::GetDatumLocked(size_t index, bool create)
{
    sp<ValueDatum> d = m_datums[index].promote();
    if (!create) return d;
    if (d == NULL) {
        Value val;
        m_data->get_meta_value_l(index, NULL, &val);
        // XXX Need to set READ_ONLY mode!
        d = new GenericNode::meta_datum(m_data, index, val);
        m_datums[index] = d.ptr();
    }
    return d;
}

void GenericNode::node_state::RemoveDatumLocked(size_t index, ValueDatum* datum)
{
    if (m_datums[index] == datum) m_datums[index] = NULL;
}

// =================================================================================
// =================================================================================
// =================================================================================

GenericNode::meta_datum::meta_datum(const sp<GenericNode>& data, size_t index, const Value& val, uint32_t mode)
    : ValueDatum(val, mode)
    , m_data(data)
    , m_index(index)
{
}

lock_status_t GenericNode::meta_datum::lock() const
{
    return m_data->lock();
}

void GenericNode::meta_datum::Unlock() const
{
    m_data->Unlock();
}

void GenericNode::meta_datum::ReportChangeLocked(const sp<IBinder>& editor, uint32_t changes, off_t start, off_t length)
{
    // XXX We are not dealing correctly here with type conversions, errors.
    m_data->datum_changed_l(m_index, ValueLocked());
    ValueDatum::ReportChangeLocked(editor, changes, start, length);
}

status_t GenericNode::meta_datum::StoreValueTypeLocked(uint32_t type)
{
    return B_NOT_ALLOWED;
}

status_t GenericNode::meta_datum::StoreSizeLocked(off_t size)
{
    if (ValueTypeLocked() == B_STRING_TYPE) return ValueDatum::StoreSizeLocked(size);
    return B_NOT_ALLOWED;
}

status_t GenericNode::meta_datum::StoreValueLocked(const Value& value)
{
    const uint32_t curType = ValueTypeLocked();
    if (curType != value.Type()) return B_BAD_TYPE;
    if (curType != B_STRING_TYPE && SizeLocked() != value.length()) return B_BAD_VALUE;
    return ValueDatum::StoreValueLocked(value);
}

GenericNode::meta_datum::~meta_datum()
{
    Autolock _l(m_data->lock());
    sp<node_state> state = m_data->m_state.promote();
    if (state != NULL) {
        state->RemoveDatumLocked(m_index, this);
    }
}

// =================================================================================
// =================================================================================
}
} // namespace os::storage
