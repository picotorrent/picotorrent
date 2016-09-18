#pragma once

#define PICOTORRENT_API_VERSION 1

#include <functional>
#include <memory>
#include <string>

#include <windows.h>
#include <commctrl.h>

struct MenuItem
{
    std::wstring text;
    std::function<void()> onClick;
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
    virtual std::string GetCurrentVersion() = 0;
    virtual std::shared_ptr<ITranslator> GetTranslator() = 0;
    virtual void ShowTaskDialog(TASKDIALOGCONFIG* tdcfg) = 0;
};
