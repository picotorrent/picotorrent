#pragma once

#include <memory>

namespace Sources
{
    struct Source;
}

namespace Wizard
{
    struct WizardState
    {
        std::shared_ptr<Sources::Source> source;
    };
}
