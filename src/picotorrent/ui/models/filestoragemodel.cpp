#include "filestoragemodel.hpp"

#include "../../core/utils.hpp"
#include "../translator.hpp"

#include <filesystem>
#include <shellapi.h>

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/tokenzr.h>

namespace fs = std::filesystem;
namespace lt = libtorrent;
using pt::UI::Models::FileStorageModel;

static wxIcon FolderIcon;

FileStorageModel::FileStorageModel(std::function<void(wxDataViewItemArray&, lt::download_priority_t)> const& priorityChanged)
    : m_priorityChangedCallback(priorityChanged)
{
    if (!FolderIcon.IsOk())
    {
        SHFILEINFO shfi = { 0 };
        SHGetFileInfo(
            TEXT("empty"),
            FILE_ATTRIBUTE_DIRECTORY,
            &shfi,
            sizeof(SHFILEINFO),
            SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);

        FolderIcon.CreateFromHICON(shfi.hIcon);
    }
}

void FileStorageModel::ClearNodes()
{
    m_root = nullptr;
    m_map.clear();
    m_icons.clear();
}

std::vector<int> FileStorageModel::GetFileIndices(wxDataViewItemArray& items)
{
    std::vector<int> result;

    for (auto const& item : items)
    {
        FillIndices(
            reinterpret_cast<Node*>(item.GetID()),
            result);
    }

    return result;
}

void FileStorageModel::FillIndices(Node* node, std::vector<int>& indices)
{
    if (node->children.empty())
    {
        if (std::find(indices.begin(), indices.end(), node->index) == indices.end())
        {
            indices.push_back(node->index);
        }
    }
    else
    {
        for (auto p : node->children)
        {
            FillIndices(p.second.get(), indices);
        }
    }
}

wxDataViewItem FileStorageModel::GetRootItem()
{
    return m_root
        ? wxDataViewItem(reinterpret_cast<void*>(m_root.get()))
        : wxDataViewItem();
}

