#ifndef _PT_FSUTIL_H
#define _PT_FSUTIL_H

#include <string>
#include <vector>

class FsUtil
{
public:
    static std::string GetDataPath();

    static void ReadFile(std::string path, std::vector<char>& buffer);
};

#endif
