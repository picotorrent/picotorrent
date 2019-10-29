#include "crashpad.hpp"

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

#include "buildinfo.hpp"
#include "core/environment.hpp"
#include "loguru.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;
using pt::CrashpadInitializer;

void CrashpadInitializer::init(std::shared_ptr<pt::Environment> env)
{
    LOG_F(INFO, "Initializing Crashpad");

    auto databasePath = env->getApplicationDataPath() / "Crashpad" / "db";
    auto handlerPath = env->getApplicationPath() / "crashpad_handler.exe";

    if (!fs::exists(handlerPath))
    {
        LOG_F(WARNING, "Could not find crashpad_handler.exe, skipping initialization...");
        return;
    }

    if (!fs::exists(databasePath))
    {
        std::error_code ec;
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

    auto consentFile = databasePath / "consent";
    bool enabled = false;

    if (fs::exists(consentFile))
    {
        std::ifstream input(consentFile, std::ios::binary);

        char consentFlag[1] = { '0' };
        input.read(consentFlag, 1);

        if (consentFlag[0] == '1')
        {
            enabled = true;
        }
    }

    LOG_F(INFO, "Crashpad consent %s", enabled ? "given" : "not given");

    // check if file CrashpadReporting exists and contains a 1. Then enable
    if (!database->GetSettings()->SetUploadsEnabled(enabled))
    {
        LOG_F(ERROR, "Failed to set uploads in Crashpad database");
        return;
    }

    crashpad::CrashpadClient client;
    if (!client.StartHandler(base::FilePath(handlerPath.wstring()),
        base::FilePath(databasePath.wstring()),
        base::FilePath(databasePath.wstring()),
        "https://api.picotorrent.org/crashpad",
        {
            { "branch", pt::BuildInfo::branch().toStdString() },
            { "commitish", pt::BuildInfo::commitish().toStdString() },
            { "version", pt::BuildInfo::version().toStdString() }
        },
        {
#if _DEBUG
            "--no-rate-limit"
#endif
        },
        true,
        true))
    {
        LOG_F(ERROR, "Failed to start Crashpad");
        return;
    }

    if (!client.WaitForHandlerStart(INFINITE))
    {
        LOG_F(ERROR, "Failed to wait for Crashpad handler start");
    }
}
