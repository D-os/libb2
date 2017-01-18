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

#include <support/Context.h>

//#include <support/INode.h>
//#include <support/IProcessManager.h>

#include <support/String.h>
#include <support/Node.h>
//#include <support/Catalog.h>
//#include <support/Looper.h>
//#include <support/StdIO.h>
//#include <support/Process.h>
//#include <support/SortedVector.h>
//#include <app/ICommand.h> // I think ICommand belongs in the support kit --joe
//#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>

namespace os {
namespace support {

const String kProcesses("/processes");
const String kServices("/services");

Context::Context()
    :	 m_root(NULL)
{
}

Context::Context(const sp<INode>& root)
    :	m_root(root)
{
}

Context::Context(const Context& context)
    :	m_root(context.m_root)
{
}

Context::~Context()
{
}

Context& Context::operator=(const Context& o)
{
    m_root = o.m_root;
    return *this;
}

bool Context::operator==(const Context& o) const { return m_root == o.m_root; }
bool Context::operator!=(const Context& o) const { return m_root != o.m_root; }
bool Context::operator<(const Context& o) const { return m_root < o.m_root; }
bool Context::operator<=(const Context& o) const { return m_root <= o.m_root; }
bool Context::operator>=(const Context& o) const { return m_root >= o.m_root; }
bool Context::operator>(const Context& o) const { return m_root > o.m_root; }

status_t Context::InitCheck() const
{
    return m_root != NULL ? OK : NO_INIT;
}

sp<IBinder> Context::LookupService(const String& name) const
{
    String path(kServices);
    path.PathAppend(name);

    Value value = Node(m_root).Walk(&path, uint32_t(0));

//	bout << "*** Lookup() name = " << name << endl;
//	bout << "             value = " << value << endl;
    return value.as<IBinder>();
}


//status_t Context::PublishService(const String& name, const sp<IBinder>& object) const
//{
//    String path(kServices);
//    path.PathAppend(name);
//    String leaf(path.PathLeaf());
//    path.PathGetParent(&path);

////	bout << "--- PublishService() path = " << path << " leaf = " << leaf << endl;

//    Value value = SNode(m_root).Walk(&path, INode::CREATE_CATALOG);

//    status_t err;
//    sp<ICatalog> catalog = ICatalog::AsInterface(value, &err);
//    if (catalog != NULL) return catalog->AddEntry(leaf, Value::Binder(object));

//#if BUILD_TYPE == BUILD_TYPE_DEBUG
//    bout << "*** NAME NOT FOUND when publishing service " << object << " as " << name << endl;
//#endif
//    return err == OK ? B_NAME_NOT_FOUND : err;
//}


//sp<IBinder> Context::New(const String &component, const Variant &args,
//    uint32_t flags, status_t* outError) const
//{
//    return RemoteNew(component, SLooper::Process(), args, flags, outError);
//}

//sp<IBinder> Context::RemoteNew(const Value &component, const sp<IProcess>& process,
//    const Value &args, uint32_t flags, status_t* outError) const
//{
//    // XXX Note that we should be caching a lot of the information we retrieve
//    // here, so we don't have to do all this work for every component instantiation.
//    String id;
//    Value allArgs, componentInfo;

//    parse_component(component, args, &id, &allArgs);

//    sp<IProcessManager> pmgr = interface_cast<IProcessManager>(
//        SNode(m_root).Walk(kProcesses, (uint32_t)0));
//    if (pmgr != NULL) {
//        status_t err;
//        sp<IBinder> obj = pmgr->NewIfRemote(m_root, id, allArgs, flags, process, &componentInfo, &err);
//        if (obj != NULL || err != OK) {
//            if (outError) *outError = err;
//            return obj;
//        }
//    } else {
//        status_t err = LookupComponent(id, &componentInfo);
//        if (err != OK) {
//            if (outError != NULL) *outError = err;
//            return NULL;
//        }
//    }

//    sp<IProcess> realProcess(process);

//    // XXX I don't know if this is exactly what we want to do...  it means
//    // that the new component for the process is a child of ours, which is
//    // both good and bad...  Maybe there should be some other flag that you
//    // want it to be in its own process that is independent of this one.
//    if ((flags&PROCESS_MASK) == PREFER_REMOTE || ((flags&PROCESS_MASK) == REQUIRE_REMOTE)) {
//        // The caller has asked that the component go in some other process, but the
//        // package manager didn't do that.  Instead, what we will do is create a new
//        // process in which to instantiate it.
//        realProcess = NewProcess(id, flags, B_UNDEFINED_VALUE, outError);
//        if (realProcess == NULL) return NULL;
//    }

//    return realProcess->InstantiateComponent(m_root, componentInfo, id, allArgs, outError);
//}

//static String find_executable(const String& executable)
//{
//    // First try to find the given executable in the current path...
//    const char* path = getenv("PATH");
//    if (!path) path = "/bin/usr/bin";
//    String fullPath;
//    while (path && *path) {
//        const char* sep = strchr(path, ':');
//        if (sep && sep != path) {
//            fullPath.SetTo(path, sep-path);
//            fullPath.PathAppend(executable);
//            int fd = open(fullPath.String(), O_RDONLY);
//            if (fd >= 0) {
//                close(fd);
//                return fullPath;
//            }
//        }
//        path = sep ? sep+1 : NULL;
//    }

//    // If we didn't find it in the path, try some other
//    // standard places.
//    fullPath = get_system_directory();
//    fullPath.PathAppend("bin");
//    fullPath.PathAppend(executable);
//    int fd = open(fullPath.String(), O_RDONLY);
//    if (fd >= 0) {
//        close(fd);
//        return fullPath;
//    }

//    return String();
//}

//sp<IProcess> Context::NewProcess(const String& name, uint32_t flags, const Value& env, status_t* outError) const
//{
//    if (outError) *outError = OK;

//    if ((flags&PROCESS_MASK) == PREFER_LOCAL || (flags&PROCESS_MASK) == REQUIRE_LOCAL) {
//        return SLooper::Process();
//    }

//    if (!SLooper::PrefersProcesses() && (flags&PROCESS_MASK) != REQUIRE_REMOTE) {
//        return SLooper::Process();
//    }
//    // If we ignored the preferres flag because REQUIRE_REMOTE is
//    // set, then fail if we actually don't even support processes.
//    if (!SLooper::SupportsProcesses()) {
//        if (outError) *outError = B_UNSUPPORTED;
//        return NULL;
//    }

//    // XXX BINDER_PROCESS_WRAPPER doesn't currently work, because we need to get
//    // the pid of the real "binderproc" being run, not the wrapper command.
//    String wrapper(getenv("BINDER_PROCESS_WRAPPER"));
//    String executable("binderproc");

//    executable = find_executable(executable);
//    if (executable == "") {
//        if (outError) *outError = B_ENTRY_NOT_FOUND;
//        return NULL;
//    }

//    SVector<String> args;
//    if (wrapper == "") {
//        wrapper = executable;
//    } else {
//        args.AddItem(executable);
//    }
//    args.AddItem(name);

//    return interface_cast<IProcess>(NewCustomProcess(wrapper, args, flags, env, outError));
//}

//struct env_entry
//{
//    const char*	var;
//    String		buf;
//};

//B_IMPLEMENT_SIMPLE_TYPE_FUNCS(env_entry);

//inline int32_t BCompare(const env_entry& v1, const env_entry& v2)
//{
//    const char* s1 = v1.var ? v1.var : v1.buf.String();
//    const char* s2 = v2.var ? v2.var : v2.buf.String();
//    while (*s1 != 0 && *s1 != '=' && *s2 != 0 && *s2 != '=' && *s1 == *s2) {
//        s1++;
//        s2++;
//    }
//    //printf("String %s @ %ld vs %s @ %ld\n", v1.var, s1-v1.var, v2.var, s2-v2.var);
//    return int32_t(*s1) - int32_t(*s2);
//}

//inline bool BLessThan(const env_entry& v1, const env_entry& v2)
//{
//    return BCompare(v1, v2) < 0;
//}

//sp<IBinder> Context::NewCustomProcess(const String& executable, const SVector<String>& inArgs, uint32_t flags, const Value& env, status_t* outError) const
//{
//    if (outError) *outError = OK;

//    // XXX How can we handle BINDER_SINGLE_PROCESS here?
//    size_t i;

//    String fullPath(find_executable(executable));
//    if (fullPath == "") {
//        if (outError) *outError = B_ENTRY_NOT_FOUND;
//        return NULL;
//    }

//    SVector<const char*> argv;
//    argv.AddItem(executable.String());
//    for (i=0; i<inArgs.CountItems(); i++) argv.AddItem(inArgs[i].String());
//    argv.AddItem(NULL);

//    const char** newEnv = const_cast<const char**>(environ);
//    SSortedVector<env_entry> entries;
//    const char** allocEnv = NULL;
//#if 0
//    if (env.IsDefined() || (flags&B_FORGET_CURRENT_ENVIRONMENT)) {
//        env_entry ent;
//        if (!(flags&B_FORGET_CURRENT_ENVIRONMENT)) {
//            const char** e = newEnv;
//            while (e && *e) {
//                ent.var = *e;
//                entries.AddItem(ent);
//                e++;
//            }
//            //for (size_t i=0; i<entries.CountItems(); i++) {
//            //	bout << "Old Environment: " << entries[i].var << endl;
//            //}
//        }
//        void* i = NULL;
//        BValue k, v;
//        ent.var = NULL;
//        while (env.GetNextItem(&i, &k, &v) >= OK) {
//            if (!k.IsWild()) {
//                ent.buf = k.AsString();
//                if (ent.buf != "") {
//                    ent.buf += "=";
//                    entries.RemoveItemFor(ent);
//                    if (!v.IsWild()) {
//                        // If v is wild, the entry is just removed.
//                        ent.buf += v.AsString();
//                        entries.AddItem(ent);
//                    }
//                }
//            }
//        }
//        const size_t N = entries.CountItems();
//        allocEnv = static_cast<const char**>(malloc(sizeof(char*)*(N+1)));
//        if (allocEnv) {
//            for (size_t i=0; i<N; i++) {
//                const env_entry& e = entries[i];
//                allocEnv[i] = e.var ? e.var : e.buf.String();
//                //bout << "New Environment: " << allocEnv[i] << endl;
//            }
//            allocEnv[N] = NULL;
//            newEnv = allocEnv;
//        }
//    }
//#endif

//    pid_t pid = fork();

//    if (pid == 0)
//    {
//        int err = execve(fullPath.String(), const_cast<char**>(argv.Array()), const_cast<char**>(newEnv));
//        if (err < 0) {
//            berr << "**** EXECV of " << fullPath << " returned err = " << strerror(errno)
//                << " (" << (void*)errno << ")" << endl;
//            // XXX What to do on error?  This probably won't work,
//            // because we are using the same Binder file descriptor as our parent.
//            SLooper::This()->SendRootObject(NULL);
//            DbgOnlyFatalError("execve failed!");
//            _exit(errno);
//        }
//    }
//    else if (pid < 0)
//    {
//        if (outError) *outError = errno;
//        return NULL;
//    }

//    return SLooper::This()->ReceiveRootObject(pid);
//}

//SValue Context::Lookup(const String& location) const
//{
//    const Value value = SNode(m_root).Walk(location);

////	bout << "*** Lookup() location = " << location << endl;
////	bout << "             value = " << value << endl;
//    return value;
//}

//status_t Context::Publish(const String& location, const Value& item) const
//{
////	bout << "+++ Publish() location = " << location << endl;
////	bout << "              item = " << item << endl;

//    String path;
//    location.PathGetParent(&path);

//    Value value = SNode(m_root).Walk(&path, INode::CREATE_CATALOG);
//    status_t err;
//    sp<ICatalog> catalog = ICatalog::AsInterface(value, &err);
////		bout << "              catalog = " << catalog << " value = " << value << endl;
//    if (catalog == NULL) return err;

//    String leaf(location.PathLeaf());
////	bout << "              adding = " << leaf << endl;
//    return catalog->AddEntry(leaf, item);
//}

//status_t Context::Unpublish(const String& location) const
//{
////	bout << "*** Unpublishing: '" << location << "'" << endl;

//    String path;
//    location.PathGetParent(&path);

//    Value value = SNode(m_root).Walk(&path, (uint32_t)0);
//    status_t err;
//    sp<ICatalog> catalog = ICatalog::AsInterface(value, &err);
//    if (catalog == NULL) return err;

//    String leaf(location.PathLeaf());
//    return catalog->RemoveEntry(leaf);
//}

//sp<INode> Context::Root() const
//{
//    return m_root;
//}

//status_t Context::LookupComponent(const String& id, Value* out_info) const
//{
//    String path("/packages/components");
//    path.PathAppend(id);

//    if (id.Length() == 0) return B_BAD_VALUE;

//    status_t err;
//    *out_info = SNode(m_root).Walk(&path, &err);
////	bout << "+++ BConext::LookupComponent() err = " << strerror(err) << endl;
//    if (err < OK) return err;

////	bout << "+++ BContext::LookupComponent() id = " << id << " out_info = " << *out_info << endl;
//    return out_info->IsDefined() ? OK : B_NAME_NOT_FOUND;
//}

//SValue Context::LookupComponentProperty(const String &component, const String &property) const
//{
//    status_t err;
//    Value value;

//    err = LookupComponent(component, &value);
//    if (err != OK) return Value::Int32(err);

//    return value[SValue::String(property)];
//}

//B_CONST_STRING_VALUE_LARGE(BSH_COMPONENT, "org.openbinder.tools.BinderShell", );
//B_CONST_STRING_VALUE_SMALL(key_sh, "sh", );
//B_CONST_STRING_VALUE_LARGE(key___login, "--login", );
//B_CONST_STRING_VALUE_SMALL(key__s, "-s", );

//status_t Context::RunScript(const String &filename) const
//{
//    sp<ICommand> shell = ICommand::AsInterface(New(BSH_COMPONENT));

//    shell->SetByteInput(NullByteInput());
//    shell->SetByteOutput(StandardByteOutput());
//    shell->SetByteError(StandardByteError());

//    ICommand::ArgList args;
//    args.AddItem(key_sh);
//    args.AddItem(key___login);
//    args.AddItem(key__s);
//    args.AddItem(SValue::String(filename));

//    status_t err = shell->Run(args).AsInt32(); // this returns 10 if there was an error
//    return err == OK ? OK : B_ERROR;
//}

//B_CONST_STRING_VALUE_LARGE(key_user, "user", );
//B_CONST_STRING_VALUE_LARGE(key_system, "system", );

//Context Context::UserContext()
//{
//    return GetContext(key_user);
//}

//Context Context::SystemContext()
//{
//    return GetContext(key_system);
//}

//Context Context::GetContext(const String& name)
//{
//    return GetContext(name, SLooper::This()->Process());
//}

//Context Context::GetContext(const String& name, const sp<IProcess>& caller)
//{
//    return SLooper::GetContext(name, caller->AsBinder());
//}

} } // namespace os::support
