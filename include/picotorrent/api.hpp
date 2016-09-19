#pragma once

#define PICOTORRENT_API_VERSION 1

#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <picojson.hpp>

#include <windows.h>
#include <commctrl.h>

#define LOG_TRACE(logger) LOG_L("TRACE", logger)
#define LOG_DEBUG(logger) LOG_L("DEBUG", logger)
#define LOG_INFO(logger)  LOG_L("INFO", logger)
#define LOG_WARN(logger)  LOG_L("WARN", logger)
#define LOG_ERROR(logger) LOG_L("ERROR", logger)

#define LOG_L(level, logger) \
    logger->OpenRecord( level, std::this_thread::get_id(), __FUNCTION__)->GetStream()

struct MenuItem
{
    std::wstring text;
    std::function<void()> onClick;
};

struct TaskDialogResult
{
    int button;
    int radioButton;
    bool verificationChecked;
};

class ILogger
{
public:
    struct ILogRecord
    {
        virtual ~ILogRecord() {};
        virtual std::ostream& GetStream() = 0;
    };

    virtual std::unique_ptr<ILogRecord> OpenRecord(const char* level, std::thread::id threadId, const char* functionName) = 0;
};

class ITranslator
{
public:
    virtual std::wstring Translate(const std::string& key) = 0;
};

class IPicoTorrent
{
public:
    virtual void AddMenuItem(MenuItem const& menuItem) = 0;
    virtual std::shared_ptr<picojson::object> GetConfiguration() = 0;
    virtual std::string GetCurrentVersion() = 0;
    virtual std::shared_ptr<ILogger> GetLogger() = 0;
    virtual std::shared_ptr<ITranslator> GetTranslator() = 0;
    virtual std::unique_ptr<TaskDialogResult> ShowTaskDialog(TASKDIALOGCONFIG* tdcfg) = 0;
};
