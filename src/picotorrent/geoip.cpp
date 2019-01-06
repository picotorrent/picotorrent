#include "geoip.hpp"

#include <filesystem>
#include <fstream>

#include "utils.hpp"

namespace fs = std::experimental::filesystem::v1;

using pt::Geoip;

MMDB_s Geoip::m_mmdb={};
bool Geoip::m_dbOpen=[]()->bool {
    const std::string mmdbFileName=pt::Utils::SaveResourceToFile("MMDB").string();
    int openStatus=MMDB_open(mmdbFileName.c_str(), MMDB_MODE_MMAP, &Geoip::m_mmdb);
    if(MMDB_SUCCESS != openStatus) {
        wxLogWarning("Can't open MMDB %s - %s\nGeoip is unavailable.\n",
            mmdbFileName, MMDB_strerror(openStatus));
        return false;
    }
    else
        return true;
}();

std::string Geoip::GetCode(const std::string & ipAddress)
{
    if(m_dbOpen)
    {
        int gai_error, mmdb_error;
        MMDB_lookup_result_s result = MMDB_lookup_string(&m_mmdb, ipAddress.c_str(), &gai_error, &mmdb_error);
        if(0 != gai_error) {
            wxLogVerbose("Error from getaddrinfo for %s - %s\n\n", ipAddress, gai_strerrorA(gai_error));
        }
        else
        {
            if(MMDB_SUCCESS != mmdb_error) {
                wxLogVerbose("Got an error from libmaxminddb: %s\n\n", MMDB_strerror(mmdb_error));
            }
            else
            {
                if(result.found_entry) {
                    MMDB_entry_data_s entry_data;
                    int status;
                    status=MMDB_get_value(&result.entry, &entry_data, "country", "iso_code", NULL);
                    if(MMDB_SUCCESS != status) {
                        wxLogVerbose("Got an error looking up the entry data - %s\n", MMDB_strerror(status));
                    }
                    else
                    {
                        if(entry_data.has_data) {
                            return std::string(entry_data.utf8_string, entry_data.data_size);
                        }
                    }
                }
                else {
                    wxLogVerbose("No entry for this IP address (%s) was found\n", ipAddress);
                }
            }
        }
    }
    return std::string();
}
