// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>

#ifdef _UNICODE
#define TString std::wstring
#else
#define TString std::string
#endif


// TODO: reference additional headers your program requires here
