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
	class Translator;

    class FileStorageViewModel : public wxDataViewModel
    {
	public:
		enum Columns
		{
			Name,
			Size,
			Progress,
			Priority
		};

		FileStorageViewModel(std::shared_ptr<Translator> translator);

		std::vector<int> GetFileIndices(wxDataViewItem&);
		wxDataViewItem GetRootItem();
		void RebuildTree(std::shared_ptr<const libtorrent::torrent_info> ti);
		void UpdatePriorities(std::vector<uint8_t> const& priorities); // TODO: merge these two
		void UpdatePriorities(std::vector<int> const& priorities);
		void UpdateProgress(std::vector<int64_t> const& progress);

	private:
		struct Node
		{
			std::string name;
			int64_t size;
			int index;
			uint8_t priority;
			float progress;

			std::shared_ptr<Node> parent;
			std::map<std::string, std::shared_ptr<Node>> children;
		};

		void FillIndices(Node* node, std::vector<int>& indices);
		wxIcon GetIconForFile(std::string const& fileName) const;
		unsigned int GetColumnCount() const wxOVERRIDE;
		wxString GetColumnType(unsigned int col) const wxOVERRIDE;
		void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const wxOVERRIDE;
		bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) wxOVERRIDE;
		wxDataViewItem GetParent(const wxDataViewItem &item) const wxOVERRIDE;
		bool IsContainer(const wxDataViewItem &item) const wxOVERRIDE;
		unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const wxOVERRIDE;

		wxIcon m_folderIcon;
		std::shared_ptr<Translator> m_trans;
		std::shared_ptr<Node> m_root;
		std::map<int, std::shared_ptr<Node>> m_map;
		std::map<std::string, wxIcon> m_icons;
    };
}
