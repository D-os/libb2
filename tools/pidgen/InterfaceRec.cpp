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

#include "InterfaceRec.h"
#include <binder/TextOutput.h>
#include <ctype.h>

jnamedtype::jnamedtype()
    : RefBase()
{
}

jnamedtype::jnamedtype(String id, String type)
    : RefBase(), m_id(id), m_type(type)
{
}

jnamedtype::jnamedtype(const sp<const jnamedtype>& orig)
    : RefBase(),
    m_id(orig->m_id),
    m_type(orig->m_type)
{
}

jmember::jmember()
    : RefBase()
{
}

jmember::jmember(String id, String rtype)
    : RefBase(), m_id(id), m_returnType(rtype)
{
}

jmember::jmember(const sp<const jmember>& orig)
    : RefBase(),
    m_id(orig->m_id),
    m_returnType(orig->m_returnType),
    m_params(orig->m_params)
{
}

status_t
jmember::AddParam(String id, String type)
{
    int32_t i, count = CountParams();
    for (i=0; i<count; i++) {
        if (ParamAt(i)->m_id == id)
            return ALREADY_EXISTS;
    }

    sp<jnamedtype> j=new jnamedtype(id, type);
    m_params.add(j);

    return OK;
}

int32_t
jmember::CountParams()
{
    return m_params.size();
}

sp<jnamedtype>
jmember::ParamAt(int32_t i)
{
    return m_params.itemAt(i);
}

String
jmember::ID()
{
    return m_id;
}

String
jmember::ReturnType()
{
    return m_returnType;
}


IDLType::IDLType()
    : RefBase()
{
    m_code=BAD_TYPE;
    m_attributes = kNoAttributes;
}

IDLType::IDLType(String name)
    : RefBase(), m_code(BAD_TYPE), m_name(name), m_attributes(kNoAttributes)
{
    #ifdef TYPEDEBUG
        bout << "<----- interfacerec.cpp -----> IDLType Constructor called" << endl;
        bout << "<----- interfacerec.cpp ----->		m_name=" << m_name << endl;
    #endif
}

IDLType::IDLType(String name, uint32_t code)
    : RefBase(), m_code(code), m_name(name), m_attributes(kNoAttributes)
{
    #ifdef TYPEDEBUG
        bout << "<----- interfacerec.cpp -----> IDLType Constructor called" << endl;
        bout << "<----- interfacerec.cpp ----->		m_name=" << name << endl;
        bout << "<----- interfacerec.cpp ----->		m_code=" << code << endl;
    #endif
}

IDLType::IDLType(String name, uint32_t code, uint32_t attr)
    : RefBase(), m_code(code), m_name(name), m_attributes(attr)
{
    #ifdef TYPEDEBUG
        bout << "<----- interfacerec.cpp -----> IDLType Constructor called" << endl;
        bout << "<----- interfacerec.cpp ----->		m_name=" << m_name << endl;
        bout << "<----- interfacerec.cpp ----->		m_attributes=" << m_attributes << endl;
    #endif
}


IDLType::IDLType(const sp<const IDLType>& orig)
    : RefBase(),
    m_code(orig->m_code),
    m_name(orig->m_name),
    m_attributes(orig->m_attributes),
    m_iface(orig->m_iface),
    m_primitive(orig->m_primitive),
    typemembers(orig->typemembers)
{
}

status_t
IDLType::SetCode(uint32_t code)
{
    m_code=code;
    return OK;
}

status_t
IDLType::SetName(String name)
{
    m_name=name;
    return OK;
}

status_t
IDLType::SetAttribute(AttributeKind attr)
{
    // we have to do extra work if we get a direction attribute
    // because those are exclusive
    if ((attr & kDirectionMask) != 0) {
        m_attributes &= ~kDirectionMask;
    }
    m_attributes |= attr;
    return OK;
}

