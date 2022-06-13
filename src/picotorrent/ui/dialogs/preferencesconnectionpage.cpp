#include "preferencesconnectionpage.hpp"

#include <sstream>
/*
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
*/
#include <wx/filepicker.h>
#include <wx/listctrl.h>
#include <wx/tokenzr.h>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "listeninterfacedialog.hpp"
#include "../translator.hpp"

struct NetworkAdapter
{
    wxString description;
    wxString ipv4;
    wxString ipv6;
};

struct Item
{
    int id = -1;
};

using pt::UI::Dialogs::PreferencesConnectionPage;

PreferencesConnectionPage::PreferencesConnectionPage(wxWindow* parent, std::shared_ptr<pt::Core::Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_parent(parent),
    m_cfg(cfg)
{
    wxStaticBoxSizer* listenSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("listen_interface"));
 
    m_listenInterfaces = new wxListView(listenSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_listenInterfaces->AppendColumn(i18n("address"), wxLIST_FORMAT_LEFT, FromDIP(180));
    m_listenInterfaces->AppendColumn(i18n("port"), wxLIST_FORMAT_RIGHT);

    auto buttonSizer = new wxBoxSizer(wxVERTICAL);
    auto addInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, "+");
    auto editInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, i18n("edit"));
    auto removeInterface = new wxButton(listenSizer->GetStaticBox(), wxID_ANY, "-");
    buttonSizer->Add(addInterface);
    buttonSizer->Add(editInterface);
    buttonSizer->Add(removeInterface);

    listenSizer->Add(m_listenInterfaces, 1, wxEXPAND | wxALL, FromDIP(5));
    listenSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, FromDIP(5));

    wxStaticBoxSizer* encryptionSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("encryption"));
    wxFlexGridSizer* encryptionGrid = new wxFlexGridSizer(1, 10, 10);

    m_incomingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_incoming"));
    m_incomingEncryption->SetValue(m_cfg->Get<bool>("libtorrent.require_incoming_encryption").value());

    m_outgoingEncryption = new wxCheckBox(encryptionSizer->GetStaticBox(), wxID_ANY, i18n("require_encryption_outgoing"));
    m_outgoingEncryption->SetValue(m_cfg->Get<bool>("libtorrent.require_outgoing_encryption").value());

    encryptionGrid->AddGrowableCol(0, 1);
    encryptionGrid->Add(m_incomingEncryption, 1, wxEXPAND);
    encryptionGrid->Add(m_outgoingEncryption, 1, wxEXPAND);
    encryptionSizer->Add(encryptionGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* privacySizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("privacy"));
    wxFlexGridSizer* privacyGrid = new wxFlexGridSizer(3, 10, 10);

    m_enableDht = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_dht"));
    m_enableDht->SetValue(m_cfg->Get<bool>("libtorrent.enable_dht").value());

    m_enableLsd = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_lsd"));
    m_enableLsd->SetValue(m_cfg->Get<bool>("libtorrent.enable_lsd").value());

    m_enablePex = new wxCheckBox(privacySizer->GetStaticBox(), wxID_ANY, i18n("enable_pex"));
    m_enablePex->SetValue(m_cfg->Get<bool>("libtorrent.enable_pex").value());

    privacyGrid->AddGrowableCol(0, 1);
    privacyGrid->AddGrowableCol(1, 1);
    privacyGrid->AddGrowableCol(2, 1);
    privacyGrid->Add(m_enableDht, 1, wxEXPAND);
    privacyGrid->Add(m_enableLsd, 1, wxEXPAND);
    privacyGrid->Add(m_enablePex, 1, wxEXPAND);
    privacySizer->Add(privacyGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* filterSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("ip_filter"));
    m_enableFilter = new wxCheckBox(filterSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_enableFilter->SetValue(m_cfg->Get<bool>("ipfilter.enabled").value());

    m_filterPath = new wxTextCtrl(filterSizer->GetStaticBox(), wxID_ANY, m_cfg->Get<std::string>("ipfilter.file_path").value_or(""));
    m_filterPath->Enable(m_cfg->Get<bool>("ipfilter.enabled").value());
    m_filterBrowse = new wxButton(filterSizer->GetStaticBox(), wxID_ANY, i18n("browse"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_filterBrowse->Enable(m_cfg->Get<bool>("ipfilter.enabled").value());

    auto filterInputSizer = new wxBoxSizer(wxHORIZONTAL);
    filterInputSizer->Add(m_filterPath, 1, wxEXPAND | wxRIGHT, FromDIP(2));
    filterInputSizer->Add(m_filterBrowse, 0);

    filterSizer->Add(m_enableFilter, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));
    filterSizer->Add(filterInputSizer, 1, wxEXPAND | wxALL, FromDIP(5));

    m_enableFilter->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            m_filterPath->Enable(m_enableFilter->GetValue());
            m_filterBrowse->Enable(m_enableFilter->GetValue());
        });

    m_filterBrowse->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            wxFileDialog ofd(this, i18n("select_ip_filter_file"), "", "", i18n("file_filter_all_files"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (ofd.ShowModal() != wxID_OK)
            {
                return;
            }

            m_filterPath->SetValue(ofd.GetPath());
        });

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(listenSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(encryptionSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(privacySizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(filterSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);

    for (auto const& li : cfg->GetListenInterfaces())
    {
        int row = m_listenInterfaces->GetItemCount();
        m_listenInterfaces->InsertItem(row, li.address);
        m_listenInterfaces->SetItem(row, 1, std::to_string(li.port));
        m_listenInterfaces->SetItemPtrData(row, reinterpret_cast<wxUIntPtr>(new Item { li.id }));
    }

    addInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            ListenInterfaceDialog dlg(this, wxID_ANY);

            if (dlg.ShowModal() == wxID_OK)
            {
                int row = m_listenInterfaces->GetItemCount();
                m_listenInterfaces->InsertItem(row, dlg.GetAddress());
                m_listenInterfaces->SetItem(row, 1, std::to_string(dlg.GetPort()));
                m_listenInterfaces->SetItemPtrData(row, reinterpret_cast<wxUIntPtr>(new Item()));
            }
        });

    editInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            long sel = m_listenInterfaces->GetFirstSelected();
            if (sel < 0) { return; }

            std::string address = m_listenInterfaces->GetItemText(sel).ToStdString();
            int port = std::atoi(m_listenInterfaces->GetItemText(sel, 1).c_str());

            ListenInterfaceDialog dlg(this, wxID_ANY, address, port);

            if (dlg.ShowModal() == wxID_OK)
            {
                m_listenInterfaces->SetItem(sel, 0, dlg.GetAddress());
                m_listenInterfaces->SetItem(sel, 1, std::to_string(dlg.GetPort()));
            }
        });

    removeInterface->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            long sel = m_listenInterfaces->GetFirstSelected();
            if (sel < 0) { return; }

            int id = reinterpret_cast<Item*>(m_listenInterfaces->GetItemData(sel))->id;
            m_listenInterfaces->DeleteItem(sel);

            if (id > 0)
            {
                m_removedListenInterfaces.push_back(id);
            }
        });
}

