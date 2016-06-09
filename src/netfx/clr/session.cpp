#include <picotorrent/netfx/clr/session.hpp>

#include <picotorrent/core/session.hpp>

using picotorrent::core::session;
using PicoTorrent::Core::Session;

Session::Session(session* sess)
    : s_(sess)
{
}