status_t
IDLType::AddAttributes(uint32_t attributes)
{
    // AddAttributes needs to be careful with the directional
    // attributes, because those are exclusive and not

    uint32_t attributeDirection = attributes & kDirectionMask;
    if (attributeDirection != 0) {
        // we have a direction attribute, so set it
        // (which will take are of handling exclusive)
        this->SetAttribute((AttributeKind)attributeDirection);
    }
    else {
        // some other attribute - just or it in
        m_attributes |= attributes;
    }

    return OK;
}

status_t
IDLType::SetAttributes(uint32_t attributes)
{
    m_attributes = attributes;
    return OK;
}

status_t
IDLType::SetIface(String typeiface)
{
    m_iface=typeiface;
    return OK;
}

status_t
IDLType::SetPrimitiveName(String name)
{
    m_primitive = name;
    return OK;
}

status_t
IDLType::AddMember(sp<jmember> typemem)
{
    typemembers.add(typemem);
    return OK;
}

uint32_t
IDLType::GetCode() const
{
    return m_code;
}

String
IDLType::GetName() const
{
    return m_name;
}

bool
IDLType::HasAttribute(AttributeKind attr) const
{
    // I don't just do != 0 because in+out==inout
    // so I don't want to get a false positive when
    // looking for inout
    return ((m_attributes & attr) == attr);
}

uint32_t
IDLType::GetAttributes() const
{
    return m_attributes;
}

uint32_t
IDLType::GetDirection() const
{
    return (m_attributes & kDirectionMask);
}

String
IDLType::GetIface() const
{
    return m_iface;
}

String
IDLType::GetPrimitiveName() const
{
    return m_primitive;
}

sp<jmember>
IDLType::GetMemberAt(int32_t i) const
{
    return typemembers.itemAt(i);
}

int32_t
IDLType::CountMembers()
{
    return typemembers.size();
}

IDLCommentBlock::IDLCommentBlock() : m_comments()
{
}


void
IDLCommentBlock::AddComment(const String& comment)
{
    m_comments.add(comment);
}

void
IDLCommentBlock::AppendToComment(const String& more)
{
    // Get the last comment we added and append to it
    // If the user calls AppendToComment before
    // any AddComment - then just have this start
    // off the new comment
    size_t lastItem = m_comments.size();
    if (lastItem == 0) {
        m_comments.add(String());
        lastItem = 1;
    }
    lastItem -= 1;
    m_comments.editItemAt(lastItem).append(more);
}


void
IDLCommentBlock::Output(TextOutput &stream, bool startWithTab) const
{
    // Output comments in order that they were
    // added to our comment block.
    // Always start the first line with a tab, and
    // always append newline after each comment

    size_t count = m_comments.size();
    for (size_t i = 0; i < count; i++) {
        if (startWithTab == true) {
            stream << "\t";
        }
        stream << m_comments[i] << endl;
    }
}

IDLNameType::IDLNameType()
    : IDLType()
{
}

IDLNameType::IDLNameType(String id, sp<IDLType> typeptr, const sp<IDLCommentBlock>& comment, bool custom)
    : IDLType(),
    m_id(id),
    m_type(typeptr),
    m_comment(comment),
    m_custom(custom)
{
}

IDLNameType::IDLNameType(const sp<const IDLNameType>& orig)
    : IDLType(orig.get()),
    m_id(orig->m_id),
    m_type(orig->m_type),
    m_comment(orig->m_comment),
    m_custom(orig->m_custom)
{
}

void
IDLNameType::OutputComment(TextOutput &stream, bool startWithTab)
{
    if (m_comment != NULL) {
        m_comment->Output(stream, startWithTab);
    }
}

bool
IDLNameType::HasComment() const
{
    return m_comment != NULL;
}

IDLTypeScope::IDLTypeScope()
    : IDLNameType()
{
}

IDLTypeScope::IDLTypeScope(String id, const sp<IDLCommentBlock>& comment)
    : IDLNameType(id, NULL, comment), m_autobinder(false)
{
}

IDLTypeScope::IDLTypeScope(const sp<const IDLTypeScope>& orig)
    : IDLNameType(orig.ptr()),
     m_params(orig->m_params),
     m_autobinder(false)
{
}

