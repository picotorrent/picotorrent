#include <picotorrent/client/ui/open_file_dialog.hpp>

#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/client/ui/file_dialog_callback.hpp>

namespace fs = picotorrent::core::filesystem;
using picotorrent::client::ui::file_dialog_callback;
using picotorrent::client::ui::open_file_dialog;

open_file_dialog::open_file_dialog()
    : dlg_(NULL)
{
    CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dlg_));
}

open_file_dialog::~open_file_dialog()
{
    dlg_->Release();
}

std::vector<fs::path> open_file_dialog::get_paths()
{
    IShellItemArray *items = NULL;
    dlg_->GetResults(&items);

    if (items == NULL)
    {
        return std::vector<fs::path>();
    }

    DWORD count;
    items->GetCount(&count);

    std::vector<fs::path> paths;

    for (DWORD i = 0; i < count; i++)
    {
        IShellItem* item = NULL;
        items->GetItemAt(i, &item);

        PWSTR path = NULL;
        item->GetDisplayName(SIGDN_FILESYSPATH, &path);

        fs::path p(path);
        paths.push_back(p);

        CoTaskMemFree(path);
        item->Release();
    }

    items->Release();

    return paths;
}

void open_file_dialog::notify_error(const std::wstring &mainInstruction, const std::wstring &text)
{
    IOleWindow *pWindow = NULL;
    HRESULT hr = dlg_->QueryInterface(IID_PPV_ARGS(&pWindow));

    HWND hDlg;
    pWindow->GetWindow(&hDlg);

    TaskDialog(
        hDlg,
        NULL,
        L"Error",
        L"An error occured",
        text.c_str(),
        TDCBF_OK_BUTTON,
        TD_ERROR_ICON,
        NULL);
}

DWORD open_file_dialog::options()
{
    DWORD flags = 0;
    dlg_->GetOptions(&flags);
    return flags;
}

void open_file_dialog::set_folder(const fs::path &path)
{
    IShellItem *folder;
    HRESULT res = SHCreateItemFromParsingName(path.to_string().c_str(), NULL, IID_PPV_ARGS(&folder));

    if (SUCCEEDED(res))
    {
        dlg_->SetFolder(folder);
        folder->Release();
    }
}

void open_file_dialog::set_guid(const GUID &guid)
{
    dlg_->SetClientGuid(guid);
}

void open_file_dialog::set_ok_button_label(LPCTSTR label)
{
    dlg_->SetOkButtonLabel(label);
}

void open_file_dialog::set_options(DWORD options)
{
    dlg_->SetOptions(options);
}

void open_file_dialog::set_title(LPCTSTR title)
{
    dlg_->SetTitle(title);
}

void open_file_dialog::show(HWND hParent)
{
    COMDLG_FILTERSPEC fileTypes[] =
    {
        { L"Torrent files", L"*.torrent" },
        { L"All files", L"*.*" }
    };

    file_dialog_callback cb;
    cb.file_ok_cb_ = std::bind(&open_file_dialog::on_file_ok, this);

    DWORD cookie = 0;
    dlg_->Advise(&cb, &cookie);
    dlg_->SetFileTypes(_countof(fileTypes), fileTypes);
    dlg_->Show(hParent);

    dlg_->Unadvise(cookie);
}
