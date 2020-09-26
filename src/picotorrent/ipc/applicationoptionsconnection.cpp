#include "applicationoptionsconnection.hpp"

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>
#include <wx/taskbarbutton.h>

#include "../ui/mainframe.hpp"

using json = nlohmann::json;
using pt::IPC::ApplicationOptionsConnection;

ApplicationOptionsConnection::ApplicationOptionsConnection(pt::UI::MainFrame* frame)
    : m_frame(frame)
{
}

bool ApplicationOptionsConnection::OnExecute(const wxString& topic, const void *data, size_t size, wxIPCFormat format)
{
    std::string textData = GetTextFromData(data, size, format);

    m_frame->GetEventHandler()->CallAfter([this, textData]()
        {
            json j;

            try
            {
                j = json::parse(textData);
            }
            catch (std::exception const& e)
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse application options JSON: " << e.what();
                return;
            }

            m_frame->MSWGetTaskBarButton()->Show();
            m_frame->Restore();
            m_frame->Raise();
            m_frame->Show();
            m_frame->HandleParams(j["files"], j["magnet_links"]);
        });

    return true;
}
