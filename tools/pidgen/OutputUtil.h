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

#ifndef OUTPUTUTIL_H
#define OUTPUTUTIL_H

#include <utils/String.h>
#include <utils/SortedVector.h>
#include "InterfaceRec.h"
#include "AST.h"

using namespace android;

extern const char* gNativePrefix;
extern const char* gProxyPrefix;

status_t HeaderGuard(const String &in, String &out, bool system);

//returns appropriate type m_name for C++; may have TypeToJavaType, TypeToxxxType, etc later

// InterfaceRec can be NULL when we are within the class definition itself.
// (use kInsideClassScope)
// For all other uses, pass in owning class so that typedefs can be qualified appropriately
extern const InterfaceRec* kInsideClassScope;
String TypeToCPPType(const InterfaceRec* rec, const sp<IDLType>& obj, bool arg);

//returns correct default value for the given type.
String TypeToDefaultValue(const InterfaceRec* rec, const sp<IDLType>& obj);

//type->SValue

String ToSValueConversion(const sp<IDLType>& type, const String &variable);
// Use FromValueExpression instead of directly calling FromSValueConversion
// String FromSValueConversion(const sp<IDLType>& type, const String &variable, bool setError);

String IndexToSValue(int32_t index);

enum ExpressionKind {
    INITIALIZE,				// BClass v = BClass(value, &err);
    ASSIGN,					// arg = BClass(value, &err);
    INDIRECT_ASSIGN,		// *arg = BClass(value, &err);
    RETURN					// return BClass(value, &err);
};

String FromSValueExpression(const InterfaceRec* rec,
                             const sp<IDLType>& type,
                             const String& varname,
                             const String& valuename,
                             ExpressionKind kind,
                             bool setError);

void AddFromSValueStatements(const InterfaceRec* rec,
                             sp<StatementList> statementList,
                             const sp<IDLType>& vartype,
                             const String& varname,
                             const String& valuename);

bool IsAutoMarshalType(const sp<IDLType> &type);
bool IsAutobinderType(const sp<IDLType> &type);

int32_t CountStringTabs(const String& str, int32_t tabLen=4);
const char* PadString(const String& str, int32_t fieldTabs, int32_t tabLen=4);

void CollectParents(InterfaceRec* rec, const Vector<InterfaceRec *> &classes, Vector<InterfaceRec *> *out);

struct NamespaceGenerator
{
public:
    NamespaceGenerator();
    ~NamespaceGenerator();

    void EnterNamespace(TextOutput &stream, const String& newNS, const Vector<String>& newUsing);

    void CloseNamespace(TextOutput &steam);

private:
    String prevNamespace;
    SortedVector<String> prevUsing;
    size_t namespaceDepth;
    bool first;
};

#endif // OUTPUTUTIL_H
