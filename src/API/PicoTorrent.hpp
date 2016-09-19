#pragma once

#include <picotorrent/api.hpp>

#include <map>
#include <thread>

namespace API
{
    class PicoTorrent : public IPicoTorrent
    {
    public:
        PicoTorrent(HWND hWndOwner);

        void AddMenuItem(MenuItem const& item);
        std::shared_ptr<picojson::object> GetConfiguration();
        std::string GetCurrentVersion();
        std::shared_ptr<ILogger> GetLogger();
        std::shared_ptr<ITranslator> GetTranslator();
        std::unique_ptr<TaskDialogResult> ShowTaskDialog(TASKDIALOGCONFIG* tdcfg);

    private:
        static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

        HWND m_hWndOwner;
        std::map<int, MenuItem> m_menuItems;
        std::thread::id m_threadId;
    };
}
