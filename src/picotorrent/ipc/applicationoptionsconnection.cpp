#include "applicationoptionsconnection.hpp"

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>
#include <wx/taskbarbutton.h>

#include "../applicationoptions.hpp"
#include "../ui/mainframe.hpp"

using json = nlohmann::json;
using pt::IPC::ApplicationOptionsConnection;



ApplicationOptionsConnection::ApplicationOptionsConnection(pt::UI::MainFrame* frame)
    : m_frame(frame)
{
}

bool ApplicationOptionsConnection::OnExecute(const wxString&, const void *data, size_t size, wxIPCFormat format)
{
    std::string textData = GetTextFromData(data, size, format).ToStdString();

    m_frame->GetEventHandler()->CallAfter([this, textData]()
        {
            json j;
            pt::CommandLineOptions options;

            try
            {
                j = json::parse(textData);
                options.files = j["files"].get<std::vector<std::string>>();
                options.magnets = j["magnet_links"].get<std::vector<std::string>>();
                options.save_path = j["save_path"].get<std::string>();
                options.silent = j["silent"].get<bool>();
            }
            catch (std::exception const& e)
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse application options JSON: " << e.what();
                return;
            }

            // m_frame->MSWGetTaskBarButton()->Show();

            if (m_frame->IsIconized())
            {
                m_frame->Restore();
            }

            m_frame->Raise();
            m_frame->Show();
            m_frame->HandleParams(options);
        });

    return true;
}
