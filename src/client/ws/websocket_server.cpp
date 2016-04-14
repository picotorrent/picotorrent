#include <picotorrent/client/ws/websocket_server.hpp>

#include <fstream>

#include <picotorrent/client/configuration.hpp>
#include <picotorrent/client/security/certificate_manager.hpp>
#include <picotorrent/client/security/dh_params.hpp>
#include <picotorrent/core/pal.hpp>

#pragma warning(disable : 4503)

namespace ssl = boost::asio::ssl;
using picotorrent::client::configuration;
using picotorrent::client::security::certificate_manager;
using picotorrent::client::security::dh_params;
using picotorrent::client::ws::websocket_server;
using picotorrent::core::pal;

websocket_server::websocket_server()
    : srv_(std::make_shared<websocketpp_server>())
{
    srv_->init_asio();
    srv_->set_message_handler(std::bind(&websocket_server::on_message, this, std::placeholders::_1));
    srv_->set_tls_init_handler(std::bind(&websocket_server::on_tls_init, this, std::placeholders::_1));
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

void websocket_server::on_message(websocketpp::connection_hdl hdl)
{
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

void websocket_server::run()
{
    configuration &cfg = configuration::instance();

    srv_->listen(cfg.websocket_listen_port());
    srv_->start_accept();
    srv_->run();
}