status_t
IDLTypeScope::AddParam(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment)
{
    int32_t i, count = CountParams();
    for (i=0; i<count; i++) {
        if (ParamAt(i)->m_id == id) {
            return B_NAME_IN_USE;
        }
    }

    sp<IDLNameType> newItem= new IDLNameType(id, typeptr, comment);
    m_params.add(newItem);
    return OK;
}

int32_t
IDLTypeScope::CountParams()
{
    return m_params.size();
}

sp<IDLNameType>
IDLTypeScope::ParamAt(int32_t i)
{
    return m_params.itemAt(i);
}

String
IDLTypeScope::ID()
{
    return m_id;
}

status_t
IDLTypeScope::SetAutoBinder(bool t)
{
    m_autobinder=t;
    return OK;
}

bool
IDLTypeScope::AutoBinder()
{
    return m_autobinder;
}


IDLMethod::IDLMethod()
    : IDLNameType()
{
    m_const=false;
}

IDLMethod::IDLMethod(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment, bool isconst)
    : IDLNameType(id, NULL, comment),
    m_returnType(typeptr),
    m_autobinder(false),
    m_const(isconst)
{
}


IDLMethod::IDLMethod(const sp<const IDLMethod>& orig, bool isconst)
    : IDLNameType(orig.ptr()),
     m_returnType(orig->m_returnType),
     m_params(orig->m_params),
     m_autobinder(false),
     m_const(isconst)
{
}

status_t
IDLMethod::AddParam(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment)
{
    int32_t i, count = CountParams();
    for (i=0; i<count; i++) {
        if (ParamAt(i)->m_id == id) {
            return B_NAME_IN_USE;
        }
    }

    sp<IDLNameType> newItem= new IDLNameType(id, typeptr, comment);
    m_params.add(newItem);

    return OK;
}

int32_t
IDLMethod::CountParams()
{
    return m_params.size();
}

sp<IDLNameType>
IDLMethod::ParamAt(int32_t i)
{
    return m_params.itemAt(i);
}

String
IDLMethod::ID()
{
    return m_id;
}

sp<IDLType>
IDLMethod::ReturnType()
{
    return m_returnType;
}

status_t
IDLMethod::SetAutoBinder(bool t)
{
    m_autobinder=t;
    return OK;
}

bool
IDLMethod::AutoBinder()
{
    return m_autobinder;
}

status_t
IDLMethod::SetConst(bool t)
{
    m_const=t;
    return OK;
}

bool
IDLMethod::IsConst() const
{
    return m_const;
}

void
IDLMethod::AddTrailingComment(const sp<IDLCommentBlock>& comment)
{
    m_trailing_comment = comment;
}

void
IDLMethod::OutputTrailingComment(const TextOutput &stream) const
{
    if (m_trailing_comment != NULL) {
        m_trailing_comment->Output(stream);
    }
}

bool
IDLMethod::HasTrailingComment() const
{
    return m_trailing_comment != NULL;
}

InterfaceRec::InterfaceRec()
            : m_attributes(kNoAttributes)
{
}

InterfaceRec::InterfaceRec(String aid, String nspace, SVector<String> cppnspace, const sp<IDLCommentBlock>& aComment, dcltype adecl)
    : m_id(aid),
    m_namespace(nspace),
    m_cppNamespace(cppnspace),
    m_comment(aComment),
    m_declared(adecl),
    m_attributes(kNoAttributes)
{
}

String
InterfaceRec::ID() const
{
    return m_id;
}

String
InterfaceRec::Namespace()  const
{
    return m_namespace;
}

SVector<String>
InterfaceRec::CppNamespace()  const
{
    return m_cppNamespace;
}

dcltype
InterfaceRec::Declaration() const
{
    return m_declared;
}

String
InterfaceRec::FullInterfaceName() const
{
    String s(m_namespace);

    s.ReplaceAll("::", ".");
    s.Append('.', 1);
    s.Append(m_id);
    return s;
}

