#ifndef _PT_UI_ADDTORRENTDIALOG_H
#define _PT_UI_ADDTORRENTDIALOG_H

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <map>
#include <vector>
#include <wx/frame.h>
#include <wx/panel.h>

class wxComboBox;
class wxListCtrl;
class wxListEvent;
class wxStaticText;
class wxTextCtrl;

class AddTorrentDialog : public wxFrame
{
public:
    AddTorrentDialog(wxWindow* parent,
        std::vector<boost::intrusive_ptr<libtorrent::torrent_info>> torrents);

protected:
    void InitTorrentsGroup();
    void InitStorageGroup();

private:
    typedef boost::intrusive_ptr<libtorrent::torrent_info> torrent_info_ptr;

    void OnBrowseSavePath(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnComboSelected(wxCommandEvent& event);
    void ShowTorrentInfo(int index);
    void OnFileItemRightClick(wxListEvent& event);
    void OnMenu(wxCommandEvent& event);
    void OnSavePathChanged(wxCommandEvent& event);

    std::string PriorityString(int priority);

    std::map<libtorrent::sha1_hash, libtorrent::add_torrent_params> params_;
    std::map<int, libtorrent::sha1_hash> items_;

    // Torrent
    wxComboBox* torrentsCombo_;
    wxStaticText* sizeText_;
    wxStaticText* commentText_;
    wxStaticText* dateText_;
    wxStaticText* creatorText_;

    // Storage
    wxTextCtrl* savePathText_;
    wxListCtrl* fileList_;

    wxPanel* panel_;

    enum
    {
        ptID_TORRENTS_COMBO = 1001,
        ptID_TORRENT_FILES_LIST = 1002,
        ptID_PRIO_SKIP = 2010,
        ptID_PRIO_NORMAL = 2011,
        ptID_PRIO_HIGH = 2016,
        ptID_PRIO_MAX = 2017,
        ptID_RENAME_FILE = 2020,
        ptID_SAVE_PATH = 2021,
    };

    enum
    {
        PRIO_SKIP = 0,
        PRIO_NORMAL = 1,
        PRIO_HIGH = 6,
        PRIO_MAX = 7
    };

    wxDECLARE_EVENT_TABLE();
};

#endif
