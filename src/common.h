#ifndef _PT_COMMON_H
#define _PT_COMMON_H

#include <string>
#include <wx/event.h>

wxDECLARE_EVENT(SESSION_ALERT, wxCommandEvent);

enum MenuItem
{
    ptID_FILE_ADD_TORRENT = 4000,
    ptID_FILE_EXIT = wxID_EXIT,

    ptID_VIEW_LOG = 4100
};

class Common
{
public:
    static std::string ToFileSize(size_t bytes);
};

#endif
