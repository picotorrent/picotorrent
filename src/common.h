#ifndef _PT_COMMON_H
#define _PT_COMMON_H

#include <string>
#include <wx/event.h>

wxDECLARE_EVENT(SESSION_ALERT, wxCommandEvent);

class Common
{
public:
    static std::string ToFileSize(size_t bytes);
};

#endif
