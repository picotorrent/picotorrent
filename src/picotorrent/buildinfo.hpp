#pragma once

namespace pt
{
    class BuildInfo
    {
    public:
        static const char* branch();
        static const char* commitish();
        static const char* semver();
        static const char* version();
    };
}
