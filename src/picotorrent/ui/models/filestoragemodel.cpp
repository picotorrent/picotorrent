#include "filestoragemodel.hpp"

#include "../../core/utils.hpp"
#include "../translator.hpp"

#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <shellapi.h>

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/tokenzr.h>

namespace fs = std::filesystem;
namespace lt = libtorrent;
using pt::UI::Models::FileStorageModel;

static wxIcon FolderIcon;
static wxIcon UnknownIcon;

FileStorageModel::FileStorageModel(std::function<void(wxDataViewItemArray&, lt::download_priority_t)> const& priorityChanged)
    : m_priorityChangedCallback(priorityChanged),
    m_root(std::make_shared<Node>())
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

    if (!UnknownIcon.IsOk())
    {
        SHSTOCKICONINFO sii;
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_DOCNOASSOC, SHGSI_ICON | SHGSI_SMALLICON, &sii)))
        {
            UnknownIcon.CreateFromHICON(sii.hIcon);
        }
    }
}

void FileStorageModel::ClearNodes()
{
    m_root->children.clear();
    m_map.clear();
    m_icons.clear();
}

std::vector<lt::file_index_t> FileStorageModel::GetFileIndices(wxDataViewItemArray& items)
{
    std::vector<lt::file_index_t> result;

    for (auto const& item : items)
    {
        FillIndices(
            static_cast<Node*>(item.GetID()),
            result);
    }

    return result;
}

void FileStorageModel::FillIndices(Node* node, std::vector<lt::file_index_t>& indices)
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
    return wxDataViewItem(static_cast<void*>(m_root.get()));
}

void FileStorageModel::RebuildTree(std::shared_ptr<const lt::torrent_info> ti)
{
    m_map.clear();
    m_root->children.clear();

    if (ti->num_files() == 0)
    {
        return;
    }

    lt::file_storage const& files = ti->files();

    for (lt::file_index_t idx : files.file_range())
    {
        std::shared_ptr<Node> currentNode = m_root;

        std::vector<std::string> parts;
        boost::split(
            parts,
            files.file_path(idx),
            [](char c) { return c == '\\'; });

        if (parts.size() > 0)
        {
            parts.pop_back();
        }

        for (auto const& part : parts)
        {
            auto np = currentNode->children.find(part) != currentNode->children.end()
                ? currentNode->children.at(part)
                : nullptr;

            if (!np)
            {
                np = std::make_shared<Node>();
                np->name = part;
                np->parent = currentNode;
                np->priority = lt::default_priority;

                currentNode->children.insert({ np->name, np });
            }

            currentNode = np;
        }

        auto n = std::make_shared<Node>();
        n->index = idx;
        n->name = files.file_name(idx).to_string();
        n->parent = currentNode;
        n->priority = lt::default_priority;
        n->size = files.file_size(idx);

        currentNode->children.insert({ n->name, n });
        m_map.insert({ idx, n });

        std::size_t pos = n->name.find_last_of(".");

        if (pos != std::string::npos)
        {
            std::string extension = n->name.substr(pos);

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
    }

    this->Cleared();
}

void FileStorageModel::UpdatePriorities(const std::vector<libtorrent::download_priority_t>& priorities)
{
    for (auto& item : m_map)
    {
        lt::download_priority_t prio = lt::default_priority;
        std::shared_ptr<Node> const& node = item.second;

        if (priorities.size() >= static_cast<int>(item.first) + 1)
        {
            prio = priorities.at(static_cast<int>(item.first));
        }

        if (node->priority == prio)
        {
            continue;
        }

        node->priority = prio;

        this->ValueChanged(
            wxDataViewItem(static_cast<void*>(node.get())),
            Columns::Priority);
    }
}

void FileStorageModel::UpdateProgress(std::vector<int64_t> const& progress)
{
    for (size_t i = 0; i < progress.size(); i++)
    {
        std::shared_ptr<Node> const& node = m_map.at(lt::file_index_t{ static_cast<int>(i) });
        float calculatedProgress = .0f;

        if (progress.at(i) > 0)
        {
            calculatedProgress = static_cast<float>(progress.at(i)) / node->size;
        }

        node->progress = calculatedProgress;

        this->ValueChanged(
            wxDataViewItem(static_cast<void*>(node.get())),
            Columns::Progress);
    }
}

wxIcon FileStorageModel::GetIconForFile(std::string const& fileName) const
{
    std::size_t pos = fileName.find_last_of(".");
    if (pos == std::string::npos) { return UnknownIcon; }

    std::string extension = fileName.substr(pos);

    if (m_icons.find(extension) != m_icons.end())
    {
        return m_icons.at(extension);
    }

    return UnknownIcon;
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

    Node* node = static_cast<Node*>(item.GetID());

    switch (col)
    {
    case Columns::Name:
    {
        if (m_priorityChangedCallback)
        {
            variant << wxDataViewCheckIconText(
                Utils::toStdWString(node->name),
                node->children.empty()
                ? GetIconForFile(node->name)
                : FolderIcon,
                node->priority == lt::dont_download ? wxCHK_UNCHECKED : wxCHK_CHECKED);
        }
        else
        {
            variant << wxDataViewIconText(
                Utils::toStdWString(node->name),
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

    Node* node = static_cast<Node*>(item.GetID());

    switch (col)
    {
    case Columns::Name:
    {
        std::function<void(wxDataViewItemArray&, Node*, lt::download_priority_t)> recursiveSkip = [&](auto& arr, Node* node, auto prio)
        {
            if (node->priority != prio)
            {
                node->priority = prio;
                arr.push_back(wxDataViewItem(static_cast<void*>(node)));
            }

            for (auto const [key,n] : node->children) { recursiveSkip(arr, n.get(), prio); }
        };

        wxDataViewCheckIconText checkIconText;
        checkIconText << variant;

        lt::download_priority_t prio = checkIconText.GetCheckedState() == wxCHK_CHECKED
            ? lt::default_priority
            : lt::dont_download;

        wxDataViewItemArray changed;
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
    wxASSERT(item.IsOk());

    Node* node = static_cast<Node*>(item.GetID());

    return node->parent.get() == m_root.get()
        ? wxDataViewItem(0)
        : wxDataViewItem(static_cast<void*>(node->parent.get()));
}

bool FileStorageModel::IsContainer(const wxDataViewItem &item) const
{
    // Override this to indicate of item is a container, i.e. if it can have child items.
    if (!item.IsOk()) { return true; }
    Node* node = static_cast<Node*>(item.GetID());
    return !(node->children.empty());
}

unsigned int FileStorageModel::GetChildren(const wxDataViewItem &item, wxDataViewItemArray &array) const
{
    Node* node = item.IsOk()
        ? static_cast<Node*>(item.GetID()) != nullptr
        ? static_cast<Node*>(item.GetID())
        : m_root.get()
        : m_root.get();

    for (auto p : node->children)
    {
        array.Add(wxDataViewItem(static_cast<void*>(p.second.get())));
    }

    return node->children.size();
}
