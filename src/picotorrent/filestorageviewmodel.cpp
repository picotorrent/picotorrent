#include "filestorageviewmodel.hpp"

#include "string.hpp"
#include "utils.hpp"

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>

namespace lt = libtorrent;
using pt::FileStorageViewModel;

FileStorageViewModel::FileStorageViewModel(std::shared_ptr<const libtorrent::torrent_info> ti)
	: m_root(std::make_shared<Node>())
{
	m_root->name = ti->name();

	lt::file_storage const& files = ti->files();

	for (int i = 0; i < files.num_files(); i++)
	{
		lt::file_index_t idx(i);

		std::vector<std::string> path = pt::String::Split(files.file_path(idx), "\\");
		path.erase(path.begin());

		BuildNodeTree(m_root, path, files, i);
	}
}

void FileStorageViewModel::BuildNodeTree(std::shared_ptr<Node> node, std::vector<std::string> path, lt::file_storage const& files, int index)
{
	std::shared_ptr<Node> currentNode = node;
	lt::file_index_t idx(index);

	for (size_t i = 0; i < path.size(); i++)
	{
		if (currentNode->children.find(path[i]) == currentNode->children.end()
			|| i == path.size() - 1)
		{
			std::shared_ptr<Node> newNode = std::make_shared<Node>();
			newNode->name = path[i];
			newNode->parent = currentNode;
			currentNode->children.insert({ newNode->name, newNode });

			if (i == path.size() - 1)
			{
				newNode->index = index;
				newNode->size = files.file_size(idx);
			}
		}

		currentNode = currentNode->children.at(path[i]);
	}
}

unsigned int FileStorageViewModel::GetColumnCount() const
{
	return -1;
}

wxString FileStorageViewModel::GetColumnType(unsigned int col) const
{
	return "string";
}

void FileStorageViewModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const
{
	wxASSERT(item.IsOk());

	Node* node = reinterpret_cast<Node*>(item.GetID());

	switch (col)
	{
	case 0:
		variant = node->name;
		break;
	case 1:
		variant = Utils::ToHumanFileSize(node->size);
		break;
	}
}

bool FileStorageViewModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col)
{
	return false;
}

wxDataViewItem FileStorageViewModel::GetParent(const wxDataViewItem &item) const
{
	if (!item.IsOk())
	{
		return wxDataViewItem();
	}

	Node* node = reinterpret_cast<Node*>(item.GetID());

	if (node == m_root.get())
	{
		return wxDataViewItem();
	}

	return wxDataViewItem(reinterpret_cast<void*>(node->parent.get()));
}

bool FileStorageViewModel::IsContainer(const wxDataViewItem &item) const
{
	if (!item.IsOk())
	{
		return true;
	}

	Node* node = reinterpret_cast<Node*>(item.GetID());
	return !(node->children.empty());
}

unsigned int FileStorageViewModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
	Node* node = reinterpret_cast<Node*>(parent.GetID());

	if (!node)
	{
		array.Add(wxDataViewItem(reinterpret_cast<void*>(m_root.get())));
		return 1;
	}

	for (auto p : node->children)
	{
		array.Add(wxDataViewItem(reinterpret_cast<void*>(p.second.get())));
	}

	return node->children.size();
}
