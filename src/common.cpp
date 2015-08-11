#include "common.h"

wxDEFINE_EVENT(SESSION_ALERT, wxCommandEvent);

const char* units[] = { "B", "KiB", "MiB", "GiB", "TiB" };

std::string Common::ToFileSize(size_t bytes)
{
    if (bytes < 0)
    {
        return "Unknown size";
    }

    int i = 0;
    double val = bytes;

    while (val >= 1024 && i < 4)
    {
        val = (val / 1024.);
        ++i;
    }

    char res[100];
    snprintf(res, sizeof(res), "%.2f %s", val, units[i]);
    return res;
}
