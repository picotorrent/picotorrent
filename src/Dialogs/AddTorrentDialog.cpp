#include "AddTorrentDialog.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include <regex>
#include <strsafe.h>

#include "../Commands/PrioritizeFilesCommand.hpp"
#include "../Configuration.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../Dialogs/TextInputDialog.hpp"
#include "../Models/TorrentFile.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../UI/TorrentFileListView.hpp"

#define LV_COL_NAME 0
#define LV_COL_SIZE 1
#define LV_COL_PRIO 2

#define FILTER_AUDIO_FILES 100
#define FILTER_IMAGE_FILES 101
#define FILTER_TEXT_FILES 102
#define FILTER_VIDEO_FILES 103
#define FILTER_REGULAR_EXPRESSION 104
#define FILTER_ALL 105
#define FILTER_CUSTOM 1000

const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

static const std::vector<std::wstring> AudioFiles = { L".3gp", L".aa", L".aac", L".aax", L".act", L".aiff", L".amr", L".ape", L".au", L".awb", L".dct", L".dss", L".dvf", L".flac", L".gsm", L".iklax", L".ivs", L".m4a", L".m4b", L".m4p", L".mmf", L".mp3", L".mpc", L".msv", L".ogg", L".oga", L".mogg", L".opus", L".ra", L".rm", L".raw", L".sln", L".tta", L".vox", L".wav", L".wma", L".wv", L".webm" };
static const std::vector<std::wstring> ImageFiles = { L".gif", L".jpg", L".jpeg", L".png", L".tiff" };
static const std::vector<std::wstring> TextFiles = { L".asc", L".txt" };
static const std::vector<std::wstring> VideoFiles = { L".webm", L".mkv", L".flv", L".vob", L".ogv", L".ogg", L".drc", L".gif", L".gifv", L".mng", L".avi", L".mov", L".qt", L".wmv", L".yuv", L".rm", L".rmvb", L".asf", L".amv", L".mp4", L".m4p", L".m4v", L".mpg", L".mp2", L".mpeg", L".mpe", L".mpv", L".m2v", L".svi", L".3gp", L".3g2", L".mxf", L".roq", L".nsv", L".f4v", L".f4p", L".f4a", L".f4b" };

static const std::map<int, std::vector<std::wstring>> FilterMap =
{
    { FILTER_AUDIO_FILES, AudioFiles },
    { FILTER_IMAGE_FILES, ImageFiles },
    { FILTER_TEXT_FILES, TextFiles },
    { FILTER_VIDEO_FILES, VideoFiles }
};

namespace lt = libtorrent;
using Dialogs::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(
    const std::vector<std::shared_ptr<lt::add_torrent_params>>& params)
    : m_params(params)
{
}

std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& AddTorrentDialog::GetParams()
{
    return m_params;
}

