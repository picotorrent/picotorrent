#pragma once

#ifdef _DEBUG
#pragma comment(lib, "libboost_random-vc140-mt-gd-1_59.lib")
#else
#pragma comment(lib, "libboost_random-vc140-mt-1_59.lib")
#endif

#pragma comment(linker, "\"/manifestdependency:type='win32' \
                            name='Microsoft.Windows.Common-Controls' \
                            version='6.0.0.0' \
                            processorArchitecture='*' \
                            publicKeyToken='6595b64144ccf1df' \
                            language='*'\"")

#include <windows.h>
#pragma warning(disable: 4458 4838)
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlctrls.h>
#include <atldlgs.h>
#include <atlframe.h>
#include <atlstr.h>
#include <atlcrack.h>
#pragma warning(default: 4458 4838)

#include "resources.h"
