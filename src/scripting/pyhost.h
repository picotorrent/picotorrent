#ifndef _PT_SCRIPTING_PYHOST_H
#define _PT_SCRIPTING_PYHOST_H

#include <boost/shared_ptr.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/object.hpp>
#include <map>
#include <string>

namespace libtorrent
{
    class session;
    class sha1_hash;
    struct torrent_status;
}

class PicoTorrent;
class Session;

class PyHost
{
public:
    PyHost(PicoTorrent* pico);

    void Load();
    void Unload();
    void OnTorrentItemSelected(const libtorrent::sha1_hash& hash);

    static void AddTorrent(const libtorrent::torrent_status& status);
    static void UpdateTorrents(boost::python::dict torrents);
    static bool Prompt(std::string message);
    static void SetApplicationStatus(std::string status);

private:
    static PicoTorrent* pico_;
    boost::python::object ns_;
    boost::python::object pt_;
    void* ts_;
};

#endif
