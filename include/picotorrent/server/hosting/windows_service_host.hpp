#pragma once

#include <memory>

#include <boost/asio/signal_set.hpp>
#include <windows.h>

namespace boost { namespace asio { class io_service; } }

namespace picotorrent
{
namespace server
{
namespace hosting
{
    class windows_service_host
    {
    public:
        int run(const std::shared_ptr<boost::asio::io_service> &io);

    private:
        void set_status(DWORD state);

        static DWORD WINAPI service_control_handler(DWORD control, DWORD event_type, LPVOID event_data, LPVOID context);
        static void WINAPI service_main(DWORD dwArgc, LPWSTR *lpSzArgv);
        static windows_service_host* instance_;

        std::shared_ptr<boost::asio::io_service> io_;
        std::shared_ptr<boost::asio::signal_set> signals_;
        SERVICE_STATUS status_;
        SERVICE_STATUS_HANDLE status_handle_;
    };
}
}
}
