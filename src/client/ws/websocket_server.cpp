#include <picotorrent/client/ws/websocket_server.hpp>

#include <fstream>

#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/security/dh_params.hpp>
#include <picotorrent/client/ws/messages/pico_state_message.hpp>
#include <picotorrent/client/ws/messages/torrent_added_message.hpp>
#include <picotorrent/client/ws/messages/torrent_finished_message.hpp>
#include <picotorrent/client/ws/messages/torrent_removed_message.hpp>
#include <picotorrent/client/ws/messages/torrent_updated_message.hpp>
#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>

#pragma warning(disable : 4503)

namespace ssl = boost::asio::ssl;

namespace msg = picotorrent::client::ws::messages;
using picotorrent::client::configuration;
using picotorrent::client::security::dh_params;
using picotorrent::client::ws::websocket_server;
using picotorrent::core::pal;
using picotorrent::core::session;
using picotorrent::core::torrent;

websocket_server::websocket_server(const std::shared_ptr<session> &session)
    : srv_(std::make_shared<websocketpp_server>()),
    session_(session)
{
    session_->on_torrent_added().connect(std::bind(&websocket_server::on_torrent_added, this, std::placeholders::_1));
    session_->on_torrent_finished().connect(std::bind(&websocket_server::on_torrent_finished, this, std::placeholders::_1));
    session_->on_torrent_removed().connect(std::bind(&websocket_server::on_torrent_removed, this, std::placeholders::_1));
    session_->on_torrent_updated().connect(std::bind(&websocket_server::on_torrent_updated, this, std::placeholders::_1));

    srv_->init_asio(&io_);
    srv_->set_close_handler(std::bind(&websocket_server::on_close, this, std::placeholders::_1));
    srv_->set_message_handler(std::bind(&websocket_server::on_message, this, std::placeholders::_1));
    srv_->set_open_handler(std::bind(&websocket_server::on_open, this, std::placeholders::_1));
    srv_->set_tls_init_handler(std::bind(&websocket_server::on_tls_init, this, std::placeholders::_1));
    srv_->set_validate_handler(std::bind(&websocket_server::on_validate, this, std::placeholders::_1));

    configuration &cfg = configuration::instance();
    certificate_file_ = cfg.websocket_certificate_file();
    configured_token_ = cfg.websocket_access_token();
}

websocket_server::~websocket_server()
{
}

bool websocket_server::is_listening()
{
    return srv_->is_listening();
}

void websocket_server::start()
{
    if (is_listening())
    {
        return;
    }

    bg_ = std::thread(std::bind(&websocket_server::run, this));
}

void websocket_server::stop()
{
    if (!is_listening())
    {
        return;
    }

    srv_->stop_listening();
    bg_.join();
}

std::string websocket_server::get_certificate_password()
{
    return configuration::instance().websocket_certificate_password();
}

void websocket_server::on_close(websocketpp::connection_hdl hdl)
{
    connections_.erase(hdl);
}

void websocket_server::on_message(websocketpp::connection_hdl hdl)
{
}

void websocket_server::on_open(websocketpp::connection_hdl hdl)
{
    connections_.insert(hdl);

    msg::pico_state_message psm(torrents_);
    std::string ser = psm.serialize();

    // Send our pico_state message.
    srv_->get_con_from_hdl(hdl)
        ->send(ser);
}

bool websocket_server::on_validate(websocketpp::connection_hdl hdl)
{
    auto connection = srv_->get_con_from_hdl(hdl);
    std::string token = connection->get_request_header("X-PicoTorrent-Token");

    if (token.empty())
    {
        return false;
    }

    return (configured_token_.compare(token) == 0);
}

context_ptr websocket_server::on_tls_init(websocketpp::connection_hdl hdl)
{
    context_ptr ctx = std::make_shared<ssl::context>(ssl::context::sslv23);
    ctx->set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3 |
        ssl::context::no_tlsv1 |
        ssl::context::single_dh_use);

    configuration &cfg = configuration::instance();

    ctx->set_password_callback(std::bind(&websocket_server::get_certificate_password, this));
    ctx->use_certificate_chain_file(certificate_file_);
    ctx->use_private_key_file(certificate_file_, ssl::context::pem);

    auto dh = dh_params::get();
    SSL_CTX_set_tmp_dh(ctx->native_handle(), dh.get());
    SSL_CTX_set_cipher_list(ctx->native_handle(), cfg.websocket_cipher_list().c_str());

    return ctx;
}

void websocket_server::on_torrent_added(const std::shared_ptr<torrent> &torrent)
{
    io_.post([this, torrent]()
    {
        torrents_.push_back(torrent);
        
        msg::torrent_added_message add(torrent);
        broadcast(add.serialize());
    });
}

void websocket_server::on_torrent_finished(const std::shared_ptr<torrent> &torrent)
{
    io_.post([this, torrent]()
    {
        msg::torrent_finished_message fin(torrent);
        broadcast(fin.serialize());
    });
}

void websocket_server::on_torrent_removed(const std::shared_ptr<torrent> &torrent)
{
    io_.post([this, torrent]()
    {
        auto &f = std::find(torrents_.begin(), torrents_.end(), torrent);
        if (f == torrents_.end()) { return; }
        torrents_.erase(f);

        std::string hash = torrent->info_hash()->to_string();

        msg::torrent_removed_message rem(hash);
        broadcast(rem.serialize());
    });
}

void websocket_server::on_torrent_updated(const std::vector<std::shared_ptr<torrent>> &torrents)
{
    if (torrents.empty())
    {
        return;
    }

    io_.post([this, torrents]()
    {
        msg::torrent_updated_message upd(torrents);
        broadcast(upd.serialize());
    });
}

void websocket_server::broadcast(const std::string &payload)
{
    for (auto hdl : connections_)
    {
        srv_
            ->get_con_from_hdl(hdl)
            ->send(payload);
    }
}

void websocket_server::run()
{
    configuration &cfg = configuration::instance();

    srv_->listen(cfg.websocket_listen_port());
    srv_->start_accept();
    srv_->run();
}
