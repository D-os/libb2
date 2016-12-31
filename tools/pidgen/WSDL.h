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

#ifndef _WSDL_H
#define _WSDL_H

#include <utils/RefBase.h>
#include <support/Locker.h>
#include <support/KeyedVector.h>
#include <utils/String.h>
#include <support/Value.h>
#include <utils/Vector.h>

#include <xml/Parser.h>

#if _SUPPORTS_NAMESPACE
using namespace palmos::storage;
using namespace palmos::support;
using namespace palmos::xml;
#endif

class SXMLTag
{
public:
	SXMLTag();
	SXMLTag(const SString& tag);
	SXMLTag(const SXMLTag& tag);

	SString Namespace();
	SString Accessor();
private:
	void split(const SString& tag);

	SString m_namespace;
	SString m_accessor;
};

class BXSDComplexContent : public SAtom
{
public:
	BXSDComplexContent();
	BXSDComplexContent(const SValue& value);

	ssize_t AddAttribute(const SValue& value);
	size_t CountAttributes() const;
	const SValue& AttributeAt(size_t index) const;

private:
	SVector<SValue> m_attributes;
};

class BXSDSimpleContent : public SAtom
{
public:
	BXSDSimpleContent();
	BXSDSimpleContent(const SValue& value);

private:
	SValue m_attributes;
};

class BXSDElement : public SAtom
{
public:
	BXSDElement();
	BXSDElement(const SValue& attributes);

	SString Name() const;
	SString Id() const;
	SString Ref() const;
	SString Type() const;
	size_t MinOccurs() const;
	size_t MaxOccurs() const;
	bool Nillable() const;
	SString SubstitutionGroup() const;
	bool Abstract() const;
	SString Final() const;
	SString Block() const;
	SString Default() const;
	SString Fixed() const;
	SString Form() const;

	const sp<ITextOutput>& operator<<(const sp<ITextOutput>& io);

private:
	SValue m_attributes;
};

class BXSDComplexType : public SAtom
{
public:
	BXSDComplexType();
	BXSDComplexType(const SValue& attributes);

	SString Name() const;
	const SValue& Attributes() const;

	size_t CountElements() const;
	size_t CountComplexContents() const;
	size_t CountSimpleContents() const;
	
	ssize_t AddElement(const sp<BXSDElement>& type);
	ssize_t AddComplexContent(const sp<BXSDComplexContent>& content);
	ssize_t AddSimpleContent(const sp<BXSDSimpleContent>& content);

	sptr<BXSDElement> ElementAt(size_t index) const;
	sptr<BXSDComplexContent> ComplexContentAt(size_t index) const;
	sptr<BXSDSimpleContent> SimpleContentAt(size_t index) const;

	void BeginSequence();
	void EndSequence();

private:
	mutable SLocker m_lock;

	SValue m_attributes;
	SString m_name;

	SVector< sp<BXSDElement> > m_elements;
	SVector< sp<BXSDComplexContent> > m_complexContents;
	SVector< sp<BXSDSimpleContent> > m_simpleContents;
	SVector<SValue> m_sequences;
};

class BXSDSimpleType : public SAtom
{
public:
	BXSDSimpleType();
	BXSDSimpleType(const SValue& attributes);

private:
	SValue m_attributes;
};

class BXMLSchema : public SAtom
{
public:
	BXMLSchema();
	
	ssize_t AddElement(const sp<BXSDElement>& type);
	ssize_t AddComplexType(const sp<BXSDComplexType>& type);
	ssize_t AddSimpleType(const sp<BXSDSimpleType>& type);

	size_t CountElements() const;
	size_t CountComplexTypes() const;
	size_t CountSimpleTypes() const;

	sptr<BXSDElement> ElementAt(size_t index) const;
	sptr<BXSDComplexType> ComplexTypeAt(size_t index) const;
	sptr<BXSDSimpleType> SimpleTypeAt(size_t index) const;
	
	const SValue& Attributes() const;
	void SetAttributes(const SValue& attrs);

	const sp<ITextOutput>& operator<<(const sp<ITextOutput>& io);

private:
	mutable SLocker m_lock;

	SVector< sp<BXSDElement> > m_elements;
	SVector< sp<BXSDComplexType> > m_complexTypes;
	SVector< sp<BXSDSimpleType> > m_simpleTypes;

	SValue m_attributes;
};

class BXMLSchemaCreator : public BCreator
{
public:
	BXMLSchemaCreator(const sp<BXMLSchema>& schema);

