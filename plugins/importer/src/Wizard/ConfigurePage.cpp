#include "ConfigurePage.hpp"

#include <memory>

#include <atlctrls.h>

#include "../Sources/qBittorrentSource.hpp"

using Wizard::ConfigurePage;

ConfigurePage::ConfigurePage()
	: m_source(std::make_unique<Sources::qBittorrentSource>())
{
    m_title = L"Configure";
    SetHeaderTitle(m_title.c_str());
}

ConfigurePage::~ConfigurePage()
{
}

LRESULT ConfigurePage::OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    /*PWSTR buf;
    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &buf) != S_OK)
    {
        return TRUE;
    }

    TCHAR qb[MAX_PATH];
    PathCombine(qb, buf, TEXT("qBittorrent"));
    PathCombine(qb, qb, TEXT("BT_backup"));

    SetDlgItemText(1313, qb);

    CoTaskMemFree(buf);
*/
    return TRUE;
}

BOOL ConfigurePage::OnSetActive()
{
	HWND hWnd = m_source->GetWindowHandle(ModuleHelper::GetResourceInstance(), m_hWnd);
	::ShowWindow(hWnd, SW_SHOW);

    return TRUE;
}
