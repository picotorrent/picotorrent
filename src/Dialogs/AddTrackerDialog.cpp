#include "AddTrackerDialog.hpp"

#include <shellapi.h>
#include <strsafe.h>

#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../VersionInformation.hpp"

using Dialogs::AddTrackerDialog;

AddTrackerDialog::AddTrackerDialog()
{
}

AddTrackerDialog::~AddTrackerDialog()
{
}

std::wstring AddTrackerDialog::GetUrl()
{
    return m_url;
}

void AddTrackerDialog::OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (nID == IDOK)
    {
        TCHAR url[1024];
        GetDlgItemText(ID_ADD_TRACKER_URL, url, ARRAYSIZE(url));
        m_url = url;
    }

    EndDialog(nID);
}

BOOL AddTrackerDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    CenterWindow();

    // Localize
    SetWindowText(TRW("add_tracker"));
    SetDlgItemText(IDOK, TRW("add_tracker"));

    return FALSE;
}
