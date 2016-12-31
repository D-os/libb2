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

#include "OutputUtil.h"

#include <ctype.h>
#include <string.h>
//#include <support/StdIO.h>

extern InterfaceRec* FindInterface(const String &name);
extern sp<IDLType> FindType(const sp<IDLType>& typeptr);


// The native/proxy prefix to the class name (used to be "L" and "R")
const char* gNativePrefix = "Bn";
const char* gProxyPrefix = "Bp";

status_t
HeaderGuard(const String &in, String &out, bool system)
{
    // Do some validity checking
    if (!isalpha(in.string()[0]))
    {
        aerr << "filename must start with a letter " << in << endl;
        return BAD_VALUE;
    }

    char * p;
    int l;

    // Header Guard define name

    out = in;
    out.toUpper();
    l = out.length();
    p = out.lockBuffer(l);
    while (*p) {
        if (!isalnum(*p)) *p = '_';
        p++;
    }
    out.unlockBuffer(l);

    if (system) out = String("_") + out;

    return OK;
}

bool
IsAutoMarshalType(const sp<IDLType> &type)
{
    if (type->GetName() == "void") return false;

    //printf("Finding type for: %s\n", type->GetName().string());
    sp<IDLType> st=FindType(type);
    //printf("The type found is: %s\n", st->GetName().string());
    return (st->GetAttributes()&kAutoMarshal) != 0;
}

bool
IsAutobinderType(const sp<IDLType> &type)
{
    sp<IDLType> st=FindType(type);
    uint32_t code = st->GetCode();
    //printf("Type %s: attrs=%08lx\n", st->GetName().string(), st->GetAttributes());
    return (code != B_WILD_TYPE && code != B_VARIABLE_ARRAY_TYPE && code != B_MESSAGE_TYPE)
         || ((st->GetAttributes()&kAutoMarshal) != 0);
}

// clients can use kInsideClassScope so they don't need to pass the interface
// to TypeToCPPType, but they'd better be prepared to use the naked typedef
// at that point for the resulting file to compile correctly
const InterfaceRec* kInsideClassScope = NULL;

String
TypeToCPPType(const InterfaceRec* rec, const sp<IDLType>& obj, bool asConst)
{
    // Convert IDLType into a C++ type name
    // rec can be NULL if we are writing the type inside the class, otherwise
    // it must point to the owning class (for typedefs)

    String	cpptype = obj->GetName();

    if (cpptype==NULL) {
        aout << "<----- outpututil.cpp -----> invalid type when converting to CPP;"  << endl;
        _exit(1);
    }
    else {
        // Binder Types
        // IClass* -> sp<IClass>
        // [weak] IClass* -> wptr<IClass>
        // (both of these types have the name of "sptr")
        if (cpptype=="sptr") {
            if (obj->HasAttribute(kWeak) == true) {
                cpptype = "wptr";
            }
            InterfaceRec* iface=FindInterface(obj->GetIface());
            cpptype.append("<");
            if (iface->ID()== "Binder") { cpptype.append("I"); }
            cpptype.append(iface->ID());
            cpptype.append(">");
            if (asConst) {
                cpptype = String("const ") + cpptype;
                cpptype.append("&");
            }

        }
        else if ((cpptype=="String") || (cpptype=="SValue") || (cpptype=="SMessage")) {
            if (asConst) {
                cpptype = String("const ") + cpptype;
                cpptype.append("&");
            }
        }
        else if (cpptype=="char*") {
            if (asConst) {
                // in the rare case this is a const char*
                cpptype = String("const ") + cpptype;
            }
            else {
                cpptype="String";
            }
        }
        else if (cpptype=="void") {
            ;	// cpptype is fine as is
        }
        else {

            // deal with typedefs...
            // If the stored type is different than the object type, then
            // we have a user defined typedef...
            // add in the class name as a qualifier, because they are defined
            // inside the IClass definition.
            sp<IDLType> storedtype=FindType(obj);
            if (storedtype->GetName() != obj->GetName() && rec != kInsideClassScope) {
                String classQualifier = rec->ID();
                classQualifier.append("::");
                cpptype = String(classQualifier) + cpptype;
            }
            if (asConst) {
                uint32_t code = storedtype->GetCode();
                if (code == B_WILD_TYPE || code == B_VARIABLE_ARRAY_TYPE) {
                    // If B_WILD_TYPE then we have an exported type
                    // that we really don't know about, or
                    // if B_VARIABLE_ARRAY_TYPE, then we have a sequence
                    // typedef.  Both of which want const references
                    // for input parameters.
                    cpptype = String("const ") + cpptype;
                    cpptype.append("&");
                }
            }
        }
    }

    return cpptype;
}

