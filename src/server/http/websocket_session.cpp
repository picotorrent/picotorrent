#include "websocket_session.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using pt::http::websocket_session;

websocket_session::websocket_session(
    boost::asio::ip::tcp::socket&& socket,
    sqlite3* db,
    std::shared_ptr<pt::session_manager> session)
    : m_websocket(std::move(socket)),
    m_db(db),
    m_session(session)
{
    /*m_subscriber_tag = m_session->subscribe(
        std::bind(&websocket_session::on_subscribe, this, std::placeholders::_1));*/
}

void websocket_session::begin_accept(boost::system::error_code ec)
{
    if (ec)
    {
        return;
    }

    json j;
    j["type"] = "init";
    j["torrents"] = json::object();

    /*m_session->for_each_torrent(
        [&j](auto const& ts)
        {
            std::stringstream ss;
            if (ts.info_hashes.has_v2()) { ss << ts.info_hashes.v2; }
            else { ss << ts.info_hashes.v1; }

            json torrent;
            torrent["name"] = ts.name;
            torrent["progress"] = ts.progress;
            torrent["save_path"] = ts.save_path;
            torrent["size_wanted"] = ts.total_wanted;
            torrent["state"] = ts.state;
            torrent["dl"] = ts.download_payload_rate;
            torrent["ul"] = ts.upload_payload_rate;
            torrent["info_hash"] = ss.str();

            j["torrents"][ss.str()] = torrent;

            return true;
        });*/

    BOOST_LOG_TRIVIAL(debug) << "Sending initial state to client";

    m_send_data.push(j.dump());
    maybe_write();

    begin_read();
}

void websocket_session::begin_read()
{
    m_websocket.async_read(
        m_buffer,
        boost::beast::bind_front_handler(
            &websocket_session::end_read,
            shared_from_this()));
}

void websocket_session::end_read(boost::system::error_code ec, std::size_t bytes_transferred)
{
    if (ec == boost::beast::websocket::error::closed)
    {
        return;
    }

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when reading from WebSocket: " << ec.message();
        return;
    }

    begin_read();
}

void websocket_session::end_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    m_is_writing = false;
    m_send_data.pop();

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to write " << bytes_transferred << " bytes: " << ec;
        return;
    }

    maybe_write();
}

void websocket_session::maybe_write()
{
    if (m_is_writing
        || m_send_data.empty()) { return; };

    std::string& d = m_send_data.front();

    m_websocket.async_write(
        boost::asio::buffer(d),
        boost::beast::bind_front_handler(
            &websocket_session::end_write,
            shared_from_this()));

    m_is_writing = true;
}

void websocket_session::on_subscribe(json& j)
{
    m_send_data.push(j.dump());
    maybe_write();
}
