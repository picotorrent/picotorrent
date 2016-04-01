#include <picotorrent/server/application.hpp>
using picotorrent::server::application;

int main(int argc, char *argv[])
{
    application app;
    return app.run();
}
