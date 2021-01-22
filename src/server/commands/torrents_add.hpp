#pragma once

#include "command.hpp"

namespace pt { class session_manager; }

namespace pt::commands
{
    class torrents_add_command : public command
    {
    public:
        torrents_add_command(std::shared_ptr<session_manager> const& session);
        nlohmann::json execute(nlohmann::json const& params) override;

    private:
        std::shared_ptr<session_manager> m_session;
    };
}
