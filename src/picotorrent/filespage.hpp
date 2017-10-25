#pragma once

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDataViewCtrl;
class wxDataViewEvent;

namespace libtorrent
{
	struct torrent_status;
}

namespace pt
{
	class FileStorageViewModel;
	class Translator;

    class FilesPage : public wxPanel
    {
	public:
		FilesPage(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);
		void Update(libtorrent::torrent_status const& ts);

	private:
		enum {
			ptID_FILE_LIST = wxID_HIGHEST
		};

		struct HandleWrapper;

		wxDECLARE_EVENT_TABLE();

		void OnFileContextMenu(wxDataViewEvent&);
		void OnSetPriority(wxCommandEvent&);

		std::shared_ptr<Translator> m_trans;
		std::unique_ptr<HandleWrapper> m_wrapper;
		wxDataViewCtrl* m_filesView;
		FileStorageViewModel* m_viewModel;
    };
}

