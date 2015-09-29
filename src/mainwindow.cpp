#include "mainwindow.h"

#include "scaler.h"
#include "stdafx.h"
#include "torrentitem.h"
#include "torrentlistview.h"

#include <boost/make_shared.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <shobjidl.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <uxtheme.h>

#define WM_PT_ALERT WM_USER + 1

namespace lt = libtorrent;
using pico::MainWindow;

MainWindow::MainWindow()
    : hWnd_(NULL),
    timer_(NULL)
{
    // Set up session
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_int(lt::settings_pack::stop_tracker_timeout, 1);

    session_ = std::make_unique<lt::session>(settings);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Create()
{
    pico::Scaler scaler;

    WNDCLASSEX wnd = { 0 };
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbWndExtra = sizeof(MainWindow*);
    wnd.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
    wnd.lpfnWndProc = &MainWindow::WndProcProxy;
    wnd.lpszClassName = TEXT("PicoTorrent/MainWindow");
    wnd.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClassEx(&wnd);

    hWnd_ = CreateWindowEx(
        0,
        wnd.lpszClassName,
        TEXT("PicoTorrent"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        scaler.GetX(700),
        scaler.GetY(200),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        static_cast<LPVOID>(this));
}

void MainWindow::LoadState()
{
    std::vector<char> buf;

    if (!ReadFile(buf, TEXT("Session.dat")))
    {
        return;
    }

    lt::bdecode_node node;
    lt::error_code ec;
    lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ec);

    if (ec)
    {
        // TODO(log)
        return;
    }

    session_->load_state(node);
}

void MainWindow::LoadTorrents()
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileEx(
        TEXT("Torrents\\*.torrent"),
        FindExInfoBasic,
        &ffd,
        FindExSearchNameMatch,
        NULL,
        FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        TCHAR path[MAX_PATH];
        PathCombine(path, TEXT("Torrents"), ffd.cFileName);

        std::vector<char> buf;

        if (!ReadFile(buf, path))
        {
            continue;
        }

        lt::bdecode_node node;
        lt::error_code ec;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ec);

        if (ec)
        {
            // TODO(log)
            continue;
        }

        lt::add_torrent_params p;
        p.save_path = "C:\\Users\\Viktor\\Downloads";
        p.ti = boost::make_shared<lt::torrent_info>(node);

        PathRemoveExtension(path);
        PathAddExtension(path, TEXT(".dat"));

        if (ReadFile(buf, path))
        {
            p.resume_data = buf;
        }

        session_->async_add_torrent(p);
    } while (FindNextFile(hFind, &ffd) != 0);
    
    FindClose(hFind);
}

void MainWindow::OnAlertNotify(HWND target)
{
    // Post a message to our window procedure telling it
    // to read the alerts from the session.
    ::PostMessage(target, WM_PT_ALERT, NULL, NULL);
}

void MainWindow::AddTorrent(std::wstring const& file)
{
    // Open and read file
    HANDLE hFile = CreateFileW(
        file.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // TODO(log)
        return;
    }

    std::vector<char> buffer;
    DWORD read = 0;

    do
    {
        char tmp[1024];
        read = 0;

        if (!::ReadFile(hFile, &tmp, _ARRAYSIZE(tmp), &read, NULL))
        {
            // TODO(log)
            CloseHandle(hFile);
            return;
        }

        buffer.insert(buffer.end(), tmp, tmp + read);
    } while (read > 0);

    if (buffer.size() <= 0)
    {
        // TODO(log)
        CloseHandle(hFile);
        return;
    }

    // Close the file handle since we have the file contents in our buffer.
    CloseHandle(hFile);

    lt::bdecode_node node;
    lt::error_code ec;
    lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, ec);

    if (ec)
    {
        // TODO(log)
        return;
    }

    lt::add_torrent_params p;
    p.save_path = "C:\\Users\\Viktor\\Downloads";
    p.ti = boost::make_shared<lt::torrent_info>(node);

    session_->async_add_torrent(p);
}

