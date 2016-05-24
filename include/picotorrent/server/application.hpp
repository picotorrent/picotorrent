#pragma once

#include <picotorrent/common/application.hpp>

namespace boost { namespace asio { class io_service; } }

namespace picotorrent
{
namespace server
{
    class application : public common::application
    {
    public:
        DLL_EXPORT void allocate_console();

    protected:
        DLL_EXPORT bool on_init();
        DLL_EXPORT void on_notifications_available();
        DLL_EXPORT int on_run(const common::command_line &cmd);

    private:
        std::shared_ptr<boost::asio::io_service> io_;
    };
}
}
