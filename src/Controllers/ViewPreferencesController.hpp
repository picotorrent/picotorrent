#pragma once

#include <memory>

namespace libtorrent { class session; }

namespace UI
{
    class NotifyIcon;
}

namespace Controllers
{
class ViewPreferencesController
{
public:
    ViewPreferencesController(
        const std::shared_ptr<UI::NotifyIcon>& notifyIcon,
        const std::shared_ptr<libtorrent::session>& session);
    void Execute();

private:
    std::shared_ptr<UI::NotifyIcon> m_notifyIcon;
    std::shared_ptr<libtorrent::session> m_session;
};
}
