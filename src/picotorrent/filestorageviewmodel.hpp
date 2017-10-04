#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
#include <memory>

#include <wx/dataview.h>

namespace libtorrent
{
	class file_storage;
	class torrent_info;
}

namespace pt
{
    class FileStorageViewModel : public wxDataViewModel
    {
	public:
		FileStorageViewModel(std::shared_ptr<const libtorrent::torrent_info> ti);

	private:
		struct Node
		{
			std::string name;
			int64_t size;
			int index;

			std::shared_ptr<Node> parent;
			std::map<std::string, std::shared_ptr<Node>> children;
		};

		void BuildNodeTree(std::shared_ptr<Node> node, std::vector<std::string> path, libtorrent::file_storage const& files, int index);

		unsigned int GetColumnCount() const wxOVERRIDE;
		wxString GetColumnType(unsigned int col) const wxOVERRIDE;
		void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const wxOVERRIDE;
		bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) wxOVERRIDE;
		wxDataViewItem GetParent(const wxDataViewItem &item) const wxOVERRIDE;
		bool IsContainer(const wxDataViewItem &item) const wxOVERRIDE;
		unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const wxOVERRIDE;

		std::shared_ptr<Node> m_root;
    };
}