String
TypeToDefaultValue(const InterfaceRec* rec, const sp<IDLType>& obj)
{
    String	cpptype=obj->GetName();

    if (cpptype==NULL) {
        aout << "<----- outpututil.cpp -----> invalid type when converting to CPP;"  << endl;
        _exit(1);
    }
    else {
        // Binder Types
        if (cpptype=="sptr") {
            cpptype = "NULL";
        }
        else if (cpptype=="char*") {
            cpptype = "NULL";
        }
        else if (cpptype=="SValue") {
            cpptype = "B_UNDEFINED_VALUE";
        }
        else if (cpptype=="String") {
            cpptype = "String::EmptyString()";
        }
        else if (cpptype=="SPoint") {
            // The default constructor for SPoint does not initialize the object,
            // for performance reasons.
            cpptype = "SPoint::Origin()";
        }
        else if (cpptype == "status_t") {
            cpptype = "OK";
        }
        else if ((cpptype=="size_t") ||
                (cpptype=="char") ||
                (cpptype=="wchar32_t") ||
                (cpptype=="int8_t") ||
                (cpptype=="int16_t") ||
                (cpptype=="int32_t") ||
                (cpptype=="int64_t") ||
                (cpptype=="uint8_t") ||
                (cpptype=="uint16_t") ||
                (cpptype=="uint32_t") ||
                (cpptype=="uint64_t") ||
                (cpptype=="nsecs_t") ||
                (cpptype=="float") ||
                (cpptype=="double")) {
            cpptype = "0";
        }
        else {
            cpptype = TypeToCPPType(rec, obj, false);
            cpptype.append("()");
        }
    }

    return cpptype;
}


String
ToSValueConversion(const sp<IDLType>& type, const String &variable)
{
    String s("SValue::");
    String tn=type->GetName();

    sp<IDLType> storedtype=FindType(type);
    if (storedtype->CountMembers()>0) {
        sp<jmember> toBV=storedtype->GetMemberAt(0);

        if (tn=="SValue") {
            s=variable;
        }
        else if ((tn=="SMessage")) {
            s =variable;
            s.append(".AsValue()");
        }
        else if (tn=="sptr") {
            s.append("Binder");
            s.append("(");
            s.append(variable);

            if (type->GetIface()!="IBinder") {
                if (type->HasAttribute(kWeak)) {
                    s.append(".promote() != NULL ? ");
                    s.append(variable);
                    s.append(".promote()->AsBinder() : sp<IBinder>()");
                }
                else {
                    s.append("->AsBinder()");
                }
            }
            s.append(")");
        }
        else if ((tn!=NULL) && (storedtype->GetCode()==B_WILD_TYPE)) {
            //aout << "<----- outpututil.cpp -----> ToSValueConversion - " << tn << " is a custom type " << endl;
            s=variable;
            s.append(".");
            s.append(toBV->ID());
            s.append("()");
        }
        else if (storedtype->GetCode() == B_VARIABLE_ARRAY_TYPE) {
            // if we are dealing with a sequence, then
            // Vector provides an AsValue
            // the storedtype we are dealing with is the element
            // types, so we ignore the toBV since we want AsValue
            // for the entire array.
            s = variable;
            s.append(".AsValue()");
        }
        else if (tn=="char*") {
            s.append("String(");
            s.append(variable);
            s.append(")");
        }
        else {
            s.append(toBV->ID());
            s.append("(");
            s.append(variable);
            s.append(")");
        }

        return s;
    }
    else {
        aout << "<----- outpututil.cpp -----> there is no function to marshall type=" << tn << endl;
        _exit(1);
    }
    return NULL;
}


