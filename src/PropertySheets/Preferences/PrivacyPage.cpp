#include "PrivacyPage.hpp"

#include "../../Configuration.hpp"
#include "../../Translator.hpp"
#include "../../UI/CheckBox.hpp"

using PropertySheets::Preferences::PrivacyPage;

PrivacyPage::PrivacyPage()
{
    m_title = TRW("privacy");
    SetTitle(m_title.c_str());
}

BOOL PrivacyPage::OnApply()
{
    Configuration& cfg = Configuration::GetInstance();

    UI::CheckBox forceIn = GetDlgItem(ID_PRIVACY_ENCRYPTION_IN);
    UI::CheckBox forceOut = GetDlgItem(ID_PRIVACY_ENCRYPTION_OUT);

    cfg.Session()->SetRequireIncomingEncryption(forceIn.IsChecked());
    cfg.Session()->SetRequireOutgoingEncryption(forceOut.IsChecked());

    return TRUE;
}

void PrivacyPage::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    switch (nID)
    {
    case ID_PRIVACY_ENCRYPTION_IN:
    case ID_PRIVACY_ENCRYPTION_OUT:
        SetModified();
        break;
    }
}

BOOL PrivacyPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    SetDlgItemText(ID_PRIVACY_ENCRYPTION_GROUP, TRW("encryption"));
    SetDlgItemText(ID_PRIVACY_ENCRYPTION_IN, TRW("require_encryption_incoming"));
    SetDlgItemText(ID_PRIVACY_ENCRYPTION_OUT, TRW("require_encryption_outgoing"));

    Configuration& cfg = Configuration::GetInstance();
    UI::CheckBox forceIn = GetDlgItem(ID_PRIVACY_ENCRYPTION_IN);
    UI::CheckBox forceOut = GetDlgItem(ID_PRIVACY_ENCRYPTION_OUT);

    if (cfg.Session()->GetRequireIncomingEncryption())
    {
        forceIn.SetCheck(BST_CHECKED);
    }

    if (cfg.Session()->GetRequireOutgoingEncryption())
    {
        forceOut.SetCheck(BST_CHECKED);
    }

    return FALSE;
}
