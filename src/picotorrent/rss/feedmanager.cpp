#include "feedmanager.hpp"

#include <wx/sstream.h>
#include <wx/xml/xml.h>

#include "../http/httpclient.hpp"

using pt::RSS::FeedManager;

FeedManager::FeedManager()
    : m_httpClient(std::make_unique<Http::HttpClient>())
{
}

FeedManager::~FeedManager()
{
}
