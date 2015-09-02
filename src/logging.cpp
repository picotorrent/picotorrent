#include "logging.h"

#include <boost/filesystem.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <windows.h>

#include "path.h"

namespace expr = boost::log::expressions;
namespace fs = boost::filesystem;
namespace sinks = boost::log::sinks;
using namespace pico;

void Logging::Init()
{
    boost::log::add_common_attributes();

    boost::shared_ptr<boost::log::core> core = boost::log::core::get();
    boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();

    // Create 'logs' dir
    fs::path logs = Path::GetLogPath();

    if (!fs::exists(logs))
    {
        fs::create_directories(logs);
    }

    // Get process id
    int pid = GetCurrentProcessId();
    fs::path logFile = logs / ("PicoTorrent." + std::to_string(pid) + ".log");
    backend->add_stream(boost::make_shared<std::ofstream>(logFile.string()));

    // Enable auto-flushing
    backend->auto_flush(true);

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(backend));

    sink->set_formatter
        (
            expr::stream
            << "(" << std::hex << std::setw(8) << std::setfill('0') << expr::attr<unsigned int>("LineID") << ") "
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f") << ": "
            << "<" << boost::log::trivial::severity << "> "
            << expr::smessage
            );

    core->add_sink(sink);
}
