#include <picotorrent/client/controllers/application_update_controller.hpp>

#include <picojson.hpp>
#include <picotorrent/core/version_info.hpp>
#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/logging/log.hpp>
#include <picotorrent/client/net/http_client.hpp>
#include <picotorrent/client/net/http_response.hpp>
#include <picotorrent/client/net/uri.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>
#include <semver.hpp>
#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::core::version_info;
using picotorrent::client::configuration;
using picotorrent::client::controllers::application_update_controller;
using picotorrent::client::net::http_client;
using picotorrent::client::net::http_response;
using picotorrent::client::net::uri;
using picotorrent::client::ui::main_window;

application_update_controller::application_update_controller(
    const std::shared_ptr<main_window> &wnd)
    : http_(std::make_shared<http_client>()),
    wnd_(wnd)
{
}

application_update_controller::~application_update_controller()
{
}

void application_update_controller::execute(bool forced)
{
    configuration &cfg = configuration::instance();

    uri api(cfg.update_url());
    http_->get_async(api, std::bind(&application_update_controller::on_response, this, std::placeholders::_1, forced));
}

void application_update_controller::on_response(const http_response &response, bool forced)
{
    if (!response.is_success_status())
    {
        LOG(error) << "GitHub returned status " << response.status_code();
        return;
    }

    picojson::value v;
    std::string err = picojson::parse(v, response.content());

    if (!err.empty())
    {
        LOG(error) << "Error when parsing release JSON: " << err;
        return;
    }

    picojson::object obj = v.get<picojson::object>();
    std::string version = obj["tag_name"].get<std::string>();
    if (version[0] == 'v') { version = version.substr(1); }

    semver::version parsedVersion(version);
    configuration &cfg = configuration::instance();

    if (cfg.ignored_update() == version
        && !forced)
    {
        // Just return if we have ignored this update.
        return;
    }

    if (parsedVersion > semver::version(version_info::current_version()))
    {
        WCHAR title[100];
        StringCchPrintf(
            title,
            ARRAYSIZE(title),
            to_wstring(TR("picotorrent_v_available")).c_str(),
            to_wstring(version).c_str());

        uri uri(obj["html_url"].get<std::string>());
        notify(to_string(title), uri, version);
    }
    else if (forced)
    {
        ui::task_dialog dlg;
        dlg.set_common_buttons(TDCBF_OK_BUTTON);
        dlg.set_main_icon(TD_INFORMATION_ICON);
        dlg.set_parent(wnd_->handle());
        dlg.set_main_instruction(TR("no_update_available"));
        dlg.set_title("PicoTorrent");
        dlg.show();
    }
}

void application_update_controller::notify(const std::string &title, const uri& uri, const std::string &version)
{
    ui::task_dialog dlg;
    dlg.add_button(1000, TR("show_on_github"),
        [this, uri]()
    {
        ShellExecute(
            wnd_->handle(),
            L"open",
            to_wstring(uri.raw()).c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL);

        return false;
    });

    dlg.set_common_buttons(TDCBF_CLOSE_BUTTON);
    dlg.set_content(TR("new_version_available"));
    dlg.set_main_icon(TD_INFORMATION_ICON);
    dlg.set_main_instruction(title);
    dlg.set_parent(wnd_->handle());
    dlg.set_title("PicoTorrent");
    dlg.set_verification_text(TR("ignore_update"));
    dlg.show();

    if (dlg.is_verification_checked())
    {
        configuration &cfg = configuration::instance();
        cfg.set_ignored_update(version);
    }
}
