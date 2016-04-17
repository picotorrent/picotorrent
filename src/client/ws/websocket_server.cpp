#include <picotorrent/client/ws/websocket_server.hpp>

#include <fstream>

#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/security/certificate_manager.hpp>
#include <picotorrent/client/security/dh_params.hpp>
#include <picotorrent/client/security/random_string_generator.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>

#pragma warning(disable : 4503)

#define DEFAULT_TOKEN_SIZE 20

namespace ssl = boost::asio::ssl;
using picotorrent::client::configuration;
using picotorrent::client::security::certificate_manager;
using picotorrent::client::security::dh_params;
using picotorrent::client::security::random_string_generator;
using picotorrent::client::ws::websocket_server;
using picotorrent::core::pal;
using picotorrent::core::session;
using picotorrent::core::torrent;

websocket_server::websocket_server(const std::shared_ptr<session> &session)
    : srv_(std::make_shared<websocketpp_server>()),
    session_(session)
{
    session_->on_torrent_added().connect(std::bind(&websocket_server::on_torrent_added, this, std::placeholders::_1));
    session_->on_torrent_removed().connect(std::bind(&websocket_server::on_torrent_removed, this, std::placeholders::_1));
    session_->on_torrent_updated().connect(std::bind(&websocket_server::on_torrent_updated, this, std::placeholders::_1));

    srv_->init_asio(&io_);
    srv_->set_close_handler(std::bind(&websocket_server::on_close, this, std::placeholders::_1));
    srv_->set_message_handler(std::bind(&websocket_server::on_message, this, std::placeholders::_1));
    srv_->set_open_handler(std::bind(&websocket_server::on_open, this, std::placeholders::_1));
    srv_->set_tls_init_handler(std::bind(&websocket_server::on_tls_init, this, std::placeholders::_1));
    srv_->set_validate_handler(std::bind(&websocket_server::on_validate, this, std::placeholders::_1));

    configuration &cfg = configuration::instance();
    configured_token_ = cfg.websocket_access_token();

    if (configured_token_.empty())
    {
        random_string_generator rsg;
        configured_token_ = rsg.generate(DEFAULT_TOKEN_SIZE);
        cfg.set_websocket_access_token(configured_token_);
    }
}

websocket_server::~websocket_server()
{
}

void websocket_server::start()
{
    bg_ = std::thread(std::bind(&websocket_server::run, this));
}

void websocket_server::stop()
{
    srv_->get_io_service().stop();
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
    std::string certificate_file = cfg.websocket_certificate_file();

    if (!pal::file_exists(certificate_file))
    {
        // Create an automatically generated SSL certificate.
        auto v = certificate_manager::generate();
        std::ofstream co(certificate_file, std::ios::binary);
        co.write(&v[0], v.size());
    }

    ctx->set_password_callback(std::bind(&websocket_server::get_certificate_password, this));
    ctx->use_certificate_chain_file(certificate_file);
    ctx->use_private_key_file(certificate_file, ssl::context::pem);

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
        // TODO: emit torrent_added event.
    });
}

void websocket_server::on_torrent_removed(const std::shared_ptr<torrent> &torrent)
{
    io_.post([this, torrent]()
    {
        auto &f = std::find(torrents_.begin(), torrents_.end(), torrent);
        if (f == torrents_.end()) { return; }
        torrents_.erase(f);
        // TODO: emit torrent_removed event.
    });
}

void websocket_server::on_torrent_updated(const std::vector<std::shared_ptr<torrent>> &torrents)
{
    io_.post([this, torrents]()
    {
        // TODO: emit torrent_updated event.
    });
}

void websocket_server::run()
{
    configuration &cfg = configuration::instance();

    srv_->listen(cfg.websocket_listen_port());
    srv_->start_accept();
    srv_->run();
}
