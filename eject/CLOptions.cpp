#include "StdAfx.h"
#include "CLOptions.h"


CCLOptions::CCLOptions(int argc, _TCHAR* argv[])
{
    Usage = false;
    Help = false;
    CloseDrives = false;
    Threaded = false;
    LockDevice = false;
    Errors = _T("");

    if (argc < 2)
    {
        Usage = true;
        return;
    }

    Drives.clear();

    NextOption nextOption = NextOpt_None;
    NextArgs nextArg = NextArg_Drive; 
            
    int iArg = 1;
    while(iArg < argc)
    {
        TString strArg = argv[iArg];
        TString strNamedArg;

        switch (nextOption)
        {
            case NextOpt_None:
                if (strArg[0] == '-' || strArg[0] == '/')
                {
                    for (size_t i = 1; i < strArg.length(); i++)
                    {
                        switch (strArg[i])
                        {
                            case 'h':
                            case 'H':
                            case '?':
                                Help = true;
                                break;

                            case 'c':
                            case 'C':
                                CloseDrives = true;
                                break;

                            case 't':
                            case 'T':
                                Threaded = true;
                                break;

                            case 'l':
                            case 'L':
                                LockDevice = true;
                                break;

                            case '-':
                                strNamedArg = strArg.substr(i + 1);
                                if (strNamedArg == _T("help"))
                                    Help = true;
                                else
                                    _ftprintf(stderr, _T("Warning:  Ignoring unknown option: --%s\n"), strNamedArg.c_str());
                                i = strArg.length();
                                break;
                            default:
                                _ftprintf(stderr, _T("Warning:  Ignoring unknown option: -%c\n"), strArg[i]);
                                break;
                        }
                    }
                }
                else switch (nextArg)
                    {
                        case NextArg_Drive:
                            Drives.push_back(strArg);
                            break;
                        default:
                            _ftprintf(stderr, _T("Warning: Ignoring extra command line parameters\n"));
                            break;
                    }
                break;
            default:
                _ftprintf(stderr, _T("Error: Unknown next option\n"));
                break;
        }
        iArg++;
    }

    if (Drives.size() < 1)
    {
        Errors.append(_T("Error: No drives specified\n"));
    }
}


CCLOptions::~CCLOptions(void)
{
}