void MainWindow::OnDestroy()
{
    if (timer_ != NULL)
    {
        KillTimer(hWnd_, 1);
    }

    // Clear the notify function since we now read alerts
    // manually.
    typedef boost::function<void()> notify_func_t;
    session_->set_alert_notify(notify_func_t());

    // Save session state to "Session.dat" in the
    // current directory.
    lt::entry e;
    session_->save_state(e);

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    HANDLE hFile = CreateFile(
        TEXT("Session.dat"),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // TODO(log)
        return;
    }

    if (!WriteFile(
        hFile,
        &buf[0],
        buf.size(),
        NULL,
        NULL))
    {
        // TODO(log)
        CloseHandle(hFile);
        return;
    }

    CloseHandle(hFile);
    hFile = NULL;

    // Pause the session (and indirectly, all torrents)
    session_->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp;
    session_->get_torrent_status(&temp, [](const lt::torrent_status &st) { return true; }, 0);

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid())
        {
            // TODO(log)
            continue;
        }

        if (!st.has_metadata)
        {
            // TODO(log)
            continue;
        }

        if (!st.need_save_resume)
        {
            // TODO(log)
            continue;
        }

        st.handle.save_resume_data();
        ++numOutstandingResumeData;
    }

    // TODO(log) outstanding resume data

    // Create the path to "Torrents/" if it does not exist
    DWORD torrentAttrs = GetFileAttributes(TEXT("Torrents"));
    
    if (torrentAttrs == INVALID_FILE_ATTRIBUTES
        || !(torrentAttrs & FILE_ATTRIBUTE_DIRECTORY))
    {
        CreateDirectory(TEXT("Torrents"), NULL);
    }

    while (numOutstandingResumeData > 0)
    {
        const lt::alert *a = session_->wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        session_->pop_alerts(&alerts);

        for (lt::alert *a : alerts)
        {
            lt::torrent_paused_alert *tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++numPaused;
                // TODO(log)
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                // TODO(log)
                continue;
            }

            lt::save_resume_data_alert *rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --numOutstandingResumeData;
            if (!rd->resume_data) { continue; }

            buf.clear();
            lt::bencode(std::back_inserter(buf), *rd->resume_data);

            std::wstring hash = lt::convert_to_wstring(lt::to_hex(rd->handle.info_hash().to_string()));
            std::wstring fileName = TEXT("Torrents\\") + hash + TEXT(".dat");

            hFile = CreateFile(
                fileName.c_str(),
                GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
             
            if (hFile == INVALID_HANDLE_VALUE)
            {
                // TODO(log)
                continue;
            }

            if (!WriteFile(
                hFile,
                &buf[0],
                buf.size(),
                NULL,
                NULL))
            {
                // TODO(log)
            }

            CloseHandle(hFile);
        }
    }
}

void MainWindow::OnFileAddTorrent()
{
    IFileOpenDialog *dialog = NULL;
    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog));

    COMDLG_FILTERSPEC fileTypes[] =
    {
        { L"Torrent files", L"*.torrent" },
        { L"All files", L"*.*" }
    };

    DWORD flags;
    dialog->GetOptions(&flags);
    dialog->SetOptions(flags | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST);

    dialog->SetFileTypes(_countof(fileTypes), fileTypes);
    dialog->SetTitle(L"Open torrent files");
    dialog->SetOkButtonLabel(L"Open");
    dialog->Show(hWnd_);

    IShellItemArray *items = NULL;
    dialog->GetResults(&items);

    if (items == NULL)
    {
        dialog->Release();
        return;
    }

    DWORD count;
    items->GetCount(&count);

    for (DWORD i = 0; i < count; i++)
    {
        IShellItem* item = NULL;
        items->GetItemAt(i, &item);

        PWSTR path = NULL;
        item->GetDisplayName(SIGDN_FILESYSPATH, &path);

        AddTorrent(path);

        CoTaskMemFree(path);
        item->Release();
    }

    items->Release();
    dialog->Release();
}

