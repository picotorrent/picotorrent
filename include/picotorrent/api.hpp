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

class FilePath;

class DirectoryPath
{
public:
    DirectoryPath(wchar_t const* path)
        : m_path(path)
    {
    }

    DirectoryPath(std::wstring const& path)
        : m_path(path)
    {
    }

    operator LPCTSTR() const
    {
        return m_path.c_str();
    }

private:
    std::wstring m_path;
};

class FilePath
{
public:
    FilePath(wchar_t const* path)
        : m_path(path)
    {
    }

    FilePath(std::wstring const& path)
        : m_path(path)
    {
    }

    operator LPCTSTR() const
    {
        return m_path.c_str();
    }

private:
    std::wstring m_path;
};

static FilePath operator+(DirectoryPath const& dir, FilePath const& file)
{
    TCHAR t[MAX_PATH];
    PathCombine(t, dir, file);
    return FilePath(t);
}

class IFileSystemInfo
{
public:
    virtual bool Exists() = 0;
};

typedef std::vector<char> ByteBuffer;

class IFile;
typedef std::shared_ptr<IFile> FileHandle;

class IFile : public IFileSystemInfo
{
public:
    virtual FileHandle ChangeExtension(std::wstring const& extension) = 0;
    virtual void Delete() = 0;
    virtual size_t Length() = 0;
    virtual std::vector<char> ReadAllBytes(std::error_code& ec) = 0;
};

typedef std::vector<FileHandle> FileHandleCollection;

class IDirectory : public IFileSystemInfo
{
public:
    virtual FileHandleCollection GetFiles(std::wstring const& filter) = 0;
};

typedef std::shared_ptr<IDirectory> DirectoryHandle;

class IFileSystem
{
public:
    virtual DirectoryHandle GetDirectory(DirectoryPath const& path) = 0;
    virtual FileHandle GetFile(FilePath const& path) = 0;
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
    std::string infoHash;
    std::string name;
    int queuePosition;
    int64_t size;
    float progress;
    // state
    //eta
    int downloadRate;
    int uploadRate;
    int seedsConnected;
    int seedsTotal;
    int nonseedsConnected;
    int nonseedsTotal;
};

struct ITorrentEventSink
{
    virtual void OnTorrentAdded(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentError(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentFinished(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentMoved(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentPaused(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentResumed(std::shared_ptr<Torrent> torrent) {};
    virtual void OnTorrentRemoved(std::string const& infoHash) {};
    virtual void OnTorrentUpdated(std::vector<std::shared_ptr<Torrent>> torrents) {};
};

class IPicoTorrent
{
public:
    virtual void AddMenuItem(MenuItem const& menuItem) = 0;
    virtual std::shared_ptr<picojson::object> GetConfiguration() = 0;
    virtual std::string GetCurrentVersion() = 0;
    virtual std::shared_ptr<IFileSystem> GetFileSystem() = 0;
    virtual std::shared_ptr<ILogger> GetLogger() = 0;
    virtual std::shared_ptr<libtorrent::session> GetSession() = 0;
    virtual std::shared_ptr<ITranslator> GetTranslator() = 0;
    virtual void RegisterEventSink(std::shared_ptr<ITorrentEventSink> sink) = 0;
    virtual std::unique_ptr<TaskDialogResult> ShowTaskDialog(TASKDIALOGCONFIG* tdcfg) = 0;
};