void AddTorrentDialog::OnShowFileFilter(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    bool include = nID == ID_INCLUDE_FILE_FILTER;

    Configuration& cfg = Configuration::GetInstance();
    std::vector<std::pair<std::string, std::string>> customFilters = cfg.GetFileFilters();

    CButton btn = GetDlgItem(nID);
    RECT rc;
    btn.GetWindowRect(&rc);

    HMENU fileTypes = CreateMenu();
    AppendMenu(fileTypes, MF_STRING, FILTER_AUDIO_FILES, TRW("audio_files"));
    AppendMenu(fileTypes, MF_STRING, FILTER_IMAGE_FILES, TRW("image_files"));
    AppendMenu(fileTypes, MF_STRING, FILTER_TEXT_FILES, TRW("text_files"));
    AppendMenu(fileTypes, MF_STRING, FILTER_VIDEO_FILES, TRW("video_files"));

    HMENU custom = CreateMenu();
    AppendMenu(custom, MF_STRING, FILTER_REGULAR_EXPRESSION, TRW("regular_expression"));

    if (!customFilters.empty())
    {
        AppendMenu(custom, MF_SEPARATOR, -1, NULL);
        int id = FILTER_CUSTOM;

        for (auto& p : customFilters)
        {
            AppendMenu(custom, MF_STRING, id, TWS(p.first));
            id += 1;
        }
    }

    HMENU filter = CreatePopupMenu();
    AppendMenu(filter, MF_STRING, FILTER_ALL, TRW("all"));
    AppendMenu(filter, MF_SEPARATOR, -1, NULL);
    AppendMenu(filter, MF_POPUP, reinterpret_cast<UINT_PTR>(fileTypes), TRW("file_types"));
    AppendMenu(filter, MF_POPUP, reinterpret_cast<UINT_PTR>(custom), TRW("custom"));

    int res = TrackPopupMenu(
        filter,
        TPM_NONOTIFY | TPM_RETURNCMD,
        rc.left,
        rc.bottom,
        0,
        m_hWnd,
        NULL);

    std::function<bool(Models::TorrentFile const&)> func;

    switch (res)
    {
    case FILTER_AUDIO_FILES:
    case FILTER_IMAGE_FILES:
    case FILTER_TEXT_FILES:
    case FILTER_VIDEO_FILES:
    {
        func = [res](Models::TorrentFile const& file)
        {
            if (FilterMap.find(res) == FilterMap.end())
            {
                return false;
            }

            std::wstring ext = file.name.substr(file.name.find_last_of('.'));
            auto find = std::find(FilterMap.at(res).begin(), FilterMap.at(res).end(), ext);
            return find != FilterMap.at(res).end();
        };
        break;
    }
    case FILTER_ALL:
    {
        func = [](Models::TorrentFile const& file) { return true; };
        break;
    }
    case FILTER_REGULAR_EXPRESSION:
    {
        Dialogs::TextInputDialog dlg;
        dlg.SetOkText(TRW("apply"));
        dlg.SetTitle(TRW("regex"));

        if (dlg.DoModal() == IDOK)
        {
            std::wstring regex = dlg.GetInput();
            func = [regex](Models::TorrentFile const& file)
            {
                std::wregex re(regex, std::regex_constants::icase);
                return std::regex_match(file.name, re);
            };
        }

        break;
    }
    default:
    {
        if (res >= FILTER_CUSTOM)
        {
            func = [res, customFilters](Models::TorrentFile const& file)
            {
                std::regex re(customFilters.at(res - FILTER_CUSTOM).second);
                return std::regex_match(TS(file.name), re);
            };
        }
        break;
    }
    }

    if (!func)
    {
        return;
    }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());
    const lt::file_storage& files = prm->ti->files();
    std::vector<int> indices;

    for (int i = 0; i < files.num_files(); i++)
    {
        Models::TorrentFile tf{ i };
        tf.name = TWS(files.file_path(i));
        tf.size = files.file_size(i);

        if (func(tf))
        {
            indices.push_back(i);
        }
    }

    if (indices.empty())
    {
        return;
    }

    Commands::PrioritizeFilesCommand cmd;
    cmd.indices = indices;
    cmd.priority = include ? PRIORITY_NORMAL : PRIORITY_DO_NOT_DOWNLOAD;

    OnPrioritizeFiles(PT_PRIORITIZEFILES, NULL, reinterpret_cast<LPARAM>(&cmd));
}

void AddTorrentDialog::OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    EndDialog(nID);
}

BOOL AddTorrentDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    DlgResize_Init();

    // Hook up controls
    m_torrents = GetDlgItem(ID_TORRENT);
    m_size = GetDlgItem(ID_SIZE);
    m_savePath = GetDlgItem(ID_SAVE_PATH);
    m_storageFull = GetDlgItem(ID_ADD_STORAGE_MODE_FULL);
    m_storageSparse = GetDlgItem(ID_ADD_STORAGE_MODE_SPARSE);
    m_includeFilter = GetDlgItem(ID_INCLUDE_FILE_FILTER);
    m_excludeFilter = GetDlgItem(ID_EXCLUDE_FILE_FILTER);

    BUTTON_SPLITINFO bs1;
    m_includeFilter.GetSplitInfo(&bs1);
    bs1.uSplitStyle = BCSS_NOSPLIT;
    m_includeFilter.SetSplitInfo(&bs1);

    BUTTON_SPLITINFO bs2;
    m_excludeFilter.GetSplitInfo(&bs2);
    bs2.uSplitStyle = BCSS_NOSPLIT;
    m_excludeFilter.SetSplitInfo(&bs2);

    // Localize
    SetWindowText(TRW("add_torrent_s"));
    SetDlgItemText(ID_TORRENT_TEXT, TRW("torrent"));
    SetDlgItemText(ID_SIZE_TEXT, TRW("size"));
    SetDlgItemText(ID_SAVE_PATH_TEXT, TRW("save_path"));
    SetDlgItemText(ID_BROWSE, TRW("browse"));
    SetDlgItemText(ID_STORAGE_GROUP, TRW("storage"));
    SetDlgItemText(IDOK, TRW("add_torrent_s"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_TEXT, TRW("storage_mode"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_SPARSE, TRW("sparse"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_FULL, TRW("full"));
    SetDlgItemText(ID_INCLUDE_FILE_FILTER, TRW("include"));
    SetDlgItemText(ID_EXCLUDE_FILE_FILTER, TRW("exclude"));

    m_fileList = std::make_shared<UI::TorrentFileListView>(GetDlgItem(ID_FILES));

    for (auto p : m_params)
    {
        m_torrents.AddString(ToWideString(p->ti->name()).c_str());
    }

    m_torrents.SetCurSel(0);
    ShowTorrent(0);

    return 0;
}

void AddTorrentDialog::OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());

    switch (nID)
    {
    case ID_ADD_STORAGE_MODE_FULL:
        m_storageFull.SetCheck(BST_CHECKED);
        m_storageSparse.SetCheck(BST_UNCHECKED);
        prm->storage_mode = lt::storage_mode_t::storage_mode_allocate;
        break;
    case ID_ADD_STORAGE_MODE_SPARSE:
        m_storageSparse.SetCheck(BST_CHECKED);
        m_storageFull.SetCheck(BST_UNCHECKED);
        prm->storage_mode = lt::storage_mode_t::storage_mode_sparse;
        break;
    }
}

