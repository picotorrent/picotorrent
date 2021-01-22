#include "torrents_add.hpp"

using json = nlohmann::json;
using pt::commands::torrents_add_command;

torrents_add_command::torrents_add_command(std::shared_ptr<pt::session_manager> const& session)
    : m_session(session)
{
}

json torrents_add_command::execute(json const& params)
{
    return {
        { "ok", true }
    };
}
