#ifndef _PT_SCRIPTING_PYHOST_H
#define _PT_SCRIPTING_PYHOST_H

#include <string>

class PicoTorrent;

class PyHost
{
public:
    PyHost(PicoTorrent* pico);

    void Load();
    void Unload();

    static void SetApplicationStatus(std::string status);

private:
    static PicoTorrent* pico_;
    void* ts_;
};

#endif
