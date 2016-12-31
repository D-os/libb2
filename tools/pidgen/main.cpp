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

#include "TypeBank.h"
#include "idlstruct.h"
#include "InterfaceRec.h"
#include "OutputI.h"
#include "OutputCPP.h"
#include "OutputUtil.h"
//#include "WsdlOutput.h"
//#include "WSDL.h"

//#include <support/atomic.h>

#if !defined(_WIN32)
#include <getopt.h>
#else
#include "getopt.h"
#endif

#include <ctype.h>

#define PRINT_PATHS(x) // x

using namespace android;
//using namespace palmos::storage; // BFile only

static Vector<String> IdlFiles;
static Vector<String> WsdlFiles;
static Vector<InterfaceRec> ParseTree;
static KeyedVector<String, StackNode> SymbolTable;
static KeyedVector<String, sp<IDLType> > TypeBank;
static KeyedVector<String, InterfaceRec*> ifnamevector;

const KeyedVector<String, sp<IDLType> >& getTypeBank()
{
    return TypeBank;
}

Vector<String> cppn;
static InterfaceRec gIBinderInterface(String("Binder"), String("org.openbinder.support.IBinder"), cppn, NULL, IMP); // default binder

#define READ_BUFFER_SIZE 32768

extern FILE *yyin; // flex input
bool verbose = false;

// Set to non-zero if there was a parse error.
static int parseError = 0;

int
usage()
{
    aout << " usage: pidgen [flags] foo.idl " << endl
         << " if more than 1 file specified, files will be run in sequence" << endl
         << " with the same set of flags"
         << endl << endl << " flags: " << endl << endl;
    aout << "-- import path | -I path " << endl << " adds path to the list of import directories " << endl;
    aout << "-- output-dir path | -O path " << endl << " outputdir for generated files; dir for cpp files only when used with -S flag; default is directory of .idl file " << endl;
    aout << "-- output-header-dir path | -S path" << endl << "outputdir for generated header files - when used in conjunction with -O, header files go in output-header directory and cpp files to the output dir; default is directory of .idl file " << endl;
    aout << "-- base-header-dir path | -B path" << endl << "base dir of header files - when used in conjunction with -O, header files go in output-header directory and cpp files to the output dir; default is directory of .idl file " << endl;
    aout << "-- verbose | -v " << endl << " prints some information while running" << endl;
    aout << "-- help | -h " << endl << " displays this text" << endl;
    return 2;
}

