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

#ifndef J_SYMBOLSTACK_H
#define J_SYMBOLSTACK_H
#include "idlc.h"
#include "InterfaceRec.h"

using namespace android;

/******* symbols ***************************/
class IDLSymbol
{
    public:
        IDLSymbol();
        IDLSymbol(const String name);
        IDLSymbol(const String name, const sp<IDLType>& aType);
        IDLSymbol(const IDLSymbol& aSymbol); // copy constructor
        ~IDLSymbol(); // destructor

        IDLSymbol&			operator = (const IDLSymbol &);

        status_t			SetType(const sp<IDLType>& aType);
        sp<IDLType> 		GetType();
        String				GetId();

    private:
        String				m_id;
        sp<IDLType>		m_type;
};


/******* stack nodes **********************/
class StackNode
{
    public:

        StackNode(); // empty node
        StackNode(const String newname, const String parent); //
        StackNode(const StackNode& node); // copy constructor
        StackNode & operator = (const StackNode& node); // assignment
        ~StackNode(); // destructor

        String 					getScopeName();
        status_t					setScopeName(const String newname);
        StackNode*					getParentInfo();
        status_t					setParentInfo(StackNode* parentptr);
        // some way of accessing the children in the future
        StackNode*					getChildInfoAt(int32_t index);
        status_t					setChildInfo(StackNode* childptr);
        int32_t						countChildren();

        // look up name in the current node, and if it's valid insert it
        IDLSymbol*					lookup(IDLSymbol sym, bool insert);

    private:
        String 							scopename;
        StackNode* 							parentlink;
        Vector<StackNode*>					childlink;
        KeyedVector<String, IDLSymbol>	scopetable;
};


/******* symbol stack **********************/
class SymbolStack
{
    public:
        SymbolStack(); // empty stack
        ~SymbolStack(); // destroys stack

        void push(const String scope); // new Table for separate scope
        StackNode & pop();
        bool empty() const;

        //IDLSymbol* lookup(IDLSymbol* symbol, bool doinsert);
        const StackNode* scopelookup(const String scopename);

        //bool lookupInterface(const String symbolname);
        //void printstack();
        StackNode* getCurrentScope() const;

    private:
        SymbolStack(const SymbolStack& stack); // copy constructor
        //void insertsym(IDLSymbol* symbol);
        StackNode* CurrentScope;
};

static Vector<StackNode> SymTab;
#endif //J_SYMBOLSTACK_H