bool MainWindow::ReadFile(std::vector<char> &buf, LPCTSTR path)
{
    buf.clear();

    HANDLE hFile = CreateFile(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // TODO(log)
        return false;
    }

    DWORD read = 0;

    do
    {
        TCHAR tmp[1024];
        read = 0;

        if (!::ReadFile(
            hFile,
            tmp,
            _ARRAYSIZE(tmp),
            &read,
            NULL))
        {
            // TODO(log)
            CloseHandle(hFile);
            return false;
        }

        buf.insert(buf.end(), tmp, tmp + read);
    } while (read > 0);

    return true;
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_FILE_ADDTORRENT:
        {
            OnFileAddTorrent();
            break;
        }
        case ID_FILE_EXIT:
        {
            PostMessage(hWnd_, WM_CLOSE, 0, 0);
            break;
        }
        }
        break;
    }

    case WM_CREATE:
    {
        pico::Scaler scaler;

        torrentsView_ = std::make_unique<pico::TorrentListView>(hWnd);
        torrentsView_->Create();

        // Set up torrent list view (columns etc)
        torrentsView_->AddColumn(TEXT("Name"), scaler.GetX(250), LVCFMT_LEFT);
        torrentsView_->AddColumn(TEXT("#"), scaler.GetX(30), LVCFMT_RIGHT);
        torrentsView_->AddColumn(TEXT("Size"), scaler.GetX(80), LVCFMT_RIGHT);
        torrentsView_->AddColumn(TEXT("Status"), scaler.GetX(100), LVCFMT_LEFT);
        torrentsView_->AddColumn(TEXT("DL"), scaler.GetX(80), LVCFMT_RIGHT);
        torrentsView_->AddColumn(TEXT("UL"), scaler.GetX(80), LVCFMT_RIGHT);

        hWnd_progress_ = CreateWindowEx(
            0,
            PROGRESS_CLASS,
            NULL,
            WS_CHILD | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL);

        hTheme_progress_ = OpenThemeData(hWnd_progress_, L"PROGRESS");

        session_->set_alert_notify(std::bind(&MainWindow::OnAlertNotify, this, hWnd));

        LoadState();
        LoadTorrents();

        // Start the timer which will post session thingies
        timer_ = SetTimer(hWnd, 1, 1000, NULL);
        break;
    }

    case WM_DESTROY:
        CloseThemeData(hTheme_progress_);

        OnDestroy();
        PostQuitMessage(0);
        break;

    case WM_NOTIFY:
    {
        NMHDR *nmhdr = reinterpret_cast<NMHDR*>(lParam);

        switch (nmhdr->code)
        {
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            
            if (inf->item.iItem == -1)
            {
                // TODO(log)
                break;
            }

            if (items_.find(inf->item.iItem) == items_.end())
            {
                // TODO(log)
                break;
            }

            lt::sha1_hash &hash = items_.find(inf->item.iItem)->second;
            TorrentItem &item = torrents_.find(hash)->second;

            if (inf->item.mask & LVIF_TEXT)
            {
                switch (inf->item.iSubItem)
                {
                case 0: // name
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.GetName().c_str());
                    break;

                case 1: // queue position
                {
                    std::wstring pos = TEXT("");

                    if (item.GetQueuePosition() >= 0)
                    {
                        pos = std::to_wstring(item.GetQueuePosition() + 1);
                    }

                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, pos.c_str());
                    break;
                }

                case 2: // size
                {
                    TCHAR buf[255];
                    StrFormatByteSize64(
                        item.GetSize(),
                        buf,
                        _ARRAYSIZE(buf));

                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, buf);
                    break;
                }
                case 4: // dl
                case 5: // ul
                {
                    int rate = inf->item.iSubItem == 4
                        ? item.GetDownloadRate()
                        : item.GetUploadRate();

                    LPCTSTR text = TEXT("-");

                    if (rate > 1024)
                    {
                        TCHAR buf[255];
                        StrFormatByteSize64(
                            rate,
                            buf,
                            _ARRAYSIZE(buf));
                        _snwprintf_s(buf, _ARRAYSIZE(buf), TEXT("%s/s"), buf);
                        text = buf;
                    }

                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, text);
                    break;
                }
                }
            }

            break;
        }
        case NM_CUSTOMDRAW:
        {
            if (nmhdr->hwndFrom != torrentsView_->GetWindowHandle())
            {
                break;
            }

            LPNMLVCUSTOMDRAW lpCD = (LPNMLVCUSTOMDRAW)nmhdr;
            
            if (lpCD->nmcd.dwDrawStage == CDDS_PREPAINT)
            {
                return CDRF_NOTIFYITEMDRAW;
            }

            if (lpCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
            {
                return CDRF_NOTIFYSUBITEMDRAW;
            }

            if (lpCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)
                && lpCD->iSubItem == 3) // status
            {
                RECT rc = { 0 };
                ListView_GetSubItemRect(lpCD->nmcd.hdr.hwndFrom, lpCD->nmcd.dwItemSpec, lpCD->iSubItem, LVIR_BOUNDS, &rc);

                rc.bottom -= 2;
                rc.left += 2;
                rc.right -= 2;
                rc.top += 2;
                
                // Paint the background
                DrawThemeBackground(hTheme_progress_, lpCD->nmcd.hdc, 11, 1, &rc, NULL);
                // Paint the filler
                rc.bottom -= 1;
                rc.right -= 1;
                rc.left += 1;
                rc.top += 1;

                // Calculate the width according to the progress of the current torrent
                lt::sha1_hash const& hash = items_.at(lpCD->nmcd.dwItemSpec);
                TorrentItem &item = torrents_.at(hash);
                float progress = item.GetProgress();
                int width = rc.right - rc.left;
                int newWidth = width * progress;
                rc.right = rc.left + newWidth;

                DrawThemeBackground(hTheme_progress_, lpCD->nmcd.hdc, 5, 1, &rc, NULL);

                return CDRF_SKIPDEFAULT;
            }

            break;
        }
        }

        break;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Set new width and height for the torrent list view.
        torrentsView_->Resize(width, height);
        break;
    }

    case WM_TIMER:
    {
        session_->post_dht_stats();
        session_->post_session_stats();
        session_->post_torrent_updates();
        break;
    }

    case WM_PT_ALERT:
    {
        std::vector<lt::alert*> alerts;
        session_->pop_alerts(&alerts);

        for (lt::alert *alert : alerts)
        {
            switch (alert->type())
            {
            case lt::add_torrent_alert::alert_type:
            {
                lt::add_torrent_alert *al = lt::alert_cast<lt::add_torrent_alert>(alert);

                if (al->error)
                {
                    // TODO(log)
                    continue;
                }

                // Save torrent file
                if (al->handle.is_valid()
                    && al->handle.torrent_file())
                {
                    lt::create_torrent ct(*al->handle.torrent_file());
                    lt::entry e = ct.generate();

                    std::vector<char> buf;
                    lt::bencode(std::back_inserter(buf), e);

                    std::wstring hash = lt::convert_to_wstring(lt::to_hex(al->handle.info_hash().to_string()));
                    std::wstring fileName = TEXT("Torrents\\") + hash + TEXT(".torrent");

                    // Create the path to "Torrents/" if it does not exist
                    DWORD torrentAttrs = GetFileAttributes(TEXT("Torrents"));

                    if (torrentAttrs == INVALID_FILE_ATTRIBUTES
                        || !(torrentAttrs & FILE_ATTRIBUTE_DIRECTORY))
                    {
                        CreateDirectory(TEXT("Torrents"), NULL);
                    }

                    HANDLE hFile = CreateFile(
                        fileName.c_str(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

                    if (hFile != INVALID_HANDLE_VALUE)
                    {
                        if (!WriteFile(
                            hFile,
                            &buf[0],
                            buf.size(),
                            NULL,
                            NULL))
                        {
                            // TODO(log)
                        }

                        CloseHandle(hFile);
                    }
                }

                TorrentItem item(al->handle.status());

                torrents_.insert(std::map<lt::sha1_hash, TorrentItem>::value_type(al->handle.info_hash(), item));
                items_[torrents_.size() - 1] = al->handle.info_hash();

                torrentsView_->SetItemCount(items_.size());
                break;
            }
            case lt::state_update_alert::alert_type:
            {
                lt::state_update_alert *al = lt::alert_cast<lt::state_update_alert>(alert);

                for (lt::torrent_status const& st : al->status)
                {
                    TorrentItem &item = torrents_.find(st.info_hash)->second;
                    item.SetDownloadRate(st.download_payload_rate);
                    item.SetName(lt::convert_to_wstring(st.name));
                    item.SetProgress(st.progress);
                    item.SetQueuePosition(st.queue_position);
                    //item.SetSize(st.)
                    item.SetStatus(st.state);
                    item.SetUploadRate(st.upload_payload_rate);
                }

                torrentsView_->Refresh();
                break;
            }
            default:
                break;
            }
        }

        break;
    }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

LRESULT MainWindow::WndProcProxy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        LPVOID pThis = pCreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));
    }

    MainWindow* pWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, 0));
    return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
}
