#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt::Http { class HttpClient; }

namespace pt::RSS
{
    class FeedManager : public wxEvtHandler
    {
    public:
        FeedManager();
        virtual ~FeedManager();

    private:
        std::unique_ptr<Http::HttpClient> m_httpClient;
    };
}
