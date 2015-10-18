#pragma once

#include <functional>
#include <windows.h>
#include <shlwapi.h>
#include <shobjidl.h>

namespace picotorrent
{
namespace ui
{
    class file_dialog_callback
        : public IFileDialogEvents
    {
        friend class open_file_dialog;

    public:
        file_dialog_callback();
        
        static HRESULT create_instance(REFIID riid, void **ppv);

        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();

        IFACEMETHODIMP OnFileOk(IFileDialog *pfd);
        IFACEMETHODIMP OnFolderChange(IFileDialog *pfd);
        IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*);
        IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*);
        IFACEMETHODIMP OnSelectionChange(IFileDialog*);
        IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*);
        IFACEMETHODIMP OnTypeChange(IFileDialog*);

    private:
        std::function<bool()> file_ok_cb_;
        long ref_;
    };
}
}
