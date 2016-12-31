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

#ifndef J_IDLSTRUCT_H
#define J_IDLSTRUCT_H
// stores carrier struct for bison + lookup utilities for types/symbols/interface names

#include "idlc.h"
#include "symbolstack.h"

using namespace android;

// in anticipation of other states for the parser/scanner
struct pState {
    bool	parseImport;
};

enum tbaction {
    ADD, LOOK, EDIT
};

class IDLStruct
{
    public:
        IDLStruct(); // empty constructor
        IDLStruct(const Vector<InterfaceRec>& interfaces,
                    const Vector<IncludeRec>& includes,
                    const KeyedVector<String, IDLType*>& typebank,
                    const KeyedVector<String, StackNode>& symtable);
        IDLStruct(IDLStruct& aStruct); // copy constructor
        ~IDLStruct(); // destructor

        //InterfaceRec*					FindInterface(const String &name, KeyedVector<String, InterfaceRec*> ifbank);

        // returns NULL ptr if not present
        StackNode*							FindScope(String scopename);
        sp<IDLType>						FindIDLType(const sp<IDLType>& typeptr, tbaction insert);
        // findsymbols has no meaning for idlstruct now -> symbol insertion only valid on the stack, and no need for lookup util yet
        // IDLSymbol*						FindSymbol(IDLSymbol sym);
        status_t							SetInterfaces(Vector<InterfaceRec> interfaces);
        status_t							AddImportDir(String importdir);
        status_t							SetHeader(String headerdir);
        status_t							SetParserState(bool isimport);
        status_t							SetNamesOnStack(String namespaces);
        status_t 							AddIncludes(Vector<IncludeRec> includes);
        status_t							AddScopeTable(StackNode scopetable);
        status_t							SetBeginComments(const sp<IDLCommentBlock>& comments);
        status_t							SetEndComments(const sp<IDLCommentBlock>& comments);

        String								Header();
        const Vector<String>&				ImportDir();
        const Vector<IncludeRec>& 			Includes();
        const Vector<InterfaceRec>&		Interfaces();
        pState								ParserState();
        String								NamesOnStack();
        const KeyedVector<String, sp<IDLType> >& TypeBank();
        const KeyedVector<String, StackNode>& SymbolTable();
        const sp<IDLCommentBlock>&		BeginComments();
        const sp<IDLCommentBlock>&		EndComments();

    private:
        status_t								AddUserType(const sp<IDLType>& utype);

        String									p_header;
        Vector<String>						p_importdir;
        Vector<IncludeRec> 					p_includes;
        Vector<InterfaceRec>					p_interfaces;
        pState									p_state;
        KeyedVector<String, sp<IDLType> >	p_typebank;
        KeyedVector<String, StackNode>		p_symboltable;
        String									p_namespaces;
        sp<IDLCommentBlock>					p_begincomments;
        sp<IDLCommentBlock>					p_endcomments;

};

#endif //J_IDLSTRUCT_H
