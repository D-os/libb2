package os.services;

import os.storage.entry_ref;

interface IRegistrarService {
    int addApplication(@utf8InCpp String mime_sig, in entry_ref ref, int flags, int team, int thread);
    void listApplications();
    void getApplication();
}
