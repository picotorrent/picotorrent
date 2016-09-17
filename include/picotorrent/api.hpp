#pragma once

#define PICOTORRENT_API_VERSION 1

#include <string>

#include <windows.h>
#include <commctrl.h>

class IMenuItem
{
public:
    virtual std::string GetText() = 0;
    virtual void OnClick() = 0;
};

class IPicoTorrent
{
public:
    virtual void AddMenuItem(IMenuItem* menuItem) = 0;
    virtual std::string GetCurrentVersion() = 0;
    virtual void ShowDialog(TASKDIALOGCONFIG* tdcfg) = 0;
};
