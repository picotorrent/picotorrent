#pragma once

#include <memory>
#include <string>

#include <picotorrent/client/ui/dialogs/dialog_base.hpp>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class qr_code;
}
namespace ui
{
namespace dialogs
{
    class remote_qr_dialog : public dialog_base
    {
    public:
        remote_qr_dialog();
        ~remote_qr_dialog();

        void set_data(const std::string &data);

    protected:
        BOOL on_command(int, WPARAM, LPARAM);
        BOOL on_init_dialog();
        BOOL on_notify(LPARAM);

    private:
        std::shared_ptr<qr::qr_code> qr_;
    };
}
}
}
}
