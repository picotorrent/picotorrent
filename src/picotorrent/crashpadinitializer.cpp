#include "crashpadinitializer.hpp"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Windows.h>
#include <CommCtrl.h>

#include <boost/log/trivial.hpp>
#include <client/crash_report_database.h>
#include <client/settings.h>
#include <client/crashpad_client.h>

#include "buildinfo.hpp"
#include "core/environment.hpp"
#include "core/utils.hpp"

namespace fs = std::filesystem;
using pt::CrashpadInitializer;

void CrashpadInitializer::Initialize(std::shared_ptr<pt::Core::Environment> env)
{
    auto databasePath = env->GetApplicationDataPath() / "Crashpad" / "db";
    auto handlerPath = env->GetApplicationPath() / "crashpad_handler.exe";

    BOOST_LOG_TRIVIAL(info) << "Initializing Crashpad (path: " << handlerPath << ", url: " << env->GetCrashpadReportUrl() << ")";

    if (!fs::exists(handlerPath))
    {
        BOOST_LOG_TRIVIAL(warning) << "Could not find crashpad_handler.exe, skipping initialization...";
        return;
    }

    std::error_code ec;
    bool exists = fs::exists(databasePath, ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to check if database path exists: " << ec;
        return;
    }

    if (!exists)
    {
        fs::create_directories(databasePath, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create Crashpad database directories: " << ec;
            return;
        }
    }

    auto database = crashpad::CrashReportDatabase::Initialize(base::FilePath(databasePath.wstring()));

    if (database == nullptr || database->GetSettings() == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialize Crashpad database from path: " << databasePath;
        return;
    }

    if (!database->GetSettings()->SetUploadsEnabled(true))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to set uploads in Crashpad database";
        return;
    }

    crashpad::CrashpadClient client;
    if (!client.StartHandlerForBacktrace(base::FilePath(handlerPath.wstring()),
        base::FilePath(databasePath.wstring()),
        base::FilePath(databasePath.wstring()),
        env->GetCrashpadReportUrl(),
        // annotations
        {
            { "branch",    pt::BuildInfo::branch() },
            { "commitish", pt::BuildInfo::commitish() },
            { "version",   pt::BuildInfo::semver() }
        },
        {
#if _DEBUG
            "--no-rate-limit"
#endif
        },
        // attachments
        {
            { "log_file", env->GetLogFilePath().string() }
        },
        true,
        true))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to start Crashpad";
        return;
    }

    if (!client.WaitForHandlerStart(10000))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to wait for Crashpad handler start";
        return;
    }

    BOOST_LOG_TRIVIAL(info) << "Crashpad handler started";
}
