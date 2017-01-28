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

#include <storage/MetaDataNode.h>

#include <support/Autolock.h>

#include <time.h>

namespace os { namespace storage {

const static String kCatalogMimeType("application/vnd.palm.catalog");

// *********************************************************************************
// ** MetaDataNode ****************************************************************
// *********************************************************************************

MetaDataNode::MetaDataNode()
{
    init();
}

MetaDataNode::~MetaDataNode()
{
}

String MetaDataNode::mimeTypeLocked() const
{
    return m_mimeType;
}

status_t MetaDataNode::storeMimeTypeLocked(const String& value)
{
    m_mimeType = value;
    return OK;
}

nsecs_t MetaDataNode::creationDateLocked() const
{
    return m_creationDate;
}

status_t MetaDataNode::storeCreationDateLocked(nsecs_t value)
{
    m_creationDate = value;
    return OK;
}

nsecs_t MetaDataNode::modifiedDateLocked() const
{
    return m_modifiedDate;
}

status_t MetaDataNode::storeModifiedDateLocked(nsecs_t value)
{
    m_creationDate = value;
    return OK;
}

void MetaDataNode::touchLocked()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);


    setModifiedDateLocked(((nsecs_t)t.tv_sec * B_ONE_SECOND) + t.tv_nsec);
}

void MetaDataNode::init()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);

    m_creationDate = m_modifiedDate = ((nsecs_t)t.tv_sec * B_ONE_SECOND) + t.tv_nsec;
}

// =================================================================================
// =================================================================================

} } // namespace os::storage
