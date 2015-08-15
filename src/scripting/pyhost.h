#ifndef _PT_SCRIPTING_PYHOST_H
#define _PT_SCRIPTING_PYHOST_H

#include <boost/shared_ptr.hpp>
#include <boost/python/object.hpp>
#include <string>

namespace libtorrent
{
    class session;
}

class PicoTorrent;
class Session;

class PyHost
{
public:
    PyHost(PicoTorrent* pico);

    void Load();
    void Unload();

    static boost::shared_ptr<libtorrent::session> GetSession();
    static bool Prompt(std::string message);
    static void SetApplicationStatus(std::string status);

private:
    static PicoTorrent* pico_;
    boost::python::object ns_;
    void* ts_;
};

#endif
