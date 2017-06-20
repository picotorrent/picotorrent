#include "WelcomePage.hpp"

#include <memory>
#include <picotorrent/api.hpp>

#include <atlctrls.h>

#include "../Sources/qBittorrentSource.hpp"
#include "../Sources/uTorrentSource.hpp"
#include "WizardState.hpp"

using Wizard::WelcomePage;

WelcomePage::WelcomePage(std::shared_ptr<IPicoTorrent> pico, std::shared_ptr<Wizard::WizardState> state)
    : m_pico(pico),
    m_state(state)
{
    m_title = L"Select application to import torrents from";
    SetHeaderTitle(m_title.c_str());
}

void WelcomePage::OnQBittorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/)
{
    m_state->source = std::make_shared<Sources::qBittorrentSource>();
    GetPropertySheet().PressButton(PSBTN_NEXT);
}

void WelcomePage::OnUTorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/)
{
    m_state->source = std::make_shared<Sources::uTorrentSource>();
    GetPropertySheet().PressButton(PSBTN_NEXT);
}

BOOL WelcomePage::OnSetActive()
{
    PropSheet_ShowWizButtons(m_hWnd, 0, PSWIZB_NEXT | PSWIZB_FINISH | PSWIZB_CANCEL);
    PropSheet_ShowWizButtons(m_hWnd, PSWIZB_CANCEL, PSWIZB_CANCEL);
    PropSheet_EnableWizButtons(m_hWnd, PSWIZB_CANCEL, PSWIZB_CANCEL);

    return TRUE;
}
