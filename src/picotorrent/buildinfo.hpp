#pragma once

#include <QString>

namespace pt
{
    class BuildInfo
    {
    public:
        static QString branch();
        static QString commitish();
        static QString version();
    };
}
