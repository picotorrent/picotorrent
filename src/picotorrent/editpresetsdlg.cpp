#include "editpresetsdlg.hpp"

#include <wx/dataview.h>

#include "config.hpp"
#include "preset.hpp"
#include "translator.hpp"

using pt::EditPresetsDialog;

wxBEGIN_EVENT_TABLE(EditPresetsDialog, wxDialog)
    EVT_BUTTON(wxID_ADD, EditPresetsDialog::OnAdd)
    EVT_BUTTON(wxID_REMOVE, EditPresetsDialog::OnRemove)
    EVT_DATAVIEW_SELECTION_CHANGED(ptID_PRESETS_LIST, EditPresetsDialog::OnSelectionChanged)
wxEND_EVENT_TABLE()

EditPresetsDialog::EditPresetsDialog(wxWindow* parent,
    std::shared_ptr<pt::Configuration> cfg,
    std::shared_ptr<pt::Translator> tr)
    : wxDialog(parent, wxID_ANY, i18n(tr, "edit_presets"), wxDefaultPosition, wxSize(350, 250), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_cfg(cfg),
    m_translator(tr)
{
    this->SetName("EditPresetsDialog");
    this->SetMinSize(this->GetSize());

    wxPanel* pnl = new wxPanel(this, wxID_ANY);

    m_presetsList = new wxDataViewListCtrl(pnl, ptID_PRESETS_LIST, wxDefaultPosition, wxSize(200, 300));
    m_presetsList->AppendTextColumn(i18n(tr, "name"));

    m_addButton = new wxButton(pnl, wxID_ADD);
    m_closeButton = new wxButton(pnl, wxID_OK);
    m_removeButton = new wxButton(pnl, wxID_REMOVE);
    m_removeButton->Enable(false);

    std::vector<Preset> presets = cfg->Presets()->GetAll();

    for (size_t i = 0; i < presets.size(); i++)
    {
        wxVector<wxVariant> item;
        item.push_back(wxVariant(presets.at(i).name));

        m_presetsList->AppendItem(item, i);
    }
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    buttonSizer->Add(m_addButton);
    buttonSizer->Add(m_removeButton);
    buttonSizer->Add(m_closeButton);

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_presetsList, 1, wxALL | wxEXPAND, 5);
    sizer->Add(buttonSizer, 0, wxALL, 5);

    pnl->SetSizerAndFit(sizer);
}

void EditPresetsDialog::OnAdd(wxCommandEvent&)
{
    wxTextEntryDialog dlg(
        GetParent(),
        i18n(m_translator, "preset_name"));

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    Preset p;
    p.move_completed_downloads = false;
    p.move_completed_path = "";
    p.name = dlg.GetValue();
    p.save_path = m_cfg->DefaultSavePath();

    m_cfg->Presets()->Insert(p);

    wxVector<wxVariant> item;
    item.push_back(wxVariant(p.name));

    m_presetsList->AppendItem(item, m_cfg->Presets()->GetAll().size() - 1);
}

void EditPresetsDialog::OnRemove(wxCommandEvent& e)
{
    int row = m_presetsList->GetSelectedRow();
    if (row < 0) { return; }

    wxDataViewItem& rem = m_presetsList->GetStore()->GetItem(row);
    size_t idx = static_cast<size_t>(m_presetsList->GetItemData(rem));

    m_cfg->Presets()->Remove(idx);
    m_presetsList->DeleteItem(row);
    m_removeButton->Enable(false);

    for (int i = row; i < m_presetsList->GetItemCount(); i++)
    {
        wxDataViewItem& item = m_presetsList->GetStore()->GetItem(i);

        size_t idx = static_cast<size_t>(m_presetsList->GetItemData(item));
        m_presetsList->SetItemData(item, idx - 1);
    }
}

void EditPresetsDialog::OnSelectionChanged(wxDataViewEvent& e)
{
    int row = m_presetsList->GetSelectedRow();
    m_removeButton->Enable(row >= 0);
}
