#include "logging.h"

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <fstream>
#include <windows.h>

#include "util.h"
#include "io/directory.h"
#include "io/path.h"

namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
using namespace pico;

void Logging::Init()
{
    boost::log::add_common_attributes();

    boost::shared_ptr<boost::log::core> core = boost::log::core::get();
    boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();

    // Create 'logs' dir
    std::wstring logs = io::Path::GetLogPath();

    if (!io::Directory::Exists(logs))
    {
        io::Directory::CreateDirectories(logs);
    }

    // Get process id
    int pid = GetCurrentProcessId();
    std::wstring logFile = io::Path::Combine(logs, (L"PicoTorrent." + std::to_wstring(pid) + L".log"));
    backend->add_stream(boost::make_shared<std::ofstream>(Util::ToString(logFile)));

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