PreferencesConnectionPage::~PreferencesConnectionPage()
{
}

void PreferencesConnectionPage::Save(bool* restartRequired)
{
    for (int removed : m_removedListenInterfaces)
    {
        m_cfg->DeleteListenInterface(removed);
    }

    for (int i = 0; i < m_listenInterfaces->GetItemCount(); i++)
    {
        Core::Configuration::ListenInterface li;
        li.address = m_listenInterfaces->GetItemText(i, 0);
        li.id = reinterpret_cast<Item*>(m_listenInterfaces->GetItemData(i))->id;
        li.port = std::atoi(m_listenInterfaces->GetItemText(i, 1).c_str());

        m_cfg->UpsertListenInterface(li);
    }

    m_cfg->Set("libtorrent.require_incoming_encryption", m_incomingEncryption->GetValue());
    m_cfg->Set("libtorrent.require_outgoing_encryption", m_outgoingEncryption->GetValue());

    if (m_enablePex->GetValue() != m_cfg->Get<bool>("libtorrent.enable_pex"))
    {
        *restartRequired = true;
    }

    m_cfg->Set("libtorrent.enable_dht", m_enableDht->GetValue());
    m_cfg->Set("libtorrent.enable_lsd", m_enableLsd->GetValue());
    m_cfg->Set("libtorrent.enable_pex", m_enablePex->GetValue());

    if (m_enableFilter->GetValue() != m_cfg->Get<bool>("ipfilter.enabled"))
    {
        *restartRequired = true;
    }

    m_cfg->Set("ipfilter.enabled", m_enableFilter->GetValue());
    m_cfg->Set("ipfilter.file_path", Utils::toStdString(m_filterPath->GetValue().ToStdWstring()));
}

bool PreferencesConnectionPage::IsValid()
{
    return true;
}
