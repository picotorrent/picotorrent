#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <string>

namespace picotorrent
{
namespace ui
{
namespace property_sheets
{
namespace preferences
{
    class downloads_page : public property_sheet_page
    {
    public:
        downloads_page();
        
        void set_downloads_path(const std::wstring &path);
    };
}
}
}
}
