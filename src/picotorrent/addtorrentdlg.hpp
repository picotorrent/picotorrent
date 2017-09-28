#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

class wxDirPickerCtrl;
class wxFileDirPickerEvent;
class wxTreeListEvent;
class wxTreeListCtrl;

namespace libtorrent
{
	struct add_torrent_params;
}

namespace pt
{
    class AddTorrentDialog : public wxDialog
    {
    public:
        AddTorrentDialog(
			wxWindow* parent,
			std::vector<libtorrent::add_torrent_params>& params);

		~AddTorrentDialog();

	private:
		enum
		{
			ptID_TORRENT_LIST = wxID_HIGHEST,
			ptID_SAVE_PATH,
			ptID_TORRENT_FILE_LIST
		};

		void LoadTorrentInfo(int index);
		void OnSavePathChanged(wxFileDirPickerEvent&);
		void OnTorrentChanged(wxCommandEvent&);
		void OnTorrentFileContextMenu(wxTreeListEvent&);

		wxDECLARE_EVENT_TABLE();

		wxImageList* m_icons;
		wxChoice* m_torrents;
		wxStaticText* m_size;
		wxDirPickerCtrl* m_savePath;
		wxTreeListCtrl* m_torrentFiles;

		std::vector<libtorrent::add_torrent_params>& m_params;
    };
}
