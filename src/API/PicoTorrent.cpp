#include "PicoTorrent.hpp"

#include "../Commands/InvokeCommand.hpp"
#include "../resources.h"
#include "TranslatorProxy.hpp"
#include "../VersionInformation.hpp"

#define EXT_MENUITEM_ID_START 10000

using API::PicoTorrent;

PicoTorrent::PicoTorrent(HWND hWndOwner)
    : m_hWndOwner(hWndOwner),
    m_threadId(std::this_thread::get_id())
{
    SetWindowSubclass(
        m_hWndOwner,
        &PicoTorrent::SubclassProc,
        1338,
        reinterpret_cast<DWORD_PTR>(this));
}

void PicoTorrent::AddMenuItem(MenuItem const& item)
{
    HMENU hMenu = GetMenu(m_hWndOwner);
    HMENU ext = GetSubMenu(hMenu, 2);

    int id = EXT_MENUITEM_ID_START + (int)m_menuItems.size();
    AppendMenu(ext, MF_STRING, id, item.text.c_str());

    m_menuItems.insert({ id, item });
}

std::string PicoTorrent::GetCurrentVersion()
{
    return VersionInformation::GetCurrentVersion();
}

std::shared_ptr<ITranslator> PicoTorrent::GetTranslator()
{
    return std::make_shared<TranslatorProxy>();
}

void PicoTorrent::ShowTaskDialog(TASKDIALOGCONFIG* tdcfg)
{
    if (std::this_thread::get_id() != m_threadId)
    {
        // Invoke
        Commands::InvokeCommand cmd;
        cmd.callback = [this, &tdcfg]() { ShowTaskDialog(tdcfg); };
        SendMessage(m_hWndOwner, PT_INVOKE, NULL, reinterpret_cast<LPARAM>(&cmd));
        return;
    }

    tdcfg->hwndParent = m_hWndOwner;
    TaskDialogIndirect(tdcfg, NULL, NULL, NULL);
}

LRESULT PicoTorrent::SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PicoTorrent* pico = reinterpret_cast<PicoTorrent*>(dwRefData);

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        DWORD dwId = LOWORD(wParam);

        if (pico->m_menuItems.find(dwId) != pico->m_menuItems.end())
        {
            pico->m_menuItems.at(dwId).onClick();
            return 0;
        }

        break;
    };
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
