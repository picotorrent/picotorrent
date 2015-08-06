#ifdef _DEBUG
#pragma comment(lib, "libboost_random-vc140-mt-gd-1_58.lib")
#else
#pragma comment(lib, "libboost_random-vc140-mt-1_58.lib")
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "picotorrent.h"

IMPLEMENT_APP(PicoTorrent)
