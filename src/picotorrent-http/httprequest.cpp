#include <picotorrent/http/httprequest.hpp>

using pt::HttpRequest;

HttpRequest::HttpRequest(QUrl const& url)
{
    m_url = new QUrl(url);
}

HttpRequest::~HttpRequest()
{
    delete m_url;
}

QUrl& HttpRequest::url() const
{
    return *m_url;
}
