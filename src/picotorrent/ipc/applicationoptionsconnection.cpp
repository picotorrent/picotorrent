#include "applicationoptionsconnection.hpp"

#include <wx/taskbarbutton.h>

#include "../applicationoptions.hpp"
#include "../mainframe.hpp"
#include "../picojson.hpp"

using pt::ipc::ApplicationOptionsConnection;

ApplicationOptionsConnection::ApplicationOptionsConnection(pt::MainFrame* frame)
    : m_frame(frame)
{
}

bool ApplicationOptionsConnection::OnExecute(const wxString& topic, const void *data, size_t size, wxIPCFormat format)
{
    wxString json = GetTextFromData(data, size, format);

    m_frame->GetEventHandler()->CallAfter([this, json]()
    {
        auto opts = ApplicationOptions::JsonDecode(json);

        if (opts == nullptr)
        {
            // TODO (log??)
            return;
        }

        m_frame->MSWGetTaskBarButton()->Show();
        m_frame->Restore();
        m_frame->Raise();
        m_frame->Show();
        m_frame->HandleOptions(opts);
    });

    return true;
}
