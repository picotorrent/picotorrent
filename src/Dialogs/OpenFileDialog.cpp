#include "OpenFileDialog.hpp"

#include "../stdafx.h"

using Dialogs::OpenFileDialog;

OpenFileDialog::OpenFileDialog()
{
    CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_dlg));
}

OpenFileDialog::~OpenFileDialog()
{
    m_dlg->Release();
}

DWORD OpenFileDialog::GetOptions()
{
    FILEOPENDIALOGOPTIONS options;
    m_dlg->GetOptions(&options);
    return options;
}

std::vector<std::wstring> OpenFileDialog::GetPaths()
{   
    CComPtr<IShellItemArray> items = NULL;
    m_dlg->GetResults(&items);

    if (items == NULL)
    {
        return std::vector<std::wstring>();
    }

    DWORD count;
    items->GetCount(&count);

    std::vector<std::wstring> paths;

    for (DWORD i = 0; i < count; i++)
    {
        CComPtr<IShellItem> item = NULL;
        items->GetItemAt(i, &item);

        PWSTR path = NULL;
        HRESULT res = item->GetDisplayName(SIGDN_FILESYSPATH, &path);

        if (res != S_OK)
        {
            //LOG(warning) << "Could not get display name from path: " << std::hex << res;
            continue;
        }

        paths.push_back(path);
        CoTaskMemFree(path);
    }

    return paths;
}

void OpenFileDialog::SetFileTypes(UINT len, COMDLG_FILTERSPEC* types)
{
    m_dlg->SetFileTypes(len, types);
}

void OpenFileDialog::SetGuid(const GUID guid)
{
    m_dlg->SetClientGuid(guid);
}

void OpenFileDialog::SetOptions(DWORD dwOptions)
{
    m_dlg->SetOptions(dwOptions);
}

void OpenFileDialog::SetTitle(const std::wstring& title)
{
    m_dlg->SetTitle(title.c_str());
}

void OpenFileDialog::Show()
{
    m_dlg->Show(GetActiveWindow());
}
