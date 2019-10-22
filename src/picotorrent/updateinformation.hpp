#pragma once

#include <QString>

namespace pt
{
    struct UpdateInformation
    {
        bool available;
        QString version;
        QString url;
    };
}
