#include "Torrent.h"

#include <picotorrent/core/torrent.hpp>

#include <msclr/marshal_cppstd.h>

using PicoTorrent::Core::Torrent;

Torrent::Torrent(picotorrent::core::torrent* torrent)
    : torrent_(torrent)
{
}

System::String^ Torrent::Name::get()
{
    return msclr::interop::marshal_as<System::String^>(torrent_->name());
}

void Torrent::Pause()
{
    torrent_->pause();
}

void Torrent::Resume(bool force)
{
    torrent_->resume(force);
}
