#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <shobjidl.h>

namespace picotorrent
{
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

        std::vector<std::string> get_paths();
        void show(HWND hParent);
        DWORD options();

        void set_folder(const std::string &path);
        void set_guid(const GUID &guid);
        void set_ok_button_label(const std::string &label);
        void set_options(DWORD options);
        void set_title(const std::string &title);

    protected:
        virtual bool on_file_ok() { return true; }
        void notify_error(const std::string &mainInstruction, const std::string &text);

    private:
        IFileOpenDialog *dlg_;
    };
}
}
}