void FileStorageModel::RebuildTree(std::shared_ptr<const lt::torrent_info> ti)
{
    m_map.clear();

    if (ti->num_files() == 0)
    {
        m_root = nullptr;
        return;
    }

    m_root = std::make_shared<Node>();
    m_root->name = ti->name();
    m_root->priority = lt::default_priority;

    lt::file_storage const& files = ti->files();

    if (files.num_files() > 1)
    {
        for (int i = 0; i < files.num_files(); i++)
        {
            lt::file_index_t idx{ i };
            std::string path = files.file_path(idx);
            int64_t size = files.file_size(idx);

            wxStringTokenizer tokenizer(path, "\\");

            std::shared_ptr<Node> current = m_root;

            while (tokenizer.HasMoreTokens())
            {
                std::string part = tokenizer.GetNextToken();
                if (part == m_root->name) { continue; }

                auto iter = current->children.find(part);

                if (iter == current->children.end()
                    || tokenizer.CountTokens() == 0)
                {
                    std::shared_ptr<Node> node = std::make_shared<Node>();
                    node->name = part;
                    node->parent = current;
                    node->priority = lt::default_priority;

                    current->children.insert({ node->name, node });

                    std::size_t pos = node->name.find_last_of(".");

                    if (pos != std::string::npos)
                    {
                        std::string extension = node->name.substr(pos);

                        if (m_icons.find(extension) == m_icons.end())
                        {
                            SHFILEINFO shfi = { 0 };
                            SHGetFileInfo(
                                Utils::toStdWString(extension).c_str(),
                                FILE_ATTRIBUTE_NORMAL,
                                &shfi,
                                sizeof(SHFILEINFO),
                                SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);

                            wxIcon icon;
                            icon.CreateFromHICON(shfi.hIcon);

                            m_icons.insert({ extension, icon });
                        }
                    }

                    if (tokenizer.CountTokens() == 0)
                    {
                        node->index = i;
                        node->size = files.file_size(idx);

                        m_map.insert({ i, node });
                    }
                    else
                    {
                        OutputDebugStringA(path.c_str());
                        OutputDebugStringA("\n");
                    }
                }

                current = current->children.at(part);
            }
        }
    }
    else if (files.num_files() > 0)
    {
        m_root->index = 0;
        m_root->size = files.file_size(lt::file_index_t(0));

        std::size_t pos = m_root->name.find_last_of(".");

        if (pos != std::string::npos)
        {
            std::string extension = m_root->name.substr(pos);

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

        m_map.insert({ 0, m_root });
    }

    this->Cleared();
}

void FileStorageModel::UpdatePriorities(const std::vector<libtorrent::download_priority_t>& priorities)
{
    for (auto& item : m_map)
    {
        lt::download_priority_t prio = lt::default_priority;
        std::shared_ptr<Node> const& node = item.second;

        if (priorities.size() >= item.first + 1)
        {
            prio = priorities.at(item.first);
        }

        if (node->priority == prio)
        {
            continue;
        }

        node->priority = prio;

        this->ValueChanged(
            wxDataViewItem(reinterpret_cast<void*>(node.get())),
            Columns::Priority);
    }
}

void FileStorageModel::UpdateProgress(std::vector<int64_t> const& progress)
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

wxIcon FileStorageModel::GetIconForFile(std::string const& fileName) const
{
    std::size_t pos = fileName.find_last_of(".");
    if (pos == std::string::npos) { return wxNullIcon; }

    std::string extension = fileName.substr(pos);

    if (m_icons.find(extension) != m_icons.end())
    {
        return m_icons.at(extension);
    }

    return wxNullIcon;
}

unsigned int FileStorageModel::GetColumnCount() const
{
    return Columns::_Max;
}

wxString FileStorageModel::GetColumnType(unsigned int col) const
{
    return "string";
}

void FileStorageModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const
{
    wxASSERT(item.IsOk());

    Node* node = reinterpret_cast<Node*>(item.GetID());

    switch (col)
    {
    case Columns::Name:
    {
        if (m_priorityChangedCallback)
        {
            variant << wxDataViewCheckIconText(
                node->name,
                node->children.empty()
                ? GetIconForFile(node->name)
                : FolderIcon,
                node->priority == lt::dont_download ? wxCHK_UNCHECKED : wxCHK_CHECKED);
        }
        else
        {
            variant << wxDataViewIconText(
                node->name,
                node->children.empty()
                ? GetIconForFile(node->name)
                : FolderIcon);
        }

        break;
    }
    case Columns::Size:
        variant = Utils::toHumanFileSize(node->size);
        break;
    case Columns::Progress:
        variant = static_cast<long>(node->progress * 100);
        break;
    case Columns::Priority:
        if (node->priority == libtorrent::dont_download)
        {
            variant = i18n("do_not_download");
        }
        else if (node->priority == libtorrent::low_priority)
        {
            variant = i18n("low");
        }
        else if (node->priority == libtorrent::default_priority)
        {
            variant = i18n("normal");
        }
        else if (node->priority == libtorrent::top_priority)
        {
            variant = i18n("maximum");
        }
        else
        {
            variant = i18n("unknown");
        }

        break;
    }
}

bool FileStorageModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col)
{
    wxASSERT(item.IsOk());

    Node* node = reinterpret_cast<Node*>(item.GetID());

    switch (col)
    {
    case Columns::Name:
    {
        std::function<void(wxDataViewItemArray&, Node*, lt::download_priority_t)> recursiveSkip = [&](auto& arr, Node* node, auto prio)
        {
            if (node->priority != prio)
            {
                node->priority = prio;
                arr.push_back(wxDataViewItem(reinterpret_cast<void*>(node)));
            }

            for (auto const [key,n] : node->children) { recursiveSkip(arr, n.get(), prio); }
        };

        wxDataViewCheckIconText checkIconText;
        checkIconText << variant;

        wxDataViewItemArray changed;
        lt::download_priority_t prio = checkIconText.GetCheckedState() == wxCHK_CHECKED
            ? lt::default_priority
            : lt::dont_download;

        recursiveSkip(changed, node, prio);

        this->ItemsChanged(changed);

        m_priorityChangedCallback(changed, prio);

        return true;
    }
    }

    return false;
}

wxDataViewItem FileStorageModel::GetParent(const wxDataViewItem &item) const
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

bool FileStorageModel::IsContainer(const wxDataViewItem &item) const
{
    if (!item.IsOk())
    {
        return true;
    }

    Node* node = reinterpret_cast<Node*>(item.GetID());
    return !(node->children.empty());
}

unsigned int FileStorageModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
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
