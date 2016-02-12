#include <picotorrent/client/controllers/unhandled_exception_controller.hpp>

#include <picojson.hpp>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/client/net/http_client.hpp>
#include <picotorrent/client/ui/main_window.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>
#include <windows.h>
#include <shellapi.h>

namespace ui = picotorrent::client::ui;
using picotorrent::core::to_string;
using picotorrent::core::to_wstring;
using picotorrent::client::controllers::unhandled_exception_controller;

unhandled_exception_controller::unhandled_exception_controller(
    const std::shared_ptr<ui::main_window> &wnd,
    const std::string &stacktrace)
    : wnd_(wnd),
    stacktrace_(stacktrace)
{
}

void unhandled_exception_controller::execute()
{
    wnd_->hide();

    ui::task_dialog dlg;
    dlg.add_button(
        1234,
        L"Create GitHub issue\nRequires a GitHub account",
        [this]()
    {
        picojson::object paste;
        paste.insert({ "key", picojson::value("<api-key>") });
        paste.insert({ "paste", picojson::value(stacktrace_) });

        picotorrent::client::net::http_client http;
        std::string result = http.post(L"https://paste.ee/api", picojson::value(paste).serialize());

        picojson::value v;
        picojson::parse(v, result);

        picojson::object obj = v.get<picojson::object>();

        if (obj.at("status").get<std::string>() != "success")
        {
            return false;
        }

        std::string rawUrl = obj.at("paste").get<picojson::object>().at("raw").get<std::string>();
        ShellExecuteA(
            wnd_->handle(),
            "open",
            std::string("https://github.com/picotorrent/picotorrent/issues/new?body=PicoTorrent%20crashed%20with%20the%20following%20stack%20trace%3B%20" + rawUrl).c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL);

        return false;
    });

    dlg.set_content(to_wstring(stacktrace_).c_str());
    dlg.set_main_icon(TD_ERROR_ICON);
    dlg.set_main_instruction(L"Unhandled exception occured");
    dlg.set_parent(wnd_->handle());
    dlg.set_title(L"PicoTorrent");
    dlg.show();
}
