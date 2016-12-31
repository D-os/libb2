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

#include "symbolstack.h"
#include "binder/TextOutput.h"

/******* stack nodes **********************/
StackNode::StackNode()
{
    scopename=String();
    parentlink=NULL;
}

StackNode::StackNode(const String newname, const String parent)
    : scopename(newname)
{
    parentlink=NULL;
}

StackNode::StackNode(const StackNode& node) // copy constructor
    : scopename(node.scopename), parentlink(node.parentlink),
        childlink(node.childlink)
{
    scopetable = node.scopetable;
}

StackNode::~StackNode()
{	//delete parentlink;
    //childlink.MakeEmpty();
    //scopetable.MakeEmpty();
}   // destructor

StackNode &
StackNode::operator = (const StackNode &node)
{
    #ifdef SYMDEBUG
        aout << "<----- symbolstack.cpp -----> stacknode assignment = called by "
            << this->getScopeName() << endl;
        aout << "<----- symbolstack.cpp ----->		about to be set to -> "
            << node.scopename << endl;
    #endif //SYMDEBUG

    scopetable = node.scopetable;
    scopename=node.scopename;
    parentlink=node.parentlink;
    childlink=node.childlink;

    return *this;
} // assignment

String
StackNode::getScopeName()
{	return scopename;}

status_t
StackNode::setScopeName(const String newname)
{	scopename=newname;
    #ifdef SYMDEBUG
        aout << "<----- symbolstack.cpp -----> attn! scopename just got reset to " << newname << endl;
        aout << "<----- symbolstack.cpp ----->		be careful! scopename should rarely be reset " << endl;
    #endif //SYMDEBUG

    return OK;
}

StackNode*
StackNode::getParentInfo()
{	return parentlink;
}

status_t
StackNode::setParentInfo(StackNode* parentptr)
{
    parentlink=parentptr;
    return OK;
    //set parent info
}

StackNode*
StackNode::getChildInfoAt(int32_t index)
{
    return childlink.itemAt(index);
}

status_t
StackNode::setChildInfo(StackNode* childptr)
{
    childlink.add(childptr);
    return OK;
    //set children info
}

int32_t
StackNode::countChildren()
{	return childlink.size();
}

IDLSymbol*
StackNode::lookup(IDLSymbol sym, bool insert)
{	bool present=scopetable.indexOfKey(sym.GetId()) >= 0;

    if (insert)
    {	if (present)
        { aerr << "----- symbolstack.cpp -----> failed to insert " << sym.GetId() <<
            "; symbol already present in" << scopename << endl;
        }
        else {
            scopetable.add(sym.GetId(), sym);
            aout << "----- symbolstack.cpp -----> " << sym.GetId() <<
                " inserted into " << scopename <<" table" << endl;
        }
        return (&scopetable.editValueFor(sym.GetId()));
    }

    else
    {	if (present)
        { aerr << "----- symbolstack.cpp -----> " << sym.GetId() <<
            " symbol present in" << scopename << endl;
            return (&scopetable.editValueFor(sym.GetId()));
        }
        else {
            scopetable.add(sym.GetId(), sym);
            aout << "----- symbolstack.cpp -----> " << sym.GetId() <<
                " not present " << scopename <<" table" << endl;
            return NULL;
        }
    }
}
/******* symbol stack **********************/

SymbolStack::SymbolStack()
{	CurrentScope=NULL;
} // empty stack

SymbolStack::~SymbolStack()
{	StackNode next;
    while (! empty())
    {	next=pop(); // delete all StackNodes on stack
    }
}

void
SymbolStack::push(const String scope)
{
        StackNode*  temp;
        StackNode*  childptr;
        KeyedVector<String, IDLSymbol>  table;

        #ifdef SYMDEBUG
            if (CurrentScope==NULL)
            { 	aout << "<----- symbolstack.cpp -----> 1st node is being pushed onto the stack - CurrentScope=NULL" << endl;
            }
            else {
                aout << "<----- symbolstack.cpp -----> node is being pushed onto the stack - CurrentScope= " <<
                CurrentScope->getScopeName() << endl;
            }
        #endif


        temp= new StackNode;

        temp->setScopeName(scope);
        temp->setParentInfo(CurrentScope);
        // give parent scope link to newborn -> parentlink=NULL if this is the first scope table

        childptr=temp;
        if (CurrentScope==NULL) {
            #ifdef SYMDEBUG
                aout << "<----- symbolstack.cpp -----> we don't have any child info to pass back since "
                    << scope << " is our first scope table " << endl;
            #endif
        }
        else {
            CurrentScope->setChildInfo(childptr);
            // give child link to parent
        }

        CurrentScope=temp;
        #ifdef SYMDEBUG
            aout << "<----- symbolstack.cpp -----> another node got pushed onto stack - CurrentScope= " <<
                CurrentScope->getScopeName() << endl;
        #endif
//	}
}

StackNode&
SymbolStack::pop()
{	if (empty())
    {	aout <<"<----- symbolstack.cpp -----> symbol stack is empty" << endl;
//		delete CurrentScope;
//		CurrentScope= NULL;
//
//		aout <<"<----- symbolstack.cpp ----->		CurrentScope deleted" << endl;
//		exit (1);
        return *(StackNode*)NULL;
    }
    else
    {
        #ifdef SYMDEBUG
            aout << "<----- symbolstack.cpp -----> symbolstack is being popped - we're popping "
                << CurrentScope->getScopeName() << endl;
            if ((CurrentScope->getParentInfo())!=NULL) {
                aout << "<----- symbolstack.cpp ----->		the next scope that is coming up is "
                << (CurrentScope->getParentInfo())->getScopeName() << endl;
            }
        #endif

        StackNode*	temp;
        temp=CurrentScope;


        #ifdef SYMDEBUG
//			aout << "<----- symbolstack.cpp -----> about to move CurrentScope " << endl;
        #endif
            CurrentScope=CurrentScope->getParentInfo();
        #ifdef SYMDEBUG
//			aout << "<----- symbolstack.cpp -----> CurrentScope has been moved to" << CurrentScope->getScopeName() << endl;
        #endif

        return *temp;
    }
}

bool SymbolStack::empty() const
{	return (CurrentScope==NULL);
}


StackNode*
SymbolStack::getCurrentScope() const
{	return (CurrentScope);
}

SymbolStack::SymbolStack(const SymbolStack& stack)
{
/*	if (stack.CurrentScope==NULL)
    {	CurrentScope=NULL;
    }
    else
    {	StackNodePtr end, temp=stack.CurrentScope;

        end=new StackNode;

         table_t=temp->scopetable;
        end->scopetable=table_t;

        #ifdef SYMDEBUG
            aout << "<----- symbolstack.cpp -----> the symbol table with CurrentScope= "
                << CurrentScope->getScopeName << " just got copied" << "\n";
        #endif

        end->setScopeName(temp->getScopeName());
        CurrentScope=end;

        temp=temp->stlink;
        while(temp!=NULL)
        {	end->stlink=new StackNode;
            end=end->stlink;

             table_t1=temp->scopetable;
            end->scopetable=table_t1;

            end->setScopeName(temp->getScopeName());
            temp=temp->stlink;
        }
        end->stlink=NULL;
    }	 */
}
