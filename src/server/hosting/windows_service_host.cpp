#include <picotorrent/server/hosting/windows_service_host.hpp>

#include <boost/asio.hpp>

using picotorrent::server::hosting::windows_service_host;

windows_service_host* windows_service_host::instance_ = 0;

int windows_service_host::run(const std::shared_ptr<boost::asio::io_service> &io)
{
    if (instance_ == nullptr)
    {
        instance_ = this;
        instance_->io_ = io;
        instance_->signals_ = std::make_shared<boost::asio::signal_set>(*io);
    }

    SERVICE_TABLE_ENTRY tbl[] =
    {
        { TEXT("PicoTorrent"), &windows_service_host::service_main },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(tbl))
    {
        return GetLastError();
    }

    return 0;
}

DWORD windows_service_host::service_control_handler(DWORD control, DWORD event_type, LPVOID event_data, LPVOID context)
{
    windows_service_host* host = static_cast<windows_service_host*>(context);

    switch (control)
    {
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        host->set_status(SERVICE_STOP_PENDING);
        host->signals_->cancel();

        break;
    }

    return NO_ERROR;
}

void windows_service_host::service_main(DWORD dwArgc, LPWSTR *lpSzArgv)
{
    instance_->status_handle_ = RegisterServiceCtrlHandlerEx(
        TEXT("PicoTorrent"),
        &windows_service_host::service_control_handler,
        instance_);

    instance_->signals_->async_wait([](const boost::system::error_code& error, int signal)
    {
        instance_->io_->stop();
        instance_->set_status(SERVICE_STOPPED);
    });

    instance_->set_status(SERVICE_RUNNING);
    instance_->io_->run();
}

void windows_service_host::set_status(DWORD state)
{
    status_.dwCheckPoint = 0;
    status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    status_.dwCurrentState = state;
    status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status_.dwWin32ExitCode = 0;

    if (!SetServiceStatus(status_handle_, &status_))
    {
        // TODO: log
    }
}
