#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <shobjidl.h>

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}
}
namespace client
{
namespace ui
{
    class file_dialog_callback;

    class open_file_dialog
    {
    public:
        open_file_dialog();
        ~open_file_dialog();

        std::vector<core::filesystem::path> get_paths();
        void show(HWND hParent);
        DWORD options();

        void set_folder(const core::filesystem::path &path);
        void set_guid(const GUID &guid);
        void set_ok_button_label(LPCTSTR label);
        void set_options(DWORD options);
        void set_title(LPCTSTR title);

    protected:
        virtual bool on_file_ok() { return true; }
        void notify_error(const std::wstring &mainInstruction, const std::wstring &text);

    private:
        IFileOpenDialog *dlg_;
    };
}
}
}
