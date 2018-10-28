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

#ifndef _VALUE_PARSER_H
#define _VALUE_PARSER_H

#include <xml/XMLParser.h>
#include <support/Package.h>

#if _SUPPORTS_NAMESPACE
namespace os {
namespace xml {
#endif

// ==================================================================
class BXML2ValueCreator : public BCreator
{
public:
	BXML2ValueCreator(SValue &targetValue, const SValue &attributes, const SPackage& resources = B_NO_PACKAGE, const SValue &references = B_UNDEFINED_VALUE);

	virtual status_t	OnStartTag(				SString			& name,
												SValue			& attributes,
												sptr<BCreator>	& newCreator	);

	virtual status_t	OnEndTag(				SString			& name			);

	virtual status_t	OnText(					SString			& data			);

	virtual status_t	Done();

private:
	static status_t ParseSignedInteger (const BNS(os::support::) SString &from, int64_t maximum, int64_t *val);
	SPackage m_resources;
	BNS(os::support::) SValue &m_targetValue;
	BNS(os::support::) SValue m_key;
	BNS(os::support::) SValue m_value;
	BNS(os::support::) SString m_data;
	BNS(os::support::) SString m_dataType;
	status_t m_status;
	SValue m_references;
	type_code m_rawTypeCode;
	size_t m_rawSize;
	int32_t m_resID;
	int32_t m_strIndex;
	bool m_isReference : 1;

	int32_t m_id;
};

#if _SUPPORTS_NAMESPACE
} } // os::xml
#endif

#endif