void AddTorrentDialog::OnChangeSavePath(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Dialogs::OpenFileDialog openDialog;
    openDialog.SetGuid(DLG_SAVE);
    openDialog.SetOptions(openDialog.GetOptions() | FOS_PICKFOLDERS);
    openDialog.Show();

    auto res = openDialog.GetPaths();
    if (res.empty()) { return; }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());
    prm->save_path = ToString(res[0]);
    m_savePath.SetWindowText(res[0].c_str());
}

LRESULT AddTorrentDialog::OnPrioritizeFiles(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::PrioritizeFilesCommand*>(lParam);
    int current = m_torrents.GetCurSel();
    auto prm = m_params.at(current);
    auto files = prm->ti->files();

    for (int idx : cmd->indices)
    {
        if (prm->file_priorities.size() <= (size_t)idx)
        {
            prm->file_priorities.resize(idx + 1, PRIORITY_NORMAL);
        }

        prm->file_priorities.at(idx) = cmd->priority;

        // Update model
        Models::TorrentFile tf{ idx };
        tf.name = TWS(files.file_path(idx));
        tf.priority = cmd->priority;
        tf.size = files.file_size(idx);

        m_fileList->Update(tf);
    }

    auto idxLo = std::min_element(cmd->indices.begin(), cmd->indices.end());
    auto idxHi = std::max_element(cmd->indices.begin(), cmd->indices.end());

    m_fileList->RedrawItems(*idxLo, *idxHi);

    return FALSE;
}

void AddTorrentDialog::OnSavePathChanged(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());
    prm->save_path = m_savePath.GetValueA();
}

void AddTorrentDialog::OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    ShowTorrent(m_torrents.GetCurSel());
}

void AddTorrentDialog::ShowTorrent(size_t torrentIndex)
{
    if (torrentIndex >= m_params.size())
    {
        // LOG
        return;
    }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(torrentIndex);
    std::shared_ptr<lt::torrent_info> ti = prm->ti;

    m_fileList->RemoveAll();
    m_savePath.SetWindowText(ToWideString(prm->save_path).c_str());
    m_storageFull.SetCheck(BST_UNCHECKED);
    m_storageSparse.SetCheck(BST_UNCHECKED);

    switch (prm->storage_mode)
    {
    case lt::storage_mode_t::storage_mode_allocate:
        m_storageFull.SetCheck(BST_CHECKED);
        break;
    case lt::storage_mode_t::storage_mode_sparse:
        m_storageSparse.SetCheck(BST_CHECKED);
        break;
    }

    if (ti)
    {
        TCHAR s[1024];
        StrFormatByteSize64(ti->total_size(), s, ARRAYSIZE(s));
        m_size.SetWindowText(s);

        // Add model to list
        const lt::file_storage& files = ti->files();

        for (int i = 0; i < files.num_files(); i++)
        {
            Models::TorrentFile tf{ i };
            tf.name = TWS(files.file_path(i));
            tf.priority = prm->file_priorities.size() > (size_t)i
                ? prm->file_priorities.at(i)
                : PRIORITY_NORMAL;
            tf.size = files.file_size(i);

            m_fileList->Add(tf);
        }

        m_fileList->SetItemCount(ti->num_files());
    }
    else
    {
        m_fileList->SetItemCount(0);
    }
}
