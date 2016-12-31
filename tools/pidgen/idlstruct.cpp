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

#include "idlc.h"
#include "idlstruct.h"

#include <binder/TextOutput.h>

//#if _SUPPORTS_NAMESPACE
//using namespace palmos::support;
//using namespace palmos::storage;
//#endif

IDLStruct::IDLStruct()
{
    p_state.parseImport=false;
}

IDLStruct::IDLStruct(const Vector<InterfaceRec>& interfaces,
            const Vector<IncludeRec>& includes,
            const KeyedVector<String, IDLType*>& typebank,
            const KeyedVector<String, StackNode>& symtable)
        : p_includes(includes), p_interfaces(interfaces),
        p_namespaces(String())
{
    p_state.parseImport=false;

    for (uint32_t index=0; index<typebank.size(); index++) {
        p_typebank.add(typebank.keyAt(index), typebank.valueAt(index));
    }
    p_symboltable = symtable;
}

IDLStruct::~IDLStruct(void)
{
    p_includes.clear();
    p_interfaces.clear();
    p_typebank.clear();
    p_symboltable.clear();
}

status_t
IDLStruct::SetHeader(String headerdir)
{
    p_header=headerdir;
    return OK;
}

status_t
IDLStruct::AddIncludes(Vector<IncludeRec> includes)
{
    p_includes=includes;
    return OK;
}

status_t
IDLStruct::SetInterfaces(Vector<InterfaceRec> interfaces)
{
    p_interfaces=interfaces;
    #ifdef STRUCTDEBUG
        bout << "<----- idlstruct.cpp -----> there are now "
            << p_interfaces.CountItems() << " interfaces in IDLStruct\n";

        for (size_t index=0 ; index<interfaces.CountItems() ; index++ ) {
            bout << "<----- idlstruct.cpp ----->		"
                <<p_interfaces.ItemAt(index).ID() << " added\n";
        }
    #endif
    return OK;
}

status_t
IDLStruct::AddImportDir(String importdir)
{
    p_importdir.add(importdir);
    return OK;
}

status_t
IDLStruct::SetParserState(bool isimport)
{
    p_state.parseImport=isimport;
    return OK;
}

status_t
IDLStruct::SetNamesOnStack(String namespaces)
{
    p_namespaces=namespaces;
    return OK;
}

status_t
IDLStruct::AddScopeTable(StackNode scopetable)
{
    p_symboltable.add(scopetable.getScopeName(), scopetable);
    #ifdef STRUCTDEBUG
        bout << "<----- idlstruct.cpp -----> there are now "
            << p_symboltable.CountItems() << " nodes in the Symbol Table\n";

        bout << "<----- idlstruct.cpp ----->		"
                << scopetable.getScopeName() << " added\n";
    #endif
    return OK;
}

status_t
IDLStruct::SetBeginComments(const sp<IDLCommentBlock>& comments)
{
    if (!p_state.parseImport && p_begincomments == NULL)
        p_begincomments = comments;
    return OK;
}

status_t
IDLStruct::SetEndComments(const sp<IDLCommentBlock>& comments)
{
    p_endcomments = comments;
    return OK;
}

status_t
IDLStruct::AddUserType(const sp<IDLType>& utype)
{
    p_typebank.add(utype->GetName(), utype);

    #ifdef STRUCTDEBUG
        bout << "<----- idlstruct.cpp -----> type "
            << utype->GetName() << " just added" << endl;
    #endif

    return OK;
}

const Vector<String>&
IDLStruct::ImportDir()
{
    return p_importdir;
}

String
IDLStruct::Header()
{
    return p_header;
}

const Vector<IncludeRec>&
IDLStruct::Includes()
{
    return p_includes;
}

const Vector<InterfaceRec>&
IDLStruct::Interfaces()
{
    return p_interfaces;
}

pState
IDLStruct::ParserState()
{
    return p_state;
}


String
IDLStruct::NamesOnStack()
{
    return p_namespaces;
}


const KeyedVector<String, sp<IDLType> >&
IDLStruct::TypeBank()
{
    return p_typebank;
}

const KeyedVector<String, StackNode>&
IDLStruct::SymbolTable()
{
    return p_symboltable;
}

const sp<IDLCommentBlock>&
IDLStruct::BeginComments()
{
    return p_begincomments;
}

const sp<IDLCommentBlock>&
IDLStruct::EndComments()
{
    return p_endcomments;
}

/*
IDLSymbol*
IDLStruct::FindSymbol(IDLSymbol sym)
{
    p_symboltable.CountItems();
    if (p_symboltable.CountItems()<=0){
        berr << "<----- idlstruct.cpp -----> " << sym.GetId() << " cannot be found : Symbol Table is empty" << endl;
        _exit(1);
    }
    else {
        uint32_t index=0;
        IDLSymbol* result=NULL;

        while ((index<p_symboltable.CountItems()) && (result==NULL)) {
            StackNode temp=p_symboltable.ValueAt(index);
            IDLSymbol*	result=temp.lookup(sym, false);
            index++;
        }

        if (result==NULL) {
            berr << "<----- idlstruct.cpp -----> " << sym.GetId() <<" cannot be found" << endl;
            _exit(1);
        }
        else { return result;}
    }
} */


StackNode*
IDLStruct::FindScope(String scopename)
{
        bool present=p_symboltable.indexOfKey(scopename) >= 0;

        if (present) {
            return (&p_symboltable.editValueFor(scopename));
        }
        else {
            aout << "<----- idlstruct.cpp ----->  " << scopename << " is not present in the Symbol Table" << endl;
            return NULL;
        }
}

sp<IDLType> IDLStruct::FindIDLType(const sp<IDLType>& typeptr, tbaction insert)
{
    // callers must be prepared for the potential of a NULL return on LOOK
    String code=typeptr->GetName();
    bool present=p_typebank.indexOfKey(code) >= 0;

    if ((insert==ADD) || (insert==EDIT)) {
        if (present) {
            if (insert==EDIT) {
                ssize_t r=p_typebank.removeItem(code);
                if (r>=OK) {
                    AddUserType(typeptr);
                    return p_typebank.editValueFor(code);
                }
                else {
                    aout << "<----- idlstruct.cpp -----> " << code << " edit failed in TypeBank" << endl;
                    _exit(1);
                }
            }
            else {
                aout << "<----- idlstruct.cpp -----> " << code << " is already present in TypeBank" << endl;
                _exit(1);
            }
        }
        else {
            //bout << "<----- idlstruct.cpp -----> " << code << " about to be added to TypeBank" << endl;
            AddUserType(typeptr);
            return p_typebank.editValueFor(code);
        }
    }
    else if ((insert==LOOK)) {
        if (present) {
            return p_typebank.editValueFor(code);
        }
        else {
            // bout << "<----- idlstruct.cpp ----->  " << code << " could not be found in TypeBank" << endl;
            return NULL;
        }
    }
    return NULL;
}
