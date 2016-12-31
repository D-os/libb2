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

#ifndef TYPEBANK_H
#define TYPEBANK_H

#include "idlc.h"
#include "InterfaceRec.h"
//extern KeyedVector<String, sp<IDLType> > TypeBank;

void cleanTypeBank(KeyedVector<String, sp<IDLType> >& tb);
status_t  initTypeBank(KeyedVector<String, sp<IDLType> > &tb);
void checktb (KeyedVector<String, sp<IDLType> > tb);

const KeyedVector<String, sp<IDLType> >& getTypeBank();

#endif // TYPEBANK_H