bool
InterfaceRec::InNamespace() const
{
    return m_namespace.Length() > 0;
}

String
InterfaceRec::FullClassName(const String &classPrefix) const
{
/*
    if (m_cppNamespace.Length() == 0) return m_id;
    String s(m_cppNamespace);
    s.Append(':', 2);
    s.Append(classPrefix);
    s.Append(m_id);
    return s;
*/
    if (m_namespace.Length() == 0) {
        return m_id;
    }
    else {
        String noid=m_id;
        noid.RemoveFirst("I");

        String s("BNS(::");
        s.Append(m_namespace);
        s.Append("::) ");
        s.Append(classPrefix);
        s.Append(noid);
        return s;
    }
}

SVector<String>
InterfaceRec::Parents() const
{
    return m_parents;
}

bool
InterfaceRec::HasMultipleBases() const
{
    return m_parents.size() > 1;
}

String
InterfaceRec::LeftMostBase() const
{
    // in the case of single (or no) inheritance
    // LeftMostBase returns the class name itself
    String left_most_base;
    if (this->HasMultipleBases()) {
        left_most_base = m_parents[0];
    }
    else {
        left_most_base = ID();
    }
    return left_most_base;
}

status_t
InterfaceRec::AddCppNamespace(String cppn)
{
    m_cppNamespace.add(cppn);
    return OK;
}

status_t
InterfaceRec::AddProperty(String aId, const sp<IDLType>& aType, const sp<IDLCommentBlock>& aComment, bool aCustom)
{
    if (aId.Length() <= 0) return B_BAD_VALUE;

    if (look_in_properties(aId)) {
        berr << "*** DUPLICATE PROPERTY: " << aId << endl;
        return B_NAME_IN_USE;
    }

    String n = aId;
    int32_t l = n.Length();
    String tmp;

    char * p = n.LockBuffer(l);
    *p = toupper(*p);
    n.UnlockBuffer(l);

    tmp = n;
    if (look_in_methods_and_events(tmp)) {
        berr << "*** PROPERTY ALREADY DEFINED AS METHOD: " << aId << endl;
        return B_NAME_IN_USE;
    }

    if (aType->HasAttribute(kReadOnly) == false) {
        tmp = "Set";
        tmp += n;
        if (look_in_methods_and_events(tmp)) {
            berr << "*** PROPERTY ALREADY DEFINED AS METHOD: " << aId << endl;
            return B_NAME_IN_USE;
        }
    }

    sp<IDLNameType> newproperty= new IDLNameType(aId, aType, aComment, aCustom);
    m_properties.add(newproperty);

    return OK;
}


status_t
InterfaceRec::AddMethod(const sp<IDLMethod>& method)
{
    if (is_id_in_use(method->ID())) {
        berr << "*** DUPLICATE METHOD: " << method->ID() << endl;
        return B_NAME_IN_USE;
    }
    m_methods.add(method);
    return OK;
}

status_t
InterfaceRec::AddEvent(const sp<IDLEvent>& event)
{
    if (is_id_in_use(event->ID())) {
        berr << "*** DUPLICATE EVENT: " << event->ID() << endl;
        return B_NAME_IN_USE;
    }
    m_events.add(event);
    return OK;
}

status_t
InterfaceRec::AddTypedef(String id, const sp<IDLType>& aType, const sp<IDLCommentBlock>& aComment)
{
    int32_t count = m_typedefs.size();
    for (int32_t i = 0; i < count; i++) {
        if (m_typedefs.itemAt(i)->m_id == id) {
            berr << "*** DUPLICATE TYPEDEF: " << id << endl;
            return B_NAME_IN_USE;
        }
    }

    sp<IDLNameType> newtypedef = new IDLNameType(id, aType, aComment);
    m_typedefs.add(newtypedef);
    return OK;
}

status_t
InterfaceRec::AddConstruct(const sp<IDLConstruct>& construct)
{
    m_constructs.add(construct);
    return OK;
}

status_t
InterfaceRec::AddParent(String parent)
{
    m_parents.add(parent);
    return OK;
}

