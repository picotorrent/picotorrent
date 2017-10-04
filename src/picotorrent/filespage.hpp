#pragma once

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/sha1_hash.hpp>

class wxDataViewCtrl;

namespace libtorrent
{
	struct torrent_status;
}

namespace pt
{
	class FileStorageViewModel;

    class FilesPage : public wxPanel
    {
	public:
		FilesPage(wxWindow* parent, wxWindowID id);
		void Update(libtorrent::torrent_status const& ts);

	private:
		wxDataViewCtrl* m_filesView;
		FileStorageViewModel* m_viewModel;
		libtorrent::sha1_hash m_hash;
    };
}

