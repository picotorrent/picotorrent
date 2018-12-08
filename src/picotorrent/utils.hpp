#pragma once
#include <QString>

namespace pt
{
    class Utils
    {
    public:
        static QString ToHumanFileSize(int64_t bytes);
    };
}