String
FromSValueConversion(const sp<IDLType>& type, const String &variable, bool setError)
{
    String s(variable);
    s.append(".");

    String tn=type->GetName();

    sp<IDLType> storedtype=FindType(type);
    if (storedtype->CountMembers()>1) {
        sp<jmember> fromBV=storedtype->GetMemberAt(1);

        if ((tn=="SValue") || (tn=="SMessage")) {
            // simple assignment
            s = variable;
            return s;
        }
        if (tn=="sptr") {
            //aout << "interface id=" << type->GetIface() << endl;
            if (type->GetIface() == "IBinder") {
                s=variable;
                if (setError) {
                    s.append(".AsBinder(&_pidgen_err)");
                }
                else {
                    s.append(".AsBinder()");
                }
            }
            else {
                InterfaceRec*	validiface=FindInterface(type->GetIface());
                s.clear();
                /* if (validiface->InNamespace()) {
                    s.append(validiface->Namespace());
                    s.append(':', 2);
                } */
                s.append(validiface->ID());
                s.append("::AsInterface(");
                s.append(variable);
                if (setError) {
                    s.append(", &_pidgen_err)");
                }
                else {
                    s.append(")");
                }
            }
            return s;
        }
        else if ((tn!=NULL) && (storedtype->GetCode() == B_WILD_TYPE)) {
            // custom types which have Code==B_WILD_TYPE
            // all custom types use explicit constructor
            //aout << "<----- outpututil.cpp -----> FromSValueConversion - " << tn << " is a custom type " << endl;

            s=fromBV->ID();
            s.append("(");
            s.append(variable);
            if (setError) {
                s.append(", &_pidgen_err)");
            }
            else {
                s.append(")");
            }
            //s.Prepend(".");
            return s;
        }
        else if ((tn!=NULL) && storedtype->GetCode() == B_VARIABLE_ARRAY_TYPE) {
            // custom types which are sequences
            // if we are dealing with an array type, then we need to
            // use the Vector SetFromValue function
            s = "SetFromValue(";
            s.append(variable);
            if (setError) {
                s.append(", &_pidgen_err)");
            }
            else {
                s.append(")");
            }
            return s;
        }
        else {
            s.append(fromBV->ID());
            if (setError) {
                s.append("(&_pidgen_err)");
            }
            else {
                s.append("()");
            }

            if ((tn=="size_t") ||
                    (tn=="char") ||
                    (tn=="wchar32_t") ||
                    (tn=="int8_t") ||
                    (tn=="int16_t") ||
                    (tn=="uint8_t") ||
                    (tn=="uint16_t") ||
                    (tn=="uint32_t") ||
                    (tn=="uint64_t")) {
                s = String(")") + tn + String("(") + s;
            }
            return s;
        }
    }
    else {
        aout << "<----- outpututil.cpp -----> there is no function to marshall type=" << tn << endl;
        _exit(1);
    }
    return NULL;

}

//String IndexToSValue(int32_t index)
//{
//	String s;
//	if (index>=0 && index<=10) {
//		s << "B_" << index << "_INT32";
//	} else {
//		s << "SValue::Int32(" << index << ")";
//	}
//	return s;
//}

static const char* kAssignStr = " = ";

String
FromSValueExpression(const InterfaceRec* rec,
                     const sp<IDLType>& vartype,
                     const String& varname,
                     const String& valuename,
                     ExpressionKind kind,
                     bool setError)
{
    // Create the appropriate expression for converting from an SValue
    // We do this here, so we don't have to special case the code all over OutputCPP.cpp
    // There are 8 different places where FromSValueConversion is called,
    // all just slightly different than the other.
    // However, they can be grouped into the following categories:
    //	INITIALIZE,				// BClass v = BClass(value, &err);
    //	ASSIGN,					// arg = BClass(value, &err);
    //	INDIRECT_ASSIGN,		// *arg = BClass(value, &err);
    //	RETURN					// return BClass(value, &err);

    //	sequences, which are handled in C++ as Vector must be treated differently.
    //	INITIALIZE,				// VectorClass v; v.SetFromValue(value, &err);
    //	ASSIGN,					// arg.SetFromValue(value, &err);
    //	INDIRECT_ASSIGN,		// arg->SetFromValue(value, &err);
    //	RETURN					// return VectorClass(value, &err);

    String expr;
    sp<IDLType> storedtype=FindType(vartype);
    if (storedtype->GetCode() != B_VARIABLE_ARRAY_TYPE) {
        switch (kind) {
            case INITIALIZE:
                {
                expr.append(TypeToCPPType(rec, vartype, false));
                expr.append(" ");
                expr.append(varname);
                expr.append(kAssignStr);
                break;
                }
            case ASSIGN:
                expr.append(varname);
                expr.append(kAssignStr);
                break;
            case INDIRECT_ASSIGN:
                expr.append("*");
                expr.append(varname);
                expr.append(kAssignStr);
                break;
            case RETURN:
                expr.append("return ");
                break;
        }
    }
    else {
        // The SFlattenable signature for SetFromValue doesn't set any errors, just returns them
        setError = false;
        switch (kind) {
            case INITIALIZE:
                // VectorClass v; v.SetFromValue(value, &err);
                expr.append(TypeToCPPType(rec, vartype, false));
                expr.append(" ");
                expr.append(varname);
                expr.append(";\n");
                expr.append(varname);
                expr.append(".");
                break;
            case ASSIGN:
                // arg.SetFromValue(value, &err);
                expr.append(varname);
                expr.append(".");
                break;
            case INDIRECT_ASSIGN:
                // arg->SetFromValue(value, &err);
                expr.append(varname);
                expr.append("->");
                break;
            case RETURN:
                // VectorClass rv; rv.SetFromValue(value, &err); return rv;
                expr.append(TypeToCPPType(rec, vartype, false));
                expr.append(" ");
                expr.append(varname);
                expr.append(";\n");
                expr.append(varname);
                expr.append(".");
                // ... to be continued after FromSValueConversion
                break;
        }
    }
    expr.append(FromSValueConversion(vartype, valuename, setError));
    expr.append(";");
    if (storedtype->GetCode() == B_VARIABLE_ARRAY_TYPE && kind == RETURN) {
        expr.append("\nreturn ");
        expr.append(varname);
        expr.append(";");
    }
    return expr;
}


