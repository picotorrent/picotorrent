#include <picotorrent/client/application.hpp>

#include <picotorrent/client/message_loop.hpp>
#include <picotorrent/client/normal_executor.hpp>
#include <picotorrent/client/other_instance_executor.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <picotorrent/common/command_line.hpp>
#include <picotorrent/common/config/configuration.hpp>
#include <picotorrent/common/logging/log.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/session_configuration.hpp>

#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <shellapi.h>
#include <string.h>

namespace core = picotorrent::core;
namespace ui = picotorrent::client::ui;
using picotorrent::client::application;
using picotorrent::client::normal_executor;
using picotorrent::client::other_instance_executor;
using picotorrent::common::command_line;
using picotorrent::common::config::configuration;
using picotorrent::common::logging::log;

application::application()
{
    SetProcessDPIAware();

    log::instance().set_unhandled_exception_callback(std::bind(&application::on_unhandled_exception, this, std::placeholders::_1));
}

bool application::pre_init()
{
    INITCOMMONCONTROLSEX icex = { 0 };
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);

    if (!InitCommonControlsEx(&icex))
    {
        return false;
    }

    return true;
}

bool application::on_init()
{
    if (is_single_instance())
    {
        executor_ = std::make_shared<normal_executor>(
            get_session(),
            get_websocket_server());
    }
    else if (is_service_running())
    {
        // show service manager thingy
    }
    else
    {
        executor_ = std::make_shared<other_instance_executor>();
    }

    return true;
}

int application::on_run(const command_line &cmd)
{
    if (executor_)
    {
        return executor_->run(cmd);
    }

    return 1;
}

void application::on_torrent_added(const std::shared_ptr<core::torrent> &torrent)
{
    executor_->torrent_added(torrent);
}

void application::on_torrent_finished(const std::shared_ptr<core::torrent> &torrent)
{
    executor_->torrent_finished(torrent);
}

void application::on_torrent_removed(const std::shared_ptr<core::torrent> &torrent)
{
    executor_->torrent_removed(torrent);
}

void application::on_torrent_updated(const std::vector<std::shared_ptr<core::torrent>> &torrents)
{
    executor_->torrent_updated(torrents);
}

bool application::is_service_running()
{
    SC_HANDLE hSCManager = OpenSCManager(
        NULL,
        SERVICES_ACTIVE_DATABASE,
        GENERIC_READ);

    if (hSCManager == NULL)
    {
        return false;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesNum = 0;

    BOOL status = EnumServicesStatusEx(
        hSCManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        NULL,
        0,
        &bytesNeeded,
        &servicesNum,
        NULL,
        NULL);

    std::vector<BYTE> bytes(bytesNeeded);

    status = EnumServicesStatusEx(
        hSCManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_ACTIVE,
        &bytes[0],
        bytesNeeded,
        &bytesNeeded,
        &servicesNum,
        NULL,
        NULL);

    ENUM_SERVICE_STATUS_PROCESS* lpServiceStatus = (ENUM_SERVICE_STATUS_PROCESS*)bytes.data();

    for (DWORD i = 0; i < servicesNum; i++)
    {
        if (lpServiceStatus[i].lpServiceName == TEXT("PicoTorrent"))
        {
            return true;
        }
    }

    return false;
}

void application::wait_for_restart(const command_line &cmd)
{
    if (!cmd.restart())
    {
        return;
    }

    HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, cmd.prev_process_id());

    if (hProc == NULL)
    {
        DWORD err = GetLastError();
        LOG(debug) << "Could not open process: " << err;
        return;
    }

    LOG(debug) << "Waiting for previous instance of PicoTorrent to shut down";
    DWORD res = WaitForSingleObject(hProc, 10000);
    CloseHandle(hProc);

    switch (res)
    {
    case WAIT_FAILED:
        LOG(debug) << "Could not wait for process: " << GetLastError();
        break;
    case WAIT_OBJECT_0:
        LOG(debug) << "Successfully waited for process";
        break;
    case WAIT_TIMEOUT:
        LOG(debug) << "Timeout when waiting for process";
        break;
    }
}

void application::on_notifications_available()
{
    executor_->notification_available();
}

void application::on_unhandled_exception(const std::string &stacktrace)
{
    // TODO: show some sort of dialog
}
