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

#ifndef INTERFACEREC_H
#define INTERFACEREC_H

#include <utils/RefBase.h>
#include "idlc.h"

using namespace android;

enum dcltype {
    FWD, IMP, DCL
};

// Attributes of a named item in the interface,
// The parser is in charge of assuring that only the applicable
// attributes are applied on that item.  (For example, [in] doesn't
// apply to methods or interfaces.
enum AttributeKind {
    kNoAttributes	= 0,	// default to anything
    kIn				= 0x00000001,	// parameter
    kOut			= 0x00000002,	// parameter
    kInOut			= 0x00000004,	// parameter
    kReadOnly		= 0x00000008,	// property
    kWriteOnly		= 0x00000010,	// property
    kWeak			= 0x00000020,	// parameter or property
    kOptional		= 0x00000040,	// parameter
    kLocal			= 0x00000080,	// method or interface
    kReserved		= 0x00000100,	// method or property
    kAutoMarshal	= 0x00000200	// type: use autobinder marshalling
};
const uint32_t kDirectionMask = kIn + kOut + kInOut;

class InterfaceRec;

// jnamedtype & jmember - goes away when we can describe types in idl
class jnamedtype : public RefBase
{
    public:
                                jnamedtype();
                                jnamedtype(String id, String type);
                                jnamedtype(const sp<const jnamedtype>& orig);

          String				m_id;
          String				m_type;
};

// used in IDLType for storing members & member functions
// if used to storing members, member type=m_returnType and member name=m_id
class jmember : public RefBase
{
    public:
                                            jmember();
                                            jmember(String id, String rtype);
                                            jmember(const sp<const jmember>& orig);

            status_t						AddParam(String id, String type);
            int32_t							CountParams();
            sp<jnamedtype>				ParamAt(int32_t i);
            String							ID();
            String							ReturnType();

    private:
            String							m_id;
            String							m_returnType;
            Vector<sp<jnamedtype> >		m_params;
};

class IDLCommentBlock : public RefBase
{
    public:
                                            IDLCommentBlock();
        void								AddComment(const String& comment);
        void								AppendToComment(const String& more);

        void								Output(TextOutput &stream, bool startWithTab = true) const;

    private:
        Vector<String>                      m_comments;
};

// IDL classes
class IDLType : public RefBase
{
    public:

                                            IDLType();
                                            IDLType(String code);
                                            IDLType(String name, uint32_t code);
                                            IDLType(String name, uint32_t code, uint32_t attr);
                                            IDLType(const sp<const IDLType>& orig);
                                            //IDLType& operator = (const IDLType& orig);

            status_t						SetCode(uint32_t code);
            status_t						SetName(String name);
            status_t						SetPrimitiveName(String name);		// for typedefs
            status_t						SetAttribute(AttributeKind attr);
            status_t						AddAttributes(uint32_t attributes);
            status_t						SetAttributes(uint32_t attributes);
            status_t						SetIface(String typeiface);
            status_t						AddMember(sp<jmember> typemem);
            uint32_t						GetCode() const;
            String							GetName() const;
            bool							HasAttribute(AttributeKind attr) const;
            uint32_t						GetAttributes() const;
            uint32_t						GetDirection() const;
            String							GetPrimitiveName() const;
            String							GetIface() const;
            sp<jmember>					GetMemberAt(int32_t i) const;
            int32_t							CountMembers();

    private:
            uint32_t						m_code;
            String							m_name;
            uint32_t						m_attributes;
            String							m_iface;
            String							m_primitive;
            Vector<sp<jmember> >			typemembers;

};

class IDLNameType : public IDLType
{
    public:

                                        IDLNameType();
                                        IDLNameType(String id, sp<IDLType> typeptr, const sp<IDLCommentBlock>& comment, bool custom =false);
                                        IDLNameType(const sp<const IDLNameType>& orig);

            void						OutputComment(TextOutput &stream, bool startWithTab = true);
            bool						HasComment() const;

            String						m_id;
            sp<IDLType>				m_type;
            sp<IDLCommentBlock>		m_comment;
            bool						m_custom;
            ssize_t						m_index;
};


// IDLTypeScopes are temporary holders of types/names
// They are used to build IDLMethods or IDLNameTypes when
// a full production is realized.
// It can hold single items (such as a global type name)
// or it can hold multple (such as a parameter list)
class IDLTypeScope : public IDLNameType
{
    public:

                                        IDLTypeScope();
                                        IDLTypeScope(String id, const sp<IDLCommentBlock>& comment);
                                        IDLTypeScope(const sp<const IDLTypeScope>& orig);

