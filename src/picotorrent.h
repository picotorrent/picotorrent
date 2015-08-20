#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <memory>
#include <wx/app.h>
#include <wx/listctrl.h>
#include <wx/snglinst.h>
#include <wx/timer.h>

namespace libtorrent
{
    class sha1_hash;
    struct torrent_status;
}

class AddTorrentController;
class LogFrame;
class MainFrame;
class PyHost;

class PicoTorrent : public wxApp
{
public:
    PicoTorrent();
    ~PicoTorrent();

    virtual bool OnInit();

    virtual int OnExit();

    void AddTorrent(const libtorrent::torrent_status& status);

    void AppendLog(std::string message);

    void UpdateTorrents(std::map<libtorrent::sha1_hash, libtorrent::torrent_status> status);
    
    bool Prompt(const wxString& message);

    void SetApplicationStatusText(const wxString& text);

    void ShowAddTorrentDialog(boost::shared_ptr<AddTorrentController> controller);

    bool ShowOpenFileDialog(std::vector<std::string>& files);

protected:
    void OnMenu(wxCommandEvent& event);
    void OnTorrentItemActivated(wxListEvent& event);
    void OnTorrentItemSelected(wxListEvent& event);

private:
    std::unique_ptr<PyHost> pyHost_;
    LogFrame* logFrame_;
    MainFrame* mainFrame_;
    wxSingleInstanceChecker* single_;

    wxDECLARE_EVENT_TABLE();
};

#endif
