#include "trackerspage.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/clipbrd.h>
#include <wx/dataview.h>

#include "string.hpp"
#include "trackersviewmodel.hpp"
#include "translator.hpp"

namespace lt = libtorrent;
using pt::TrackersPage;

wxBEGIN_EVENT_TABLE(TrackersPage, wxPanel)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ptID_TRACKERS_LIST, TrackersPage::OnTrackersContextMenu)
    EVT_MENU(ptID_ADD, TrackersPage::OnAddTrackers)
    EVT_MENU(ptID_COPY_URL, TrackersPage::OnCopyUrl)
    EVT_MENU(ptID_FORCE_REANNOUNCE, TrackersPage::OnForceReannounce)
    EVT_MENU(ptID_REMOVE, TrackersPage::OnRemoveTrackers)
wxEND_EVENT_TABLE()

struct TrackersPage::StatusWrap
{
    lt::torrent_status status;
};

TrackersPage::TrackersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_trackersView(new wxDataViewCtrl(this, ptID_TRACKERS_LIST, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE)),
    m_viewModel(new TrackersViewModel(tr)),
    m_translator(tr),
    m_wrap(std::make_unique<TrackersPage::StatusWrap>())
{
    auto urlCol = m_trackersView->AppendTextColumn(
        i18n(tr, "url"),
        TrackersViewModel::Columns::Url,
        wxDATAVIEW_CELL_INERT,
        220);

    auto statusCol = m_trackersView->AppendTextColumn(
        i18n(tr, "status"),
        TrackersViewModel::Columns::Status,
        wxDATAVIEW_CELL_INERT,
        160);

    m_trackersView->AppendTextColumn(
        i18n(tr, "fails"),
        TrackersViewModel::Columns::Fails,
        wxDATAVIEW_CELL_INERT,
        60,
        wxALIGN_RIGHT);

    m_trackersView->AppendTextColumn(
        i18n(tr, "next_announce"),
        TrackersViewModel::Columns::NextAnnounce,
        wxDATAVIEW_CELL_INERT,
        100,
        wxALIGN_RIGHT)->SetMinWidth(100);

    m_trackersView->AssociateModel(m_viewModel);
    m_viewModel->DecRef();

    urlCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);
    statusCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_trackersView, 1, wxEXPAND);

    this->SetSizer(sizer);
}

void TrackersPage::Clear()
{
    m_wrap->status = lt::torrent_status();
    m_viewModel->Clear();
}

void TrackersPage::Update(lt::torrent_status const& ts)
{
    m_wrap->status = ts;
    m_viewModel->Update(ts);
}

void TrackersPage::OnAddTrackers(wxCommandEvent&)
{
    wxTextEntryDialog dlg(
        m_parent,
        i18n(m_translator, "add_trackers"),
        i18n(m_translator, "tracker_urls"),
        wxEmptyString,
        wxTextEntryDialogStyle | wxTE_MULTILINE);

    dlg.SetClientSize(dlg.GetClientSize().GetWidth(), 150);
    // dlg.SetTextValidator(MagnetLinkValidator());

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    auto urls = String::Split(dlg.GetValue().ToStdString(), "\n");

    for (const std::string& url : urls)
    {
        if (url.empty()) { continue; }
        m_wrap->status.handle.add_tracker(lt::announce_entry(url));
    }

    m_viewModel->Update(m_wrap->status);
}

void TrackersPage::OnCopyUrl(wxCommandEvent&)
{
    wxDataViewItemArray items;
    m_trackersView->GetSelections(items);

    std::vector<lt::announce_entry> trackers = m_wrap->status.handle.trackers();
    std::stringstream ss;

    for (const wxDataViewItem& item : items)
    {
        unsigned int row = m_viewModel->GetRow(item);
        ss << trackers.at(row).url << std::endl;
    }

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(ss.str()));
        wxTheClipboard->Close();
    }
}

void TrackersPage::OnForceReannounce(wxCommandEvent&)
{
    wxDataViewItemArray items;
    m_trackersView->GetSelections(items);

    for (const wxDataViewItem& item : items)
    {
        unsigned int row = m_viewModel->GetRow(item);
        m_wrap->status.handle.force_reannounce(0, row);
    }

    m_viewModel->Update(m_wrap->status);
}


void TrackersPage::OnRemoveTrackers(wxCommandEvent&)
{
    wxDataViewItemArray items;
    m_trackersView->GetSelections(items);

    std::vector<lt::announce_entry> trackers = m_wrap->status.handle.trackers();

    for (const wxDataViewItem& item : items)
    {
        unsigned int row = m_viewModel->GetRow(item);
        trackers.erase(trackers.begin() + row);
    }

    m_wrap->status.handle.replace_trackers(trackers);
    m_viewModel->Update(m_wrap->status);
}

void TrackersPage::OnTrackersContextMenu(wxDataViewEvent& event)
{
    if (!m_wrap->status.handle.is_valid())
    {
        return;
    }

    wxDataViewItemArray items;
    m_trackersView->GetSelections(items);

    wxMenu trackersMenu;

    if (items.Count() > 0)
    {
        trackersMenu.Append(ptID_COPY_URL, i18n(m_translator, "copy_url"));
        trackersMenu.Append(ptID_FORCE_REANNOUNCE, i18n(m_translator, "force_reannounce"));
        trackersMenu.AppendSeparator();
        trackersMenu.Append(ptID_REMOVE, i18n(m_translator, "remove"));
    }
    else
    {
        trackersMenu.Append(ptID_ADD, i18n(m_translator, "add_tracker"));
    }

    PopupMenu(&trackersMenu);
}
