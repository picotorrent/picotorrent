#ifndef _PT_SCRIPTING_PYHOST_H
#define _PT_SCRIPTING_PYHOST_H

#include <boost/shared_ptr.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>
#include <string>
#include <vector>

namespace libtorrent
{
    class session;
    class sha1_hash;
    struct torrent_status;
}

class AddTorrentController;
class PicoTorrent;
class Session;

class PyHost
{
public:
    PyHost(PicoTorrent* pico);

    void Init();
    void Load();
    void Unload();

    void OnInstanceAlreadyRunning();
    void OnMenuItemClicked(int id);
    void OnTorrentItemActivated(const libtorrent::sha1_hash& hash);
    void OnTorrentItemSelected(const libtorrent::sha1_hash& hash);

    static void AddTorrent(const libtorrent::torrent_status& status);
    static void Exit();
    static void Log(std::string message);
    static void UpdateTorrents(boost::python::dict torrents);
    static bool Prompt(std::string message);
    static void SetApplicationStatus(std::string status);
    static void ShowAddTorrentDialog(boost::shared_ptr<AddTorrentController> controller);
    static boost::python::tuple ShowOpenFileDialog();

private:
    std::string GetPyPath();

    static PicoTorrent* pico_;
    boost::python::object ns_;
    boost::python::object pt_;
    void* ts_;
};

#endif