void
AddFromSValueStatements(const InterfaceRec* rec,
                        sp<StatementList> statementList,
                        const sp<IDLType>& vartype,
                        const String& varname,
                        const String& valuename)
{
    sp<IDLType> storedtype = FindType(vartype);
    String cpptype = TypeToCPPType(rec, vartype, false);
    if (storedtype->GetCode() != B_VARIABLE_ARRAY_TYPE) {
        // statements = MyType rv(value);
        statementList->AddItem(new Literal(true, "%s %s(%s)", cpptype.string(), varname.string(), valuename.string()));
    }
    else {
        // statements = MyType rv;
        //				rv.SetFromValue(value);
        statementList->AddItem(new Literal(true, "%s %s", cpptype.string(), varname.string()));
        statementList->AddItem(new Literal(true, "%s.SetFromValue(%s)", varname.string(), valuename.string()));
    }
}


int32_t
CountStringTabs(const String& str, int32_t tabLen)
{
    return (str.length()+(tabLen-1))/tabLen;
}

const char*
PadString(const String& str, int32_t fieldTabs, int32_t tabLen)
{
    static const char tabs[] =
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"	// 16
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"	// 32
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"	// 48
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";	// 64
    const int32_t len = str.length()/tabLen;
    int32_t pad = fieldTabs - len;
    if (pad <= 0) pad = 1;
    if (pad > (sizeof(tabs)-1)) pad = sizeof(tabs)-1;
    return tabs + (sizeof(tabs) - pad - 1);
}

void CollectParents(InterfaceRec* rec, const Vector<InterfaceRec*>& classes, Vector<InterfaceRec*>* out)
{
    Vector<String> parents = rec->Parents();
    const size_t N = parents.size();
    for (size_t i=0; i<N; i++) {
        const size_t Nc = classes.size();
        for (size_t c=0; c<Nc; c++) {
            if (parents[i] == classes[c]->ID() || parents[i] == classes[c]->FullInterfaceName()) {
                out->add(classes[c]);
            }
        }
    }
    out->add(rec);
}

NamespaceGenerator::NamespaceGenerator()
    : namespaceDepth(0), first(true)
{
}

NamespaceGenerator::~NamespaceGenerator()
{
}

void NamespaceGenerator::EnterNamespace(TextOutput &stream, const String& newNS, const Vector<String>& newUsing)
{
    String ns, nspart;
    int32_t pos, oldpos;
    size_t i;
    bool diff = prevNamespace != newNS;

    for (i=0; !diff && i<newUsing.size(); i++) {
        diff = prevUsing.indexOf(newUsing[i]) < 0;
    }

    if (!diff) return;

    if (prevNamespace != newNS) {
        if (prevNamespace.length() > 0) {
            stream << endl;
            for (i=0; i<namespaceDepth; i++) stream << "} ";
            namespaceDepth = 0;
            stream << "// namespace " << prevNamespace << endl;
            stream << endl;
        }
        prevNamespace = newNS;
        prevUsing.clear();
        if (prevNamespace.length() > 0) {
            ns = newNS;
            oldpos = 0;
            while ((pos = ns.findFirst("::", oldpos)) > 0) {
                nspart.setTo(ns.string()+oldpos, pos-oldpos);
                stream << "namespace " << nspart << " {" << endl;
                oldpos = pos + 2;
                namespaceDepth++;
            }
            namespaceDepth++;
            nspart.setTo(ns.string()+oldpos);
            stream << "namespace " << nspart << " {" << endl;
        } else {
            namespaceDepth = 0;
        }
    }

    if (newNS != "palmos::support" && prevUsing.indexOf(String("palmos::support")) < 0) {
        stream << "using namespace palmos::support;" << endl;
        prevUsing.add(String("palmos::support"));
    }

    for (i=0; i<newUsing.size(); i++) {
        const String& n = newUsing[i];
        if (newNS != n && prevUsing.indexOf(n) < 0) {
            stream << "using namespace " << n << ";" << endl;
            prevUsing.add(n);
        }
    }
}

void NamespaceGenerator::CloseNamespace(TextOutput &stream)
{
    if (namespaceDepth > 0) {
        size_t i;
        for (i=0; i<namespaceDepth; i++) stream << "} ";
        stream << "// namespace " << prevNamespace << endl;
        stream << endl;
        namespaceDepth = 0;
        prevNamespace = "";
        prevUsing.clear();
    }
}
