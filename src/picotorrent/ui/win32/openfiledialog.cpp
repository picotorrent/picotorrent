#include "openfiledialog.hpp"

#ifdef _WIN32
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#endif

#include <string>

#include "../../core/utils.hpp"

using pt::UI::Win32::OpenFileDialog;

OpenFileDialog::OpenFileDialog()
    : m_wrappedDialog(nullptr)
{
#ifdef _WIN32
    if (!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_wrappedDialog))))
    {
        throw std::exception();
    }
#endif
}

OpenFileDialog::~OpenFileDialog()
{
#ifdef _WIN32
    m_wrappedDialog->Release();
#endif
}

void OpenFileDialog::GetFiles(std::vector<std::string>& files)
{
#ifdef _WIN32
    IShellItemArray* results = nullptr;

    if (!SUCCEEDED(m_wrappedDialog->GetResults(&results))
        || results == nullptr)
    {
        return;
    }

    DWORD count;
    results->GetCount(&count);

    files.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        PWSTR path = nullptr;

        IShellItem* item;
        results->GetItemAt(i, &item);
        
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))
            && path != nullptr)
        {
            files.push_back(
                Utils::toStdString(path));
            CoTaskMemFree(path);
        }

        item->Release();
    }

    results->Release();
#endif
}

void OpenFileDialog::SetFileTypes(std::vector<std::tuple<std::wstring, std::wstring>> const& types)
{
#ifdef _WIN32
    std::vector<COMDLG_FILTERSPEC> spec;

    for (auto const& t : types)
    {
        spec.push_back({
                std::get<0>(t).c_str(),
                std::get<1>(t).c_str() });
    }

    m_wrappedDialog->SetFileTypes(spec.size(), spec.data());
#endif
}

void OpenFileDialog::SetOption(OpenFileDialog::Option option)
{
#ifdef _WIN32
    DWORD dwOptions;
    m_wrappedDialog->GetOptions(&dwOptions);
    if (option == Option::Multi) { dwOptions |= FOS_ALLOWMULTISELECT; }
    m_wrappedDialog->SetOptions(dwOptions);
#endif
}

void OpenFileDialog::SetTitle(std::wstring const& title)
{
#ifdef _WIN32
    m_wrappedDialog->SetTitle(title.c_str());
#endif
}

void OpenFileDialog::Show(wxWindow* parent)
{
#ifdef _WIN32
    m_wrappedDialog->Show(parent->GetHWND());
#endif
}
