#include "filestorageviewmodel.hpp"

#include "string.hpp"
#include "translator.hpp"
#include "utils.hpp"

#include <shellapi.h>

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>

namespace lt = libtorrent;
using pt::FileStorageViewModel;

FileStorageViewModel::FileStorageViewModel(std::shared_ptr<pt::Translator> translator)
    : m_trans(translator)
{
    SHFILEINFO shfi = { 0 };
    SHGetFileInfo(
        TEXT("empty"),
        FILE_ATTRIBUTE_DIRECTORY,
        &shfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);

    m_folderIcon.CreateFromHICON(shfi.hIcon);
}

void FileStorageViewModel::ClearNodes()
{
    m_root = nullptr;
    m_map.clear();
    m_icons.clear();
}

std::vector<int> FileStorageViewModel::GetFileIndices(wxDataViewItem& item)
{
    Node* node = reinterpret_cast<Node*>(item.GetID());

    if (node->children.empty())
    {
        return { node->index };
    }

    std::vector<int> result;

    FillIndices(node, result);

    return result;
}

void FileStorageViewModel::FillIndices(Node* node, std::vector<int>& indices)
{
    if (node->children.empty())
    {
        indices.push_back(node->index);
    }
    else
    {
        for (auto p : node->children)
        {
            FillIndices(p.second.get(), indices);
        }
    }
}

wxDataViewItem FileStorageViewModel::GetRootItem()
{
    if (!m_root)
    {
        return wxDataViewItem();
    }

    return wxDataViewItem(reinterpret_cast<void*>(m_root.get()));
}

void FileStorageViewModel::RebuildTree(std::shared_ptr<const lt::torrent_info> ti)
{
    m_map.clear();

    if (ti->num_files() == 0)
    {
        m_root = nullptr;
        return;
    }

    m_root = std::make_shared<Node>();
    m_root->name = ti->name();

    lt::file_storage const& files = ti->files();

    if (files.num_files() > 1)
    {
        for (int i = 0; i < files.num_files(); i++)
        {
            lt::file_index_t idx(i);

            std::vector<std::string> path = pt::String::Split(files.file_path(idx), "\\");
            path.erase(path.begin());

            std::shared_ptr<Node> currentNode = m_root;

            for (size_t j = 0; j < path.size(); j++)
            {
                if (currentNode->children.find(path[j]) == currentNode->children.end()
                    || j == path.size() - 1)
                {
                    std::shared_ptr<Node> newNode = std::make_shared<Node>();
                    newNode->name = path[j];
                    newNode->parent = currentNode;
                    currentNode->children.insert({ newNode->name, newNode });

                    std::size_t pos = newNode->name.find_last_of(".");

                    if (pos != std::string::npos)
                    {
                        std::string extension = newNode->name.substr(pos);

                        if (m_icons.find(extension) == m_icons.end())
                        {
                            SHFILEINFO shfi = { 0 };
                            SHGetFileInfo(
                                wxString(extension).ToStdWstring().c_str(),
                                FILE_ATTRIBUTE_NORMAL,
                                &shfi,
                                sizeof(SHFILEINFO),
                                SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);

                            wxIcon icon;
                            icon.CreateFromHICON(shfi.hIcon);

                            m_icons.insert({ extension, icon });
                        }
                    }

                    if (j == path.size() - 1)
                    {
                        newNode->index = i;
                        newNode->size = files.file_size(idx);

                        m_map.insert({ i, newNode });
                    }
                }

                currentNode = currentNode->children.at(path[j]);
            }
        }
    }
    else if (files.num_files() > 0)
    {
        m_root->index = 0;
        m_root->size = files.file_size(lt::file_index_t(0));

        std::string extension = m_root->name.substr(m_root->name.find_last_of("."));

        SHFILEINFO shfi = { 0 };
        SHGetFileInfo(
            wxString(extension).ToStdWstring().c_str(),
            FILE_ATTRIBUTE_NORMAL,
            &shfi,
            sizeof(SHFILEINFO),
            SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);

        wxIcon icon;
        icon.CreateFromHICON(shfi.hIcon);

        m_icons.insert({ extension, icon });
        m_map.insert({ 0, m_root });
    }

    this->Cleared();
}

void FileStorageViewModel::UpdatePriorities(const std::vector<libtorrent::download_priority_t>& priorities)
{
    for (size_t i = 0; i < priorities.size(); i++)
    {
        std::shared_ptr<Node> const& node = m_map.at(static_cast<int>(i));

        if (node->priority == priorities.at(i))
        {
            continue;
        }

        node->priority = priorities.at(i);

        this->ValueChanged(
            wxDataViewItem(reinterpret_cast<void*>(node.get())),
            Columns::Priority);
    }
}

void FileStorageViewModel::UpdateProgress(std::vector<int64_t> const& progress)
{
    for (size_t i = 0; i < progress.size(); i++)
    {
        std::shared_ptr<Node> const& node = m_map.at(static_cast<int>(i));
        float calculatedProgress = .0f;

        if (progress.at(i) > 0)
        {
            calculatedProgress = static_cast<float>(progress.at(i)) / node->size;
        }

        node->progress = calculatedProgress;

        this->ValueChanged(
            wxDataViewItem(reinterpret_cast<void*>(node.get())),
            Columns::Progress);
    }
}

wxIcon FileStorageViewModel::GetIconForFile(std::string const& fileName) const
{
    std::string extension = fileName.substr(fileName.find_last_of("."));

    if (m_icons.find(extension) != m_icons.end())
    {
        return m_icons.at(extension);
    }

    return wxNullIcon;
}

unsigned int FileStorageViewModel::GetColumnCount() const
{
    return Columns::_Max;
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
    case Columns::Name:
    {
        wxIcon icon = m_folderIcon;

        if (node->children.empty())
        {
            icon = GetIconForFile(node->name);
        }

        variant << wxDataViewIconText(
            wxString::FromUTF8(node->name),
            icon);

        break;
    }
    case Columns::Size:
        variant = Utils::ToHumanFileSize(node->size);
        break;
    case Columns::Progress:
        variant = static_cast<long>(node->progress * 100);
        break;
    case Columns::Priority:
        if (node->priority == libtorrent::dont_download)
        {
            variant = i18n(m_trans, "do_not_download");
        }
        else if (node->priority == libtorrent::low_priority)
        {
            variant = i18n(m_trans, "low");
        }
        else if (node->priority == libtorrent::default_priority)
        {
            variant = i18n(m_trans, "normal");
        }
        else if (node->priority == libtorrent::top_priority)
        {
            variant = i18n(m_trans, "maximum");
        }
        else
        {
            variant = i18n(m_trans, "unknown");
        }

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
        return wxDataViewItem(0);
    }

    Node* node = reinterpret_cast<Node*>(item.GetID());

    if (node == m_root.get())
    {
        return wxDataViewItem(0);
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
    if (!m_root)
    {
        return 0;
    }

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
