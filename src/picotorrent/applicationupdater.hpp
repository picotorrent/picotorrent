#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    namespace http
    {
        class HttpClient;
        struct HttpResponse;
    }

    class Configuration;
    class Translator;

    class ApplicationUpdater
    {
    public:
        ApplicationUpdater(wxFrame* parent, std::shared_ptr<Configuration> cfg, std::shared_ptr<Translator> translator);
        ~ApplicationUpdater();

        void Check(bool force);

    private:
        void OnHttpResponse(http::HttpResponse const& response, bool force);
        void ShowUpdateDialog(std::string const& version, std::string& url);
        void ShowNoUpdateDialog();

        wxFrame* m_parent;

        std::shared_ptr<Configuration> m_config;
        std::shared_ptr<Translator> m_translator;
        std::unique_ptr<http::HttpClient> m_httpClient;
    };
}