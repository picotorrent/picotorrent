#include "preferenceslabelspage.hpp"

#include <wx/filepicker.h>
#include <wx/listctrl.h>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../translator.hpp"

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesLabelsPage;

struct Label
{
    std::string name;
    std::string color;
    std::string savePath;
};

PreferencesLabelsPage::PreferencesLabelsPage(wxWindow* parent, std::shared_ptr<Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
    auto labelsListSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("labels"));

    m_labelsList = new wxListView(labelsListSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_labelsList->AppendColumn(i18n("name"), wxLIST_FORMAT_LEFT, FromDIP(200));

    auto buttonsSizer = new wxBoxSizer(wxVERTICAL);
    auto addLabel = new wxButton(labelsListSizer->GetStaticBox(), wxID_ANY, "+");
    auto removeLabel = new wxButton(labelsListSizer->GetStaticBox(), wxID_ANY, "-");
    buttonsSizer->Add(addLabel);
    buttonsSizer->Add(removeLabel);

    labelsListSizer->Add(m_labelsList, 1, wxEXPAND | wxALL, FromDIP(5));
    labelsListSizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, FromDIP(5));

    auto labelDetailsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("label_details"));

    m_name = new wxTextCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_color = new wxTextCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_savePath = new wxDirPickerCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);

    auto labelDetailsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(25));
    labelDetailsGrid->AddGrowableCol(1, 1);
    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("name")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    labelDetailsGrid->Add(m_name, 1, wxEXPAND | wxALL, FromDIP(3));
    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("color")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    labelDetailsGrid->Add(m_color, 1, wxEXPAND | wxALL, FromDIP(3));
    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("save_path")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    labelDetailsGrid->Add(m_savePath, 1, wxEXPAND | wxALL, FromDIP(3));

    labelDetailsSizer->Add(labelDetailsGrid, 1, wxEXPAND);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(labelsListSizer, 0, wxEXPAND);
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(labelDetailsSizer, 0, wxEXPAND);

    this->SetSizerAndFit(sizer);

    m_name->Enable(false);
    m_color->Enable(false);
    m_savePath->Enable(false);

    addLabel->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            int row = m_labelsList->GetItemCount();
            std::string name = "Label #" + std::to_string(row + 1);

            auto lbl = new Label();
            lbl->name = name;

            m_labelsList->InsertItem(row, name);
            m_labelsList->SetItemPtrData(row, reinterpret_cast<wxUIntPtr>(lbl));
            m_labelsList->Select(row);
        });

    m_labelsList->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [this](wxCommandEvent&)
        {
            m_name->Enable(true);
            m_color->Enable(true);
            m_savePath->Enable(true);

            auto label = reinterpret_cast<Label*>(
                m_labelsList->GetItemData(
                    m_labelsList->GetFirstSelected()));

            m_name->SetValue(label->name);
            m_color->SetValue(label->color);
            m_savePath->SetPath(label->savePath);
        });

    m_labelsList->Bind(
        wxEVT_LIST_ITEM_DESELECTED,
        [this](wxCommandEvent&)
        {
            m_name->Enable(false);
            m_color->Enable(false);
            m_savePath->Enable(false);
            m_name->SetValue("");
            m_color->SetValue("");
            m_savePath->SetPath("");
        });

    m_name->Bind(
        wxEVT_TEXT,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Label*>(m_labelsList->GetItemData(sel));
            label->name = m_name->GetValue();
            m_labelsList->SetItemText(sel, label->name);
        });

    m_color->Bind(
        wxEVT_TEXT,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Label*>(m_labelsList->GetItemData(sel));
            label->color = m_color->GetValue();
        });

    m_savePath->Bind(
        wxEVT_DIRPICKER_CHANGED,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Label*>(m_labelsList->GetItemData(sel));
            label->savePath = m_savePath->GetPath();
        });
}

void PreferencesLabelsPage::Save()
{
}

bool PreferencesLabelsPage::IsValid()
{
    return true;
}