status_t
readargs(int argc, char ** argv, String& hd, String& bd, String& od, Vector<String>& id)
{
    static option long_options[] = {
        {"import", 1, 0, 'I'},
        {"output-dir", 1, 0, 'O'},
        {"output-headerdir", 1, 0, 'S'},
        {"base-headerdir", 1, 0, 'B'},
        {"verbose", 1, 0, 'v'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };

    char getopt_result;
    for (;;) {
        getopt_result=getopt_long(argc, argv, "I:O:S:B:vh", long_options, NULL);

        if (getopt_result==-1) break;
        switch (getopt_result) {
            case 'I' : {
                    id.add(String(optarg));
                    //aout << "we have " << id.size() << " importdirs" << endl;
                    break;
                }
            case 'O': {
                    od=optarg;
                    break;
                }
            case 'S': {
                    hd=optarg;
                    break;
                }
            case 'B': {
                    bd=optarg;
                    break;
                }
            case 'v': {
                    verbose = true;
                    break;
                }
            default: {
                    return usage();
                    break;
                }
        }
    }

    if (optind>=argc) {
        aerr << argv[0] << ": no idl file specified" << endl;
        return usage();
    }
    else {
        for (int ifile=optind; ifile<argc; ifile++) {
            String fn(argv[ifile]);

//            int32_t index = fn.IFindLast(".wsdl");

//            if (index >= 0)
//                WsdlFiles.add(fn);
//            else
                IdlFiles.add(fn);
        }
    }

    // if no import directory was specified then
    // use the current direcotry
    if (id.size() == 0) {
        id.add(od);
    }

    return OK;
}


//idl file, name of new header, name of new cpp file, specified headerdir, specified outputdir
status_t
setpath(
        String& filebase, String& iheader,
        String& cppfile, String& header, const String& basehdir,
        String& output, String& prevdir)
{
    String origbase = filebase;

    ssize_t pos = -1, f;

    filebase = filebase.getPathLeaf();
    PRINT_PATHS(aout << "Initial filebase: " << filebase << endl);

    // no path is given in association with the idl file
    if (output == "") {
        if (pos>=0) {
            output = origbase.getBasePath();
        }
        else {
            output.setPathName(".");
        }
    }
    if (filebase.length()<=0) {
        return usage();
    }

    pos = -1;
    while ((f = filebase.find(".")) >= 0) {
        pos = f;
    }
    if (pos>=0) {
        if (0==strcmp(filebase.string()+pos, ".idl")) {
            filebase.setTo(filebase, pos);
        }
    }
    PRINT_PATHS(aout << "truncated filebase=" << filebase << endl);

    status_t err;

    // if no header dir specified, default is output dir for .h
    if (header != "") {
        iheader.setPathName(header);
    }
    else {
        iheader.setPathName(output);
    }

    iheader.appendPath(filebase);
    iheader.append(".h");

    cppfile.setPathName(output);
    cppfile.appendPath(filebase);
    cppfile.append(".cpp");

    PRINT_PATHS(aout << "Header : " << header << endl);
    PRINT_PATHS(aout << "I Header : " << iheader << endl);
    PRINT_PATHS(aout << "CPP File : " << cppfile << endl);

    if (basehdir == "") {
        // Try to infer the path to the header -- just assume it is
        // one level deep.
        prevdir.setPathName(String(header.getPathLeaf()));
    } else {
        // Strip 'basehdir' off the front of the full header path.
        int32_t p = header.find(basehdir);
        if (p == 0) {
            prevdir.setTo(header.string()+basehdir.length(), header.length()-basehdir.length());
            if (prevdir.find("/") == 0) {
                prevdir = String(prevdir.string()+1);
            }
        }
    }

    PRINT_PATHS(aout << "prevdir=" << prevdir << endl);
    prevdir.appendPath(String(iheader.getPathLeaf()));
    PRINT_PATHS(aout << "printableIHeader=" << prevdir << endl);

    return OK;
}

InterfaceRec*
FindInterface(const String &name)
{
    if (name=="IBinder" || name=="palmos::support::IBinder") {
        return &gIBinderInterface;
    }
    else {
        bool present=ifnamevector.indexOfKey(name) >= 0;

        if (present) {
            return(ifnamevector.editValueFor(name));
        }
        else {
            aerr << "pidgen: <---- FindInterface ----> invalid interface - check to see if foward declaration needed for " << name << endl;
            parseError = 10;
            _exit(10);
            return NULL;
        }
    }
}

sp<IDLType>
FindType(const sp<IDLType>& typeptr)
{
    String code=typeptr->GetName();
    bool present=TypeBank.indexOfKey(code) >= 0;

    if (present) {
        return TypeBank.editValueFor(code);
    }
    else {
        aerr << "pidgen: <---- FindType ----> " << code << " could not be found in TypeBank" << endl;
        parseError = 10;
        return NULL;
    }
}

int
yyerror(void *voidref, const char *errmsg)
{
    fprintf(stderr, "we got an err %s\n", errmsg);
    parseError = 10;
    return 1;
}

int generate_from_idl(
        const String& _idlFileBase, const String& _headerdir,
        const String& basehdir, const String& _outputdir, Vector<String>& importdir)
{
    if (verbose)
        aout << "IDL File: " << _idlFileBase << endl;
    // open interface
    const char* nfn= _idlFileBase.string();
    FILE *input = fopen(nfn, "r");

    sp<IByteOutput> byteStream;
    sp<ITextOutput> stream;
    sp<BFile> file;
    status_t err;
    bool system = false;

    if (!input) {
        aerr << "pidgen: --- could not open " << _idlFileBase << " --- " << endl;
        return 10;
    }
    else {
        // if input was valid, create header and cppfile name
        String idlFileBase(_idlFileBase), headerdir(_headerdir), outputdir(_outputdir);
        String iheader, cppfile, prevdir;
        setpath(idlFileBase, iheader, cppfile, headerdir, basehdir, outputdir, prevdir);
        PRINT_PATHS(aout << "idlFileBase: " << idlFileBase << ", iheader: " << iheader
                    << ", cppfile: " << cppfile << ", headerdir: " << headerdir
                    << ", outputdir: " << outputdir << ", prevdir: " << prevdir << endl);

        initTypeBank(TypeBank);
        IDLStruct result;

        // pass importdir to flex if it is used
        if (importdir.size()!=0) {
            for (size_t a=0; a<importdir.size(); a++)
            {	result.AddImportDir(importdir.ItemAt(a)); }
        }

        // force flex to discard any previous files
        fflush(yyin);
        yyin=input;
        yyparse((void*)&result);

        // If there was an error, abort!
        if (parseError != 0) return parseError;

        // get things back from bison - everything that is passed back is a hardcopy
        ParseTree=result.Interfaces();
        headerdir=result.Header();
        SymbolTable.SetTo(result.SymbolTable());

        // add user-defined types from bison into the TypeBank
        // First we need to add all "primitive" types, and then
        // we need to add typedefs.  If we don't do this two step
        // approach we can get to a typedef that uses a type we
        // haven't yet added to our TypeBank

        const KeyedVector<String, sp<IDLType> >& UTypes = result.TypeBank();

        // Iterate and add all user defined types
        for (size_t i = 0; i < UTypes.size(); i++) {
            String userKey = UTypes.KeyAt(i);
            sp<IDLType> userType = UTypes.ValueAt(i);
            const String& syn = userType->GetPrimitiveName();
            if (syn.length() == 0) {
                TypeBank.add(userKey, userType);
            }
        }

        // Now iterate again, but only pick up typedefs
        for (size_t i=0; i<UTypes.size(); i++) {
            // Get the real type and verify that we know about it
            String userKey = UTypes.KeyAt(i);
            sp<IDLType> userType = UTypes.ValueAt(i);
            const String& syn = userType->GetPrimitiveName();
            if (syn.length() > 0) {
                bool exists=false;
                sp<IDLType> baseType = TypeBank.ValueFor(syn, &exists);
                if (exists) {
                    // Now add a clone of the existing type with the new name
                    // That way, when that name is used, we will still know
                    // how to deal with the type.
                    sp<IDLType> newType = new IDLType(baseType);
                    // If the parse put in some code modifier, we need to
                    // bring that along also
                    if (userType->GetCode() != B_UNDEFINED_TYPE) {
                        newType->SetCode(userType->GetCode());
                    }
                    // For sptr (et al) types, the parse put in an iface,
                    // but the base types don't specify the interface.
                    // Make sure we don't leave that behind either
                    newType->SetIface(userType->GetIface());
                    // aout << "baseType code =" << baseType->GetCode() << " baseType name=" << baseType->GetName() << endl;
                    // aout << "UTypes.KeyAt = " << userKey << endl;
                    TypeBank.add(userKey, newType); }
                else {
                    aout << "Error - Can't find user type: " << syn << " for typedef: " << userKey << endl;
                    exit(10);
                }
            }
        }

        //checktb(TypeBank);

        Vector<InterfaceRec*> ifvector;
        // store things from the parse tree into digestable formats for the output engine
        for (size_t s=0; s<ParseTree.size(); s++) {
            InterfaceRec* _interface = const_cast<InterfaceRec*>(&ParseTree[s]);

            // Trim out forward declarations if an import or definition has
            // also been found.
            if(_interface->Declaration()==FWD) {
                // aout << "FWD: " << _interface->FullInterfaceName() << endl;
                bool skip = false;
                for (size_t t=0; !skip && t<ifvector.size(); t++) {
                    if (ifvector[t]->FullInterfaceName() == _interface->FullInterfaceName()) skip = true;
                }
                if (skip) continue;
            }
            else {
                // aout << "IMP/DCL: " << _interface->FullInterfaceName() << endl;
                for (size_t t=0; t<ifvector.size(); t++) {
                    if (ifvector[t]->FullInterfaceName() == _interface->FullInterfaceName()
                            && ifvector[t]->Declaration()==FWD) {
                        // we currently have this interface as a forward declaration,
                        // remove that version so we can add implementation version
                        ifvector.RemoveItemsAt(t);
                        ifnamevector.RemoveItemFor(_interface->ID());
                    }
                }
            }

            ifvector.add(_interface);
            ifnamevector.add(_interface->ID(), _interface);
            // aout << "we just added interface = " << _interface->ID() << " as... " << ((_interface->Declaration()==FWD)?"Forward":"Implementation") << endl;
        }

        // scan through the includes to attach namespaces & check their validity
        Vector<IncludeRec> headers;

        for (size_t t=0; t<(result.Includes()).size(); t++) {
            IncludeRec includerec = (result.Includes()).ItemAt(t);
            String includefile = includerec.File();

#if TARGET_HOST == TARGET_HOST_WIN32
            includefile.ReplaceAll('.\\', '/');
#endif
            //aout << "includefile=" << includefile << endl;

            includefile.ReplaceLast("/", ":");
            int s=includefile.FindLast("/");
            if (s>=0) {
                includefile=includefile.string()+s+1;
            }

            int32_t pos=includefile.FindLast('.');
            if (pos>=0) {
                if (0==strcmp(includefile.string()+pos, ".idl")) {
                    includefile.Truncate(pos);
                }
            }

            includefile.append(".h");
            includefile.ReplaceAll(":", "/");
            headers.add(IncludeRec(includefile, includerec.Comments()));
        }

        // create interface header
        uint32_t flags =  O_WRONLY | O_CREAT | O_TRUNC;
#if TARGET_HOST == TARGET_HOST_WIN32
        flags |= O_BINARY;
#endif
        if (verbose)
            aout << "Trying to open file '" << iheader << "'" << endl;
        file=new BFile(iheader.string(), flags);
        if (!file->IsWritable()) {
            aerr << "pidgen Overwrite failed - could not open " << iheader << " for writing" << endl;
            return 10;
        }
        byteStream=new BByteStream(sp<IStorage>(file.ptr()));
        stream=new BTextOutput(byteStream);

        // A word about include files...
        // The current .h file is specified with prevdir, which uses the leaf of the
        // -S directory (so that the result is "widget/IWidget.h" rather than "IWidget.h"
        // For other header files, if found on search paths, it uses the last directory
        // in the path, so that once again, the result is a "widget/IWidget.h" format.
        // This should probably be controlled with a -include_parent_dir to indicate
        // what you want there.  .idl files outside the normal build directory structures
        // might not fit this pattern.
        // Also, there should be a -system_include or -local_include type option which would
        // dictate the #include using <> or "".
        err=WriteIHeader(stream, ifvector, prevdir, headers, result.BeginComments(), result.EndComments(), system);

        stream=NULL;
        byteStream=NULL;
        file=NULL;
        unlink(cppfile.string());

        // create cppfile
        file=new BFile(cppfile.string(), flags);
        if (!file->IsWritable()) {
            aerr << "pidgen: Overwrite failed - could not open " << cppfile << " for writing" << endl;
            return 10;
        }
        byteStream=new BByteStream(sp<IStorage>(file.ptr()));
        stream=new BTextOutput(byteStream);

        err=WriteCPP(stream, ifvector, idlFileBase, prevdir, system);

        stream=NULL;
        byteStream=NULL;
        file=NULL;

        // clean up before we start on the next file
        ParseTree.MakeEmpty();
        cleanTypeBank(TypeBank);
        SymbolTable.MakeEmpty();
        headers.MakeEmpty();
        ifvector.MakeEmpty();
        ifnamevector.MakeEmpty();
        importdir.MakeEmpty();

        fflush(yyin);
        fclose(yyin);
        yyin = NULL;
    }

    return parseError;
}

int generate_from_wsdl(
        const String& wsdlFile, const String& headerdir,
        const String& basehdir, const String& outputdir, Vector<String>& importdir)
{
    if (verbose)
        aout << endl << "WSDL File : " << wsdlFile << endl;

    const char* name = wsdlFile.string();
    int fd = open(name, 0, O_RDONLY);

    sp<BFile> file;
    sp<IByteOutput> byteStream;
    sp<ITextOutput> stream;

    if (!fd) {
        aerr << "pidgen: --- could not open " << wsdlFile << " --- " << endl;
        return 10;
    }

    char* buf = (char*)malloc(READ_BUFFER_SIZE);
    size_t howmany = 0;
    String buffer;
    do {
        memset(buf, 0, READ_BUFFER_SIZE);
        howmany = read(fd, buf, READ_BUFFER_SIZE);
        buffer.append(buf, howmany);
    } while (howmany == READ_BUFFER_SIZE);

    free(buf);

    sp<BWsdl> wsdl = new BWsdl();
    sp<BWsdlCreator> creator = new BWsdlCreator();

    creator->Parse(buffer, wsdl);
    if (verbose)
        aout << "--------------------------------------------" << endl;

    // create the WsdlClass.
    sp<WsdlClass> obj = NULL;
    create_wsdl_class(wsdl, obj);

    String output;

    String typesInterface;
    typesInterface.append(wsdl->Name());
    typesInterface.append("Types.idl");

    output.setPathName(outputdir);
    output.appendPath(typesInterface);
    //	aout << "Creating " << output << endl;
    uint32_t flags =  O_WRONLY | O_CREAT | O_TRUNC;
#if TARGET_HOST == TARGET_HOST_WIN32
    flags |= O_BINARY;
#endif
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the interface
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_types_interface(obj, stream);

    String interfaceName("I");
    interfaceName.append(wsdl->Name());
    interfaceName.append(".idl");
    output.setPathName(outputdir);
    output.appendPath(interfaceName);
    //	aout << "Creating " << output << endl;
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the interface
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_interface(obj, stream, typesInterface);

    // create the output file
    String headername = wsdl->Name();
    headername.append(".h");
    output.setPathName(outputdir);
    output.appendPath(headername);
    //	aout << "Creating " << output << endl;
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the header
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_header(obj, stream, headername);

    String typesname = wsdl->Name();
    typesname.append("Types.h");
    output.setPathName(outputdir);
    output.appendPath(typesname);
    //	aout << "Creating " << output << endl;
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the types header
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_types_header(obj, stream, typesname);

    String cppname = wsdl->Name();
    cppname.append(".cpp");
    output.setPathName(outputdir);
    output.appendPath(cppname);
    //	aout << "Creating " << output << endl;
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the cpp
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_cpp(obj, stream, cppname, headername);


    String typescppName = wsdl->Name();
    typescppName.append("Types.cpp");
    output.setPathName(outputdir);
    output.appendPath(typescppName);
    //	aout << "Creating " << output << endl;
    file = new BFile(output.string(), flags);
    if (!file->IsWritable()) {
        aerr << "pidgen: Overwrite failed - could not open " << output << " for writing" << endl;
        return 10;
    }

    // generate the cpp
    byteStream = new BByteStream(sp<IStorage>(file.ptr()));
    stream = new BTextOutput(byteStream);
    wsdl_create_types_cpp(obj, stream, typescppName, headername);

    // now process the idl file
    output.setPathName(outputdir);
    output.appendPath(interfaceName);
    if (verbose)
        aout << "--------------------------------------------" << endl;
    return generate_from_idl(output, headerdir, basehdir, outputdir, importdir);
}

int
main(int argc, char ** argv)
{
    // headerdir stores the header directory specified when we want to separate the .h and .cpp output
    String outputdir;
    String basehdir;
    String headerdir;
    Vector<String> importdir;

    gIBinderInterface.AddCppNamespace(String("palmos::support"));
    readargs(argc, argv, headerdir, basehdir, outputdir, importdir);
    outputdir.PathNormalize();
    basehdir.PathNormalize();
    headerdir.PathNormalize();

    size_t size = IdlFiles.size();
    for (size_t i = 0; i < size; i++) {
        String idl = IdlFiles.ItemAt(i);
        idl.PathNormalize();
        int result = generate_from_idl(idl, headerdir, basehdir, outputdir, importdir);
        if (result != 0) return result;
    }

    size = WsdlFiles.size();
    for (size_t i = 0; i < size; i++) {
        String wsdl = WsdlFiles.ItemAt(i);
        wsdl.PathNormalize();
        int result = generate_from_wsdl(wsdl, headerdir, basehdir, outputdir, importdir);
        if (result != 0) return result;
    }

    return 0;
}
