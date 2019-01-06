#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>

#include <maxminddb.h>

namespace pt
{
    class Geoip
    {
    public:
        static std::string GetCode(const std::string& ipAddress);

    private:
        static MMDB_s m_mmdb;
        static bool m_dbOpen;
    };
}