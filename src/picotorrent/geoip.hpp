#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <set>

#include <maxminddb.h>

namespace pt
{
    class Geoip
    {
    public:
        static std::string GetCode(const std::string& ipAddress);
        static bool HasFlag(const std::string& code);
    private:
        static bool m_dbOpen;
        static std::set<std::string> m_noImageCountry;
        static MMDB_s m_mmdb;
    };
}