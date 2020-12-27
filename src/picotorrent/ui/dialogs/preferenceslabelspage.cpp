#include "preferenceslabelspage.hpp"

#include <wx/clrpicker.h>
#include <wx/filepicker.h>
#include <wx/listctrl.h>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesLabelsPage;

PreferencesLabelsPage::PreferencesLabelsPage(wxWindow* parent, std::shared_ptr<Configuration> cfg)
    : wxPanel(parent, wxID_ANY),
    m_cfg(cfg)
{
    auto labelsListSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("labels"));

    m_labelsList = new wxListView(labelsListSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_labelsList->AppendColumn(i18n("name"), wxLIST_FORMAT_LEFT, FromDIP(280));

    auto buttonsSizer = new wxBoxSizer(wxVERTICAL);
    auto addLabel = new wxButton(labelsListSizer->GetStaticBox(), wxID_ANY, "+");
    auto removeLabel = new wxButton(labelsListSizer->GetStaticBox(), wxID_ANY, "-");
    buttonsSizer->Add(addLabel);
    buttonsSizer->Add(removeLabel);

    labelsListSizer->Add(m_labelsList, 1, wxEXPAND | wxALL, FromDIP(5));
    labelsListSizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, FromDIP(5));

    auto labelDetailsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("label_details"));

    m_name = new wxTextCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_colorEnabled = new wxCheckBox(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_colorPicker = new wxColourPickerCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, *wxBLACK);
    m_savePath = new wxDirPickerCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL);
    m_savePathEnabled = new wxCheckBox(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_applyFilter = new wxTextCtrl(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_applyFilterEnabled = new wxCheckBox(labelDetailsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);

    auto labelDetailsGrid = new wxFlexGridSizer(2, FromDIP(4), FromDIP(25));
    labelDetailsGrid->AddGrowableCol(1, 1);
    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("name")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    labelDetailsGrid->Add(m_name, 1, wxEXPAND | wxALL, FromDIP(3));

    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("color")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    auto colorSizer = new wxBoxSizer(wxHORIZONTAL);
    colorSizer->Add(m_colorEnabled, 0, wxALIGN_CENTER_VERTICAL);
    colorSizer->Add(m_colorPicker);
    labelDetailsGrid->Add(colorSizer, 1, wxALL, FromDIP(3));

    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("save_path")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    auto savePathSizer = new wxBoxSizer(wxHORIZONTAL);
    savePathSizer->Add(m_savePathEnabled, 0, wxALIGN_CENTER_VERTICAL);
    savePathSizer->Add(m_savePath, 1, wxEXPAND);
    labelDetailsGrid->Add(savePathSizer, 1, wxEXPAND | wxALL, FromDIP(3));

    labelDetailsGrid->Add(new wxStaticText(labelDetailsSizer->GetStaticBox(), wxID_ANY, i18n("apply_filter")), 0, wxALL | wxALIGN_CENTER_VERTICAL, FromDIP(3));
    auto applyFilterSizer = new wxBoxSizer(wxHORIZONTAL);
    applyFilterSizer->Add(m_applyFilterEnabled, 0, wxALIGN_CENTER_VERTICAL);
    applyFilterSizer->Add(m_applyFilter, 1, wxEXPAND);
    labelDetailsGrid->Add(applyFilterSizer, 1, wxALL, FromDIP(3));

    labelDetailsSizer->Add(labelDetailsGrid, 1, wxEXPAND);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(labelsListSizer, 0, wxEXPAND);
    sizer->AddSpacer(FromDIP(7));
    sizer->Add(labelDetailsSizer, 0, wxEXPAND);

    this->SetSizerAndFit(sizer);
    this->EnableDisableAll(false);

    removeLabel->Enable(false);

    // add labels to list view
    for (auto const& label : m_cfg->GetLabels())
    {
        int row = m_labelsList->GetItemCount();
        m_labelsList->InsertItem(row, Utils::toStdWString(label.name));
        m_labelsList->SetItemPtrData(row, reinterpret_cast<wxUIntPtr>(new Configuration::Label(label)));
    }

    addLabel->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            int row = m_labelsList->GetItemCount();
            std::string name = "Label #" + std::to_string(row + 1);

            auto lbl = new Configuration::Label();
            lbl->id = -1;
            lbl->name = name;

            m_labelsList->InsertItem(row, name);
            m_labelsList->SetItemPtrData(row, reinterpret_cast<wxUIntPtr>(lbl));
            m_labelsList->Select(row);
        });

    removeLabel->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            m_removedLabels.push_back(label->id);

            delete label;

            m_labelsList->DeleteItem(sel);
        });

    m_labelsList->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [this, removeLabel](wxCommandEvent&)
        {
            removeLabel->Enable(true);
            this->EnableDisableAll(true);

            auto label = reinterpret_cast<Configuration::Label*>(
                m_labelsList->GetItemData(
                    m_labelsList->GetFirstSelected()));

            m_name->SetValue(Utils::toStdWString(label->name));

            m_colorEnabled->SetValue(label->colorEnabled);

            if (!label->color.empty())
            {
                m_colorPicker->SetColour(wxColor(label->color));
            }

            m_savePath->Enable(label->savePathEnabled);
            m_savePath->SetPath(Utils::toStdWString(label->savePath));
            m_savePathEnabled->SetValue(label->savePathEnabled);

            m_applyFilter->Enable(label->applyFilterEnabled);
            m_applyFilter->SetValue(label->applyFilter);
            m_applyFilterEnabled->SetValue(label->applyFilterEnabled);
        });

    m_labelsList->Bind(
        wxEVT_LIST_ITEM_DESELECTED,
        [this, removeLabel](wxCommandEvent&)
        {
            removeLabel->Enable(false);
            this->EnableDisableAll(false);
        });

    m_name->Bind(
        wxEVT_TEXT,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->name = Utils::toStdString(m_name->GetValue().wc_str());
            m_labelsList->SetItemText(sel, Utils::toStdWString(label->name));
        });

    m_colorEnabled->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->colorEnabled = m_colorEnabled->GetValue();
            m_colorPicker->Enable(m_colorEnabled->GetValue());
        });

    m_colorPicker->Bind(
        wxEVT_COLOURPICKER_CHANGED,
        [this](wxColourPickerEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->color = m_colorPicker->GetColour().GetAsString(wxC2S_HTML_SYNTAX);
        });

    m_savePathEnabled->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->savePathEnabled = m_savePathEnabled->GetValue();
            m_savePath->Enable(m_savePathEnabled->GetValue());
        });

    m_savePath->Bind(
        wxEVT_DIRPICKER_CHANGED,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->savePath = Utils::toStdString(m_savePath->GetPath().wc_str());
        });

    m_applyFilter->Bind(
        wxEVT_TEXT,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->applyFilter = m_applyFilter->GetValue();
        });

    m_applyFilterEnabled->Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent&)
        {
            long sel = m_labelsList->GetFirstSelected();
            if (sel < 0) { return; }
            auto label = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(sel));
            label->applyFilterEnabled = m_applyFilterEnabled->GetValue();
            m_applyFilter->Enable(m_applyFilterEnabled->GetValue());
        });
}

PreferencesLabelsPage::~PreferencesLabelsPage()
{
    for (int i = 0; i < m_labelsList->GetItemCount(); i++)
    {
        delete reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(i));
    }
}

void PreferencesLabelsPage::Save()
{
    for (int i = 0; i < m_labelsList->GetItemCount(); i++)
    {
        auto lbl = reinterpret_cast<Configuration::Label*>(m_labelsList->GetItemData(i));
        m_cfg->UpsertLabel(*lbl);
    }

    for (size_t i = 0; i < m_removedLabels.size(); i++)
    {
        m_cfg->DeleteLabel(m_removedLabels.at(i));
    }
}

bool PreferencesLabelsPage::IsValid()
{
    return true;
}

void PreferencesLabelsPage::EnableDisableAll(bool enabled)
{
    m_name->Enable(enabled);
    m_colorEnabled->Enable(enabled);
    m_colorPicker->Enable(enabled);
    m_savePath->Enable(enabled);
    m_savePathEnabled->Enable(enabled);
    m_applyFilter->Enable(enabled);
    m_applyFilterEnabled->Enable(enabled);

    if (!enabled)
    {
        m_name->SetValue("");
        m_colorEnabled->SetValue(false);
        m_colorPicker->SetColour(*wxBLACK);
        m_savePath->SetPath("");
        m_savePathEnabled->SetValue(false);
        m_applyFilter->SetValue("");
        m_applyFilterEnabled->SetValue(false);
    }
}
