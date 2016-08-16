#pragma once

#include <windows.h>
#include <shobjidl.h>

#include <string>
#include <vector>

namespace Dialogs
{
class OpenFileDialog
{
public:
    OpenFileDialog();
    ~OpenFileDialog();

    DWORD GetOptions();
    std::vector<std::wstring> GetPaths();

    void SetFileTypes(UINT len, COMDLG_FILTERSPEC* types);
    void SetGuid(const GUID guid);
    void SetOptions(DWORD dwOptions);
    void SetTitle(const std::wstring& title);
    void Show();

private:
    IFileOpenDialog* m_dlg;
};
}
