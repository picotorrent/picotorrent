#pragma once

#define PICOTORRENT_API_VERSION 1

#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <picojson.hpp>

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>

#define LOG_TRACE(logger) LOG_L("TRACE", logger)
#define LOG_DEBUG(logger) LOG_L("DEBUG", logger)
#define LOG_INFO(logger)  LOG_L("INFO", logger)
#define LOG_WARN(logger)  LOG_L("WARN", logger)
#define LOG_ERROR(logger) LOG_L("ERROR", logger)

#define LOG_L(level, logger) \
    logger->OpenRecord( level, std::this_thread::get_id(), __FUNCTION__)->GetStream()

namespace libtorrent
{
    class session;
}

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

struct Torrent
{
	enum State
	{
		Unknown = -1,
		CheckingResumeData,
		Complete,
		Downloading,
		DownloadingChecking,
		DownloadingForced,
		DownloadingMetadata,
		DownloadingPaused,
		DownloadingQueued,
		DownloadingStalled,
		Error,
		Uploading,
		UploadingChecking,
		UploadingForced,
		UploadingPaused,
		UploadingQueued,
		UploadingStalled
	};

	bool operator==(const Torrent& other)
    {
        return infoHash == other.infoHash;
    }

    bool operator!=(const Torrent& other)
    {
        return !(*this == other);
    }

    std::string infoHash;
    std::string name;
    int queuePosition;
    int64_t totalSize;
    int64_t totalWanted;
    State state;
    float progress;
    std::chrono::seconds eta;
    int downloadRate;
    int uploadRate;
    int seedsConnected;
    int seedsTotal;
    int peersConnected;
    int peersTotal;
    float shareRatio;
    bool isPaused;
    std::string savePath;
    int64_t downloadedBytes;
    int64_t uploadedBytes;
    int piecesHave;
    int pieceLength;
    int piecesCount;
    std::string errorMessage;
};

struct ITorrentEventSink
{
    virtual ~ITorrentEventSink() {};

    virtual void OnTorrentAdded(Torrent torrent) {};
    virtual void OnTorrentError(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentFinished(Torrent torrent) {};
    virtual void OnTorrentMoved(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentPaused(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentResumed(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentRemoved(std::string const& infoHash) {};
    virtual void OnTorrentUpdated(std::vector<Torrent> torrents) {};
};

class IPicoTorrent
{
public:
    virtual void AddMenuItem(MenuItem const& menuItem) = 0;
    virtual std::shared_ptr<picojson::object> GetConfiguration() = 0;
    virtual std::string GetCurrentVersion() = 0;
    virtual std::shared_ptr<ILogger> GetLogger() = 0;
    virtual std::shared_ptr<libtorrent::session> GetSession() = 0;
    virtual std::shared_ptr<ITranslator> GetTranslator() = 0;
    virtual void RegisterEventSink(std::shared_ptr<ITorrentEventSink> sink) = 0;
    virtual std::unique_ptr<TaskDialogResult> ShowTaskDialog(TASKDIALOGCONFIG* tdcfg) = 0;
};
