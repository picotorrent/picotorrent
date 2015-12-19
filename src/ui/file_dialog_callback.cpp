#include <picotorrent/ui/file_dialog_callback.hpp>

using picotorrent::ui::file_dialog_callback;

file_dialog_callback::file_dialog_callback()
    : ref_(1)
{
}

HRESULT file_dialog_callback::create_instance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    file_dialog_callback *cb = new file_dialog_callback();
    HRESULT hr = cb->QueryInterface(riid, ppv);
    cb->Release();

    return hr;
}

IFACEMETHODIMP file_dialog_callback::QueryInterface(REFIID riid, void** ppv)
{
#pragma warning( push )
#pragma warning( disable : 4838 )
    static const QITAB qit[] = {
        QITABENT(file_dialog_callback, IFileDialogEvents),
        { 0 }
    };
#pragma warning( pop )

    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) file_dialog_callback::AddRef()
{
    return InterlockedIncrement(&ref_);
}

IFACEMETHODIMP_(ULONG) file_dialog_callback::Release()
{
    long ref = InterlockedDecrement(&ref_);

    if (!ref)
    {
        delete this;
    }

    return ref;
}

IFACEMETHODIMP file_dialog_callback::OnFileOk(IFileDialog *pfd)
{
    if (file_ok_cb_)
    {
        return file_ok_cb_() ? S_OK : S_FALSE;
    }

    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnFolderChange(IFileDialog *pfd)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnFolderChanging(IFileDialog *pfd, IShellItem *psi)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnOverwrite(IFileDialog *pfd, IShellItem *psi, FDE_OVERWRITE_RESPONSE *pres)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnSelectionChange(IFileDialog *pfd)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnShareViolation(IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pres)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP file_dialog_callback::OnTypeChange(IFileDialog *pfd)
{
    return E_NOTIMPL;
}
