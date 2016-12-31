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

#include "TypeBank.h"
#include "yacc.h"
#include <stdio.h>
#include <stdlib.h>
#include <binder/TextOutput.h>
#include <support/TypeConstants.h>

using namespace android;
using namespace palmos::support;

// to add types, update initTypeBank and scanner.l
// you may also want to update the functions in OutputUtil.cpp

void cleanTypeBank(KeyedVector<String, sp<IDLType> > &tb)
{
    tb.clear();
}

status_t
initTypeBank(KeyedVector<String, sp<IDLType> > &tb)
{

    size_t	numberoftypes;
    KeyedVector<String, uint32_t> stdtypes;
    KeyedVector<String, String>	 ToSValue;
    KeyedVector<String, String>	 FromSValue;

    // 0-4 : std types where X -> X() & var.AsX();
    stdtypes.add(String("bool"), B_BOOL_TYPE);
    stdtypes.add(String("float"), B_FLOAT_TYPE);
    stdtypes.add(String("double"), B_DOUBLE_TYPE);
    stdtypes.add(String("int32_t"), B_INT32_TYPE);
    stdtypes.add(String("int64_t"), B_INT64_TYPE);
    // 5-11 : std types where X-> Int32(var) & (x)var.AsInt32();
    stdtypes.add(String("int8_t"), B_INT8_TYPE);
    stdtypes.add(String("int16_t"), B_INT16_TYPE);
    stdtypes.add(String("uint8_t"), B_UINT8_TYPE);
    stdtypes.add(String("uint16_t"), B_UINT16_TYPE);
    stdtypes.add(String("uint32_t"), B_UINT32_TYPE);
    stdtypes.add(String("size_t"), B_SIZE_T_TYPE);
    stdtypes.add(String("char"), B_CHAR_TYPE);
    // 12-15 : binder types
    stdtypes.add(String("sptr"), B_BINDER_TYPE);
    stdtypes.add(String("wptr"), B_BINDER_WEAK_TYPE);
    // 16-27 : std types that need customization
    stdtypes.add(String("nsecs_t"), B_NSECS_TYPE);
    stdtypes.add(String("bigtime_t"), B_BIGTIME_TYPE);
    stdtypes.add(String("off_t"), B_OFF_T_TYPE);
    stdtypes.add(String("ssize_t"), B_SSIZE_T_TYPE);
    stdtypes.add(String("status_t"), B_INT32_TYPE);
    stdtypes.add(String("uint64_t"), B_UINT64_TYPE);
    stdtypes.add(String("wchar32_t"), B_WCHAR_TYPE);
    stdtypes.add(String("SValue"), B_VALUE_TYPE);
    stdtypes.add(String("String"), B_STRING_TYPE);
    stdtypes.add(String("SMessage"), B_MESSAGE_TYPE);
    stdtypes.add(String("char*"), B_CONSTCHAR_TYPE);

    numberoftypes=stdtypes.size();

    // bout << "Debug TypeBank - # of stdtypes loaded=" << numberoftypes << endl << endl;

    for (size_t index=0; index<numberoftypes ; index++)
    {	String name=stdtypes.KeyAt(index);
        String output=name;

        uint32_t	code=stdtypes.ValueAt(index);

        // initialize for the standard types

        switch (code) {

        case B_BOOL_TYPE :
        case B_FLOAT_TYPE :
        case B_DOUBLE_TYPE:
        {
            output.Capitalize();
            ToSValue.add(name, output);
            output.Prepend("As");
            FromSValue.add(name, output);
        }	break;

        case B_INT8_TYPE:
        case B_INT16_TYPE:
        case B_UINT8_TYPE:
        case B_UINT16_TYPE:
        case B_UINT32_TYPE:
        case B_SIZE_T_TYPE:
        case B_CHAR_TYPE:
        case B_WCHAR_TYPE:
        case B_INT32_TYPE:
        {
            ToSValue.add(name, String("Int32"));
            FromSValue.add(name, String("AsInt32"));
        }	break;

        case B_INT64_TYPE:
        {
            ToSValue.add(name, String("Int64"));
            FromSValue.add(name, String("AsInt64"));
        }	break;

        case B_BINDER_TYPE:
        case B_BINDER_WEAK_TYPE:
        {
            ToSValue.add(name, String());
            FromSValue.add(name, String());
        }	break;
        // initialize types that need customized output
        case B_MESSAGE_TYPE:
        case B_VALUE_TYPE:
        {	output=String();
            ToSValue.add(name, output);
            output.Prepend("As");
            FromSValue.add(name, output);
        }	break;

        case B_STRING_TYPE:
        case B_BIGTIME_TYPE:
        case B_NSECS_TYPE:
        case B_OFF_T_TYPE:
        case B_SSIZE_T_TYPE:
        case B_UINT64_TYPE:
        case B_CONSTCHAR_TYPE:
        {
            if (name=="char*")
                {	output=String("String"); }
            if (name=="String")
                {	output=String("String"); }
            if ((name=="nsecs_t") || (name=="bigtime_t"))
                {	output=String("Time"); }
            if (name=="off_t")
                {	output=String("Offset"); }
            if (name=="ssize_t")
                {	output=String("SSize"); }
            if (name=="uint64_t")
                {	output=String("Int64"); }

            ToSValue.add(name, output);
            output.Prepend("As");

            FromSValue.add(name, output);
        }	break;
        }
    }

    //  put all the info we have into the typebank
    for (size_t	index2=0; index2 < numberoftypes; index2++)
    {
            bool present=false;
            bool present1=false;

            String tname=stdtypes.KeyAt(index2);
            uint32_t tcode=stdtypes.ValueAt(index2);
            String func1=ToSValue.EditValueFor(tname, &present);
            String func2=FromSValue.EditValueFor(tname, &present1);

            if ((present) && (present1))
            {
                sp<IDLType>	typeobj=new IDLType(tname, tcode);
                sp<jmember>	f1=new jmember(func1, String("SValue"));
                sp<jmember>	f2=new jmember(func2, tname);

                typeobj->AddMember(f1);
                typeobj->AddMember(f2);

                tb.add(tname, typeobj);
            }

            else { berr << "type initialization for typename=" << tname << " failed \n"; }
    }

    //checktb(tb);

    return B_OK;
}

void
checktb (KeyedVector<String, sp<IDLType> > tb)
{
    int32_t  numberoftypes=tb.size();

    if (numberoftypes>0) {
            bout << "we have initialized " << numberoftypes << " types" << endl;

            for (int32_t index=0; index<numberoftypes ; index++)
            {	bout << endl;

                String	typekey=tb.KeyAt(index);
                sp<IDLType>	temp=tb.ValueAt(index);
                bout << "key=" << typekey << " type=" << temp->GetCode() << endl;

                    for (int32_t index2=0; index2<2 ; index2++)
                    {
                        bout << "function=" << (temp->GetMemberAt(index2))->ID() << endl;
                    }
            }
    }
}
