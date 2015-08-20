#ifndef _PT_CONTROLLERS_ADDTORRENTCONTROLLER_H
#define _PT_CONTROLLERS_ADDTORRENTCONTROLLER_H

#include <string>

class AddTorrentController
{
public:
    virtual void PerformAdd() = 0;

    virtual std::string GetName(int index) = 0;

    virtual std::string GetComment(int index) = 0;

    virtual std::string GetCreationDate(int index) = 0;

    virtual std::string GetCreator(int index) = 0;

    virtual std::string GetSavePath(int index) = 0;

    virtual std::string GetSize(int index) = 0;

    virtual int GetCount() = 0;

    virtual int GetFileCount(int index) = 0;

    virtual std::string GetFileName(int torrentIndex, int fileIndex) = 0;

    virtual std::string GetFileSize(int torrentIndex, int fileIndex) = 0;

    virtual int GetFilePriority(int torrentIndex, int fileIndex) = 0;

    virtual void SetSavePath(int index, std::string savePath) = 0;

    virtual void SetFileName(int torrentIndex, int fileIndex, std::string name) = 0;

    virtual void SetFilePriority(int torrentIndex, int fileIndex, int priority) = 0;

    virtual std::string ToFriendlyPriority(int priority) = 0;
};

#endif
