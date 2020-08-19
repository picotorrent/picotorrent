#include "crashpadinitializer.hpp"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Windows.h>
#include <CommCtrl.h>

#include <client/crash_report_database.h>
#include <client/settings.h>
#include <client/crashpad_client.h>
#include <loguru.hpp>

#include "buildinfo.hpp"
#include "core/environment.hpp"
#include "core/utils.hpp"

namespace fs = std::filesystem;
using pt::CrashpadInitializer;

void CrashpadInitializer::Initialize(std::shared_ptr<pt::Core::Environment> env)
{
    auto databasePath = env->GetApplicationDataPath() / "Crashpad" / "db";
    auto handlerPath = env->GetApplicationPath() / "crashpad_handler.exe";

    LOG_F(INFO, "Initializing Crashpad (path: %s, url: %s)",
        handlerPath.string().c_str(),
        env->GetCrashpadReportUrl().c_str());

    if (!fs::exists(handlerPath))
    {
        LOG_F(WARNING, "Could not find crashpad_handler.exe, skipping initialization...");
        return;
    }

    std::error_code ec;
    bool exists = fs::exists(databasePath, ec);

    if (ec)
    {
        LOG_F(ERROR, "Failed to check if database path exists: %s", ec.message().c_str());
        return;
    }

    if (!exists)
    {
        fs::create_directories(databasePath, ec);

        if (ec)
        {
            LOG_F(ERROR, "Failed to create Crashpad database directories: %s", ec.message().c_str());
            return;
        }
    }

    auto database = crashpad::CrashReportDatabase::Initialize(base::FilePath(databasePath.wstring()));

    if (database == nullptr || database->GetSettings() == nullptr)
    {
        LOG_F(ERROR, "Failed to initialize Crashpad database from path: %s", databasePath.string().c_str());
        return;
    }

    if (!database->GetSettings()->SetUploadsEnabled(true))
    {
        LOG_F(ERROR, "Failed to set uploads in Crashpad database");
        return;
    }

    crashpad::CrashpadClient client;
    if (!client.StartHandlerForBacktrace(base::FilePath(handlerPath.wstring()),
        base::FilePath(databasePath.wstring()),
        base::FilePath(databasePath.wstring()),
        env->GetCrashpadReportUrl(),
        // annotations
        {
            { "branch", pt::BuildInfo::branch() },
            { "commitish", pt::BuildInfo::commitish() },
            { "version", pt::BuildInfo::version() }
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
        LOG_F(ERROR, "Failed to start Crashpad");
        return;
    }

    if (!client.WaitForHandlerStart(10000))
    {
        LOG_F(ERROR, "Failed to wait for Crashpad handler start");
        return;
    }

    LOG_F(INFO, "Crashpad handler started");
}