	virtual status_t OnStartTag(SString& name, SValue& attributes, sp<BCreator>& newCreator);
	virtual status_t OnEndTag(SString& name);
	virtual status_t OnText(SString& data);
private:
	sptr<BXMLSchema> m_schema;
};

class BWsdl : public SAtom
{
public:
	class Binding : public SLightAtom
	{
	public:
		class Operation
		{
		public:
			SString name;
			SString urn;
		};
		
		Binding(const SValue& attrs);
		
		SString Name() const;
		SString Type() const;
		SString Style() const;
		SString Transport() const;

		void SetStyle(const SString& style);
		void SetTransport(const SString& trans);

		ssize_t AddOperation(const Operation& operation);
		size_t CountOperations() const;
		BWsdl::Binding::Operation OperationAt(size_t index) const;
		
	private:
		SString m_name;
		SString m_type;
		SString m_style;
		SString m_transport;
		SVector<Operation> m_operations;
	};

	class Message : public SLightAtom
	{
	public:
		Message(const SValue& attrs);
	
		SString Name() const;
		ssize_t AddPart(const SValue& part);
		size_t CountParts() const;
		const SValue& PartAt(size_t index) const;
		
	private:
		SValue m_attributes;
		SVector<SValue> m_parts;
	};

	class PortType : public SLightAtom
	{
	public:
		class Operation
		{
		public:
			SString name;
			SString input;
			SString output;
		};
		
		PortType(const SString& name);

		SString Name() const;
		ssize_t AddOperation(const Operation& operation);
		size_t CountOperations() const;
		BWsdl::PortType::Operation OperationAt(size_t index) const;
		BWsdl::PortType::Operation OperationFor(const SString& op) const;

	private:
		SString m_name;
		SKeyedVector<SString, Operation> m_operations;
	};

	class Service : public SLightAtom
	{
	public:
		Service(const SValue& attrs);

		SString Name() const;
		ssize_t AddPort(const SValue& port);
		size_t CountPorts() const;
		const SValue& PortAt(size_t index) const;

	private:
		SValue m_attributes;
		SVector<SValue> m_ports;
	};
	
	BWsdl();

	SString Name() const;
	void SetDefinitions(const SValue& definitions);
	const SValue& Definitions() const;

	ssize_t AddBinding(const sp<BWsdl::Binding>& binding);
	ssize_t AddMessage(const sp<BWsdl::Message>& message);
	ssize_t AddPortType(const sp<BWsdl::PortType>& port);
	ssize_t AddService(const sp<BWsdl::Service>& service);

	size_t CountBindings() const;
	size_t CountMessages() const;
	size_t CountPortTypes() const;
	size_t CountServices() const;

	sptr<BWsdl::Binding> BindingAt(size_t index) const;
	sptr<BWsdl::Binding> BindingFor(const SString& binding) const;
	sptr<BWsdl::Message> MessageAt(size_t index) const;
	sptr<BWsdl::Message> MessageFor(const SString& message);
	sptr<BWsdl::PortType> PortTypeAt(size_t index) const;
	sptr<BWsdl::PortType> PortTypeFor(const SString& port) const;
	sptr<BWsdl::Service> ServiceAt(size_t index) const;

	sptr<BXMLSchema> Schema() const;

	void DumpMessages();

private:
	mutable SLocker m_lock;
	
	SVector< sp<BWsdl::Service> > m_services;
	SKeyedVector<SString, sp<BWsdl::PortType> > m_portTypes;

	SKeyedVector<SString, sp<BWsdl::Binding> > m_bindings;
	SKeyedVector<SString, sp<BWsdl::Message> > m_messages;
	
	sptr<BXMLSchema> m_schema;
	SValue m_definitions;
};

class BWsdlCreator : public BCreator
{
public:
	BWsdlCreator();

	virtual status_t Parse(const SString& file, const sp<BWsdl>& wsdl);
   
	virtual status_t OnStartTag(SString& name, SValue& attributes, sp<BCreator>& newCreator);
	virtual status_t OnEndTag(SString& name);
	virtual status_t OnText(SString& data);

private:
	sptr<BWsdl> m_wsdl;
};

class BGoogle
{
public:
	BGoogle();

	void DoGoogleSearch(const SString& string);
};

const sp<ITextOutput>& operator<<(const sp<ITextOutput>& io, const sp<BXMLSchema>& schema);
const sp<ITextOutput>& operator<<(const sp<ITextOutput>& io, const sp<BXSDComplexType>& type);
const sp<ITextOutput>& operator<<(const sp<ITextOutput>& io, const sp<BXSDElement>& element);

#endif // _WSDL_H
