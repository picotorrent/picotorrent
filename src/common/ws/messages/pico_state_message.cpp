#include <picotorrent/common/ws/messages/pico_state_message.hpp>

#include <picojson.hpp>
#include <picotorrent/common/ws/serialization/torrent_serializer.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/version_info.hpp>

namespace pj = picojson;
using picotorrent::common::ws::messages::pico_state_message;
using picotorrent::common::ws::serialization::torrent_serializer;
using picotorrent::core::torrent;
using picotorrent::core::version_info;

pico_state_message::pico_state_message(const std::vector<std::shared_ptr<torrent>> &torrents)
    : torrents_(torrents)
{
}

std::string pico_state_message::serialize()
{
    pj::object obj;
    obj["type"] = pj::value("pico_state");

    pj::object vi;
    vi["api_version"] = pj::value((int64_t)1);
    vi["picotorrent_version"] = pj::value(version_info::current_version());
    vi["git_branch"] = pj::value(version_info::git_branch());
    vi["git_commit_hash"] = pj::value(version_info::git_commit_hash());

    obj["version_info"] = pj::value(vi);

    pj::array torrents;

    for (auto &torrent : torrents_)
    {
        pj::object t = torrent_serializer::serialize(torrent);
        torrents.push_back(pj::value(t));
    }

    obj["torrents"] = pj::value(torrents);

    return pj::value(obj).serialize();
}
