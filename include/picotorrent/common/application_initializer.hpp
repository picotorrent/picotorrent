#pragma once

namespace picotorrent
{
namespace common
{
    class application_initializer
    {
    public:
        void create_application_paths();
        void generate_websocket_access_token();
        void generate_websocket_certificate();
    };
}
}
