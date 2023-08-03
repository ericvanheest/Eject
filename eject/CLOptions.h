#pragma once

#include "stdafx.h"

class CCLOptions
{
public:
    CCLOptions(int argc, _TCHAR* argv[]);
    ~CCLOptions(void);

public:
    enum NextOption { NextOpt_None };
    enum NextArgs { NextArg_None, NextArg_Drive }; 

    bool Usage;
    bool Help;
    bool CloseDrives;
    bool LockDevice;
    bool Threaded;
    TString Errors;
    std::vector<TString> Drives;

private:

};

