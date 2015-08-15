#include "picotorrent.h"
#include "common.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <wx/msgdlg.h>

#include "config.h"
#include "fsutil.h"
#include "platform.h"
#include "scripting/pyhost.h"
#include "ui/mainframe.h"

wxBEGIN_EVENT_TABLE(PicoTorrent, wxApp)
wxEND_EVENT_TABLE()

PicoTorrent::PicoTorrent()
{
    mainFrame_ = new MainFrame();
    pyHost_ = std::unique_ptr<PyHost>(new PyHost(this));
}

PicoTorrent::~PicoTorrent()
{
}

bool PicoTorrent::OnInit()
{
    if(!wxApp::OnInit())
    {
        return false;
    }

    // Show our main frame.
    mainFrame_->Show(true);

    // Load the python scripting host as the last thing
    pyHost_->Load();

    return true;
}

int PicoTorrent::OnExit()
{
    pyHost_->Unload();
    return wxApp::OnExit();
}

bool PicoTorrent::Prompt(const wxString& text)
{
    return wxMessageBox(text, "PicoTorrent", wxOK | wxCANCEL, mainFrame_) == wxOK;
}

void PicoTorrent::SetApplicationStatusText(const wxString& text)
{
    if (mainFrame_)
    {
        mainFrame_->SetStatusText(text, 0);
    }
}
