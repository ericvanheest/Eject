// eject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CLOptions.h"
#include <Windows.h>

void Usage(int argc, _TCHAR* argv[])
{
    _tprintf(_T("\
%s, version 1.0.1\n\n\
Usage:   %s [-c] [-l] [-t] drive(s)\n\
\n\
Example: %s D E F\n\
\n\
Options: -c   Close drive tray instead of ejecting\n\
         -l   Lock the device before ejecting (can cause program hang)\n\
         -t   Launch eject commands simultaneously in threads\n\
"), argv[0], argv[0], argv[0]);
}

class CThreadEjectInfo
{
public:
    CThreadEjectInfo::CThreadEjectInfo()
    {
        m_pOptions = NULL;
    }

    CCLOptions *m_pOptions;
    TString m_strPhysicalPath;
    HANDLE m_hStartedEvent;
};

DWORD WINAPI ThreadEject( LPVOID lpParam ) 
{
    CThreadEjectInfo *pInfo = (CThreadEjectInfo *) lpParam;

    TString strPath = pInfo->m_strPhysicalPath;
    CCLOptions *pOptions = pInfo->m_pOptions;

    SetEvent(pInfo->m_hStartedEvent);

    HANDLE hDrive = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE)
        goto Error;

    else
    {
        DWORD bytes = 0;

        if (pInfo->m_pOptions->LockDevice)
        {
            if (!DeviceIoControl(hDrive, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &bytes, 0))
                goto Error;
        }

        if (!DeviceIoControl(hDrive, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &bytes, 0))
            goto Error;

        if (!DeviceIoControl(hDrive, pOptions->CloseDrives ?  IOCTL_STORAGE_LOAD_MEDIA : IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &bytes, 0))
            goto Error;

        CloseHandle(hDrive);
    }

    return 0;

Error:
    if (hDrive != INVALID_HANDLE_VALUE)
        CloseHandle(hDrive);
    _tprintf(_T("Error %s %s  [%d]\n"), pOptions->CloseDrives ? _T("retracting") : _T("ejecting"), strPath.c_str(), GetLastError());
    return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::vector<HANDLE> vhThreads;

    CCLOptions options(argc, argv);
    if (options.Errors.length() > 0)
    {
        _ftprintf(stderr, options.Errors.c_str());
        return 0;
    }

    if (options.Usage || options.Help)
    {
        Usage(argc, argv);
        return 0;
    }

    CThreadEjectInfo info;
    info.m_pOptions = &options;
    info.m_hStartedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for(size_t iIndex = 0; iIndex < options.Drives.size(); iIndex++)
    {
        TString strDrive = options.Drives.at(iIndex);
        if (strDrive.length() < 1)
            continue;

        if (strDrive.length() == 1)
            strDrive.append(_T(":"));

        _tprintf(_T("%s %s\n"), options.CloseDrives ? _T("retracting") : _T("ejecting"), strDrive.c_str());

        TString strPhysicalPath = strDrive;
        if (strPhysicalPath.substr(0,2) != _T("\\\\"))
            strPhysicalPath = _T("\\\\.\\") + strPhysicalPath;

        vhThreads.clear();

        info.m_strPhysicalPath = strPhysicalPath.substr(0);

        if (options.Threaded)
        {
            DWORD dwID = 0;
            ResetEvent(info.m_hStartedEvent);
            vhThreads.push_back(CreateThread(NULL, 0, ThreadEject, &info, 0, &dwID));
            WaitForSingleObject(info.m_hStartedEvent, INFINITE);
        }
        else
        {
            ThreadEject(&info);
        }
    }

    for(size_t iThread = 0; iThread < vhThreads.size(); iThread++)
    {
        if (WaitForSingleObject(vhThreads.at(iThread), 5000) != WAIT_OBJECT_0)
        {
            _tprintf(_T("Error:  Thread %d did not exit properly.  Terminating.\n"), iThread);
            TerminateThread(vhThreads.at(iThread), 0);
        }
    }

	return 0;
}

