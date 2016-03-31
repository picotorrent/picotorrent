#pragma once

#include <picotorrent/core/signals/signal.hpp>
#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <commctrl.h>
#include <uxtheme.h>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace controls
{
    class list_view;
}
namespace property_sheets
{
namespace details
{
    class files_page : public property_sheet_page
    {
    public:
        files_page();
        ~files_page();

        void add_file(const std::string &name, uint64_t size, float progress, int priority);
        core::signals::signal_connector<void, const std::pair<int, int>&>& on_set_file_priority();
        void refresh();
        void update_file_progress(int index, float progress);

    protected:
        void on_init_dialog();
        std::string on_list_display(const std::pair<int, int> &p);
        void on_list_item_context_menu(const std::vector<int> &indices);
        int on_list_item_image(const std::pair<int, int> &p);
        float on_list_progress(const std::pair<int, int> &p);

    private:
        struct file_item;

        HIMAGELIST images_;

        std::unique_ptr<controls::list_view> files_;
        std::map<std::string, int> icon_map_;
        std::vector<file_item> items_;
        core::signals::signal<void, const std::pair<int, int>&> on_set_file_prio_;
    };
}
}
}
}
}
