#include "picotorrent.h"
#include "common.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <libtorrent/torrent_handle.hpp>
#include <wx/msgdlg.h>

#include "config.h"
#include "platform.h"
#include "scripting/pyhost.h"
#include "ui/logframe.h"
#include "ui/mainframe.h"

namespace lt = libtorrent;

wxBEGIN_EVENT_TABLE(PicoTorrent, wxApp)
    EVT_LIST_ITEM_ACTIVATED(4000, PicoTorrent::OnTorrentItemActivated)
    EVT_LIST_ITEM_SELECTED(4000, PicoTorrent::OnTorrentItemSelected)

    EVT_MENU(wxID_ANY, PicoTorrent::OnViewLog)
wxEND_EVENT_TABLE()

PicoTorrent::PicoTorrent()
{
    mainFrame_ = new MainFrame();
    logFrame_ = new LogFrame(mainFrame_);

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

    // Initialize the Python scripting host. We always do this, even when
    // an instance of PicoTorrent is already running. We later call
    // 'picotorrent.on_instance_already_running()' which takes care of passing any args
    // to the already running instance.
    pyHost_->Init();

    // Check if we are the only instance of PicoTorrent.
    single_ = new wxSingleInstanceChecker();

    if (single_->IsAnotherRunning())
    {
        delete single_;
        single_ = NULL;

        // Let Python know and then shut down.
        pyHost_->OnInstanceAlreadyRunning();

        return false;
    }

    mainFrame_->Show(true);

    // Load the python scripting host as the last thing
    pyHost_->Load();

    return true;
}

int PicoTorrent::OnExit()
{
    pyHost_->Unload();
    delete single_;

    // Save the configuration
    Config::GetInstance().Save();

    return wxApp::OnExit();
}

void PicoTorrent::AddTorrent(const lt::torrent_status& status)
{
    mainFrame_->AddTorrent(status);
}

void PicoTorrent::AppendLog(std::string message)
{
    logFrame_->AppendLog(message);
}

void PicoTorrent::UpdateTorrents(std::map<lt::sha1_hash, lt::torrent_status> status)
{
    mainFrame_->UpdateTorrents(status);
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

void PicoTorrent::OnTorrentItemActivated(wxListEvent& event)
{
    lt::sha1_hash* hash = (lt::sha1_hash*)event.GetItem().GetData();
    pyHost_->OnTorrentItemActivated(*hash);
}

void PicoTorrent::OnTorrentItemSelected(wxListEvent& event)
{
    lt::sha1_hash* hash = (lt::sha1_hash*)event.GetItem().GetData();
    pyHost_->OnTorrentItemSelected(*hash);
}

void PicoTorrent::OnViewLog(wxCommandEvent& event)
{
    int id = event.GetId();
    pyHost_->OnMenuItemClicked(id);
}