            status_t					AddParam(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment);
            int32_t						CountParams();
            sp<IDLNameType>			ParamAt(int32_t i);
            String						ID();
            status_t					SetAutoBinder(bool t);
            bool						AutoBinder();

private:
            bool						m_autobinder;
            Vector<sp<IDLNameType> >	m_params;
};


class IDLMethod : public IDLNameType
{
    public:

                                            IDLMethod();
                                            IDLMethod(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment, bool isconst=false);
                                            IDLMethod(const sp<const IDLMethod>& orig, bool isconst=false);

            status_t						AddParam(String id, const sp<IDLType>& typeptr, const sp<IDLCommentBlock>& comment);
            int32_t							CountParams();
            sp<IDLNameType>				ParamAt(int32_t i);
            String							ID();
            sp<IDLType>					ReturnType();
            status_t						SetAutoBinder(bool t);
            bool							AutoBinder();
            status_t						SetConst(bool t);
            bool							IsConst() const;

            void							AddTrailingComment(const sp<IDLCommentBlock>& comment);
            void							OutputTrailingComment(const TextOutput &stream) const;
            bool							HasTrailingComment() const;

    private:
            bool							m_autobinder;
            bool							m_const;
            sp<IDLType>					m_returnType;
            Vector<sp<IDLNameType> >		m_params;
            sp<IDLCommentBlock>			m_trailing_comment;
};

// events are just like methods ... same syntax
typedef IDLMethod IDLEvent;
// enums/structs are packaged as methods with enumerators/members as parameters
typedef IDLMethod IDLConstruct;

class InterfaceRec
{
    public:
                                        InterfaceRec();
                                        InterfaceRec(String aId, String nspace, Vector<String> cppnspace, const sp<IDLCommentBlock>& aComment, dcltype adecl);

            String						ID() const;
            String						Namespace() const;
            Vector<String>			CppNamespace() const;

            dcltype						Declaration() const;

            String						FullInterfaceName() const;

            bool						InNamespace() const;
            String						FullClassName(const String &classPrefix) const;

            Vector<String>			Parents() const;
            bool						HasMultipleBases() const;
            String						LeftMostBase() const;

            status_t					AddCppNamespace(String cppn);
            status_t					AddProperty(String aId, const sp<IDLType>& type, const sp<IDLCommentBlock>& aComment, bool aCustom);
            status_t					AddMethod(const sp<IDLMethod>& method);
            status_t					AddEvent(const sp<IDLEvent>& event);
            status_t					AddConstruct(const sp<IDLConstruct>& construct);
            status_t					AddTypedef(String aId, const sp<IDLType>& type, const sp<IDLCommentBlock>& aComment);
            status_t					AddParent(String parent);
            status_t					SetNamespace(String nspace);
            status_t					SetDeclaration(dcltype adecl);

            int32_t						CountTypedefs();
            sp<IDLNameType>			TypedefAt(int32_t i);

            int32_t						CountProperties();
            sp<IDLNameType>			PropertyAt(int32_t i);

            int32_t			 			CountMethods();
            sp<IDLMethod>				MethodAt(int32_t i);

            int32_t						CountEvents();
            sp<IDLEvent>				EventAt(int32_t i);

            int32_t						CountConstructs();
            sp<IDLConstruct>			ConstructAt(int32_t i);

                                        // bouts to look at the whole interface - debugging only
            status_t					View();

            status_t					SetAttribute(AttributeKind attr);
            bool						HasAttribute(AttributeKind attr) const;

            void						OutputComment(const TextOutput &stream, bool startWithTab = true);

    private:

            bool						is_id_in_use(const String & id);
            bool						look_in_properties(const String & id);
            bool						look_in_methods_and_events(const String & id);


            String							m_id;
            String							m_namespace;
            Vector<String>				m_cppNamespace;
            Vector<String>				m_parents;
            sp<IDLCommentBlock>			m_comment;
            Vector<sp<IDLNameType> >		m_properties;
            Vector<sp<IDLMethod> >		m_methods;
            Vector<sp<IDLEvent> >		m_events;
            Vector<sp<IDLNameType> >		m_typedefs;
            Vector<sp<IDLConstruct> >	m_constructs;
            dcltype							m_declared;
            uint32_t						m_attributes;
};

class IncludeRec
{
public:
    IncludeRec() { }
    IncludeRec(const String& file, const sp<IDLCommentBlock>& comments = NULL) : m_file(file), m_comments(comments) { }

    inline String File() const { return m_file; }
    inline sp<IDLCommentBlock> Comments() const { return m_comments; }

private:
    String					m_file;
    sp<IDLCommentBlock>	m_comments;
};

#endif // INTERFACEREC_H