status_t
InterfaceRec::SetNamespace(String nspace)
{
    m_namespace=nspace;
    return OK;
}

status_t
InterfaceRec::SetDeclaration(dcltype adecl)
{
    m_declared=adecl;
    return OK;
}

int32_t
InterfaceRec::CountProperties()
{
    return m_properties.size();
}

sp<IDLNameType>
InterfaceRec::PropertyAt(int32_t i)
{
    return m_properties.itemAt(i);
}

int32_t
InterfaceRec::CountMethods()
{
    return m_methods.size();
}

sp<IDLMethod>
InterfaceRec::MethodAt(int32_t i)
{
    return m_methods.itemAt(i);
}

int32_t
InterfaceRec::CountEvents()
{
    return m_events.size();
}

sp<IDLEvent>
InterfaceRec::EventAt(int32_t i)
{
    return m_events.itemAt(i);
}

int32_t
InterfaceRec::CountConstructs()
{
    return m_constructs.size();
}

sp<IDLConstruct>
InterfaceRec::ConstructAt(int32_t i)
{
    return m_constructs.itemAt(i);
}

int32_t
InterfaceRec::CountTypedefs()
{
    return m_typedefs.size();
}

sp<IDLNameType>
InterfaceRec::TypedefAt(int32_t i)
{
    return m_typedefs.itemAt(i);
}

void
InterfaceRec::OutputComment(const TextOutput &stream, bool startWithTab)
{
    if (m_comment != NULL) {
        m_comment->Output(stream, startWithTab);
    }
}

status_t
InterfaceRec::View()
{
    bout << "ID = " << ID() << endl;
    bout << "Namespace = " << Namespace() << endl;

    SVector<String> rents=Parents();
    for (int s=0; s<rents.size(); s++) {
        if (s=0) {
            bout << "Parents = " << endl;
        }
        bout << " - " << rents.itemAt(s) << endl;
    }

    int32_t num=CountProperties();
    bout << "# of Properties = " << num << endl;
    for (int s=0; s< num; s++) {
        sp<IDLNameType> nt=PropertyAt(s);
        bout << " - " << nt->m_id << endl;
    }

    num=CountMethods();
    bout << "# of Methods = " << num << endl;
    for (int s=0; s< num; s++) {
        sp<IDLMethod> m=MethodAt(s);
        bout << " - " << m->ID() << endl;
    }

    num=CountEvents();
    bout << "# of Events = " << num << endl;
    for (int s=0; s< CountEvents(); s++) {
        sp<IDLEvent> e=EventAt(s);
        bout << " - " << e->ID() << endl;
    }
    return OK;
}

bool
InterfaceRec::is_id_in_use(const String & id)
{
    String tmp = id;
    int32_t length = tmp.Length();
    char *p = tmp.LockBuffer(length);
    p[0] = tolower(p[0]);
    tmp.UnlockBuffer(length);

    if (look_in_properties(tmp)) {
        return true;
    }
    if (0 == id.Compare("Set", 3)) {
        if (look_in_properties(String(tmp.String() + 3))) {
            return true;
        }
    }

    return look_in_methods_and_events(id);
}

bool
InterfaceRec::look_in_properties(const String & id)
{
    int32_t i, count = CountProperties();
    for (i=0; i<count; i++) {
        if (PropertyAt(i)->m_id == id) {
            return true;
        }
    }
    return false;
}

bool
InterfaceRec::look_in_methods_and_events(const String & id)
{
    int32_t i, count = CountMethods();
    for (i=0; i<count; i++) {
        if (MethodAt(i)->ID() == id) {
            return true;
        }
    }

    count = CountEvents();
    for (i=0; i<count; i++) {
        if (EventAt(i)->ID() == id) {
            return true;
        }
    }

    return false;
}

status_t
InterfaceRec::SetAttribute(AttributeKind attr)
{
    m_attributes |= attr;
    return OK;
}

bool
InterfaceRec::HasAttribute(AttributeKind attr) const
{
    return ((m_attributes & attr) == attr);
}
