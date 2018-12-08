#pragma once

#include <QString>
#include <QWidget>

namespace pt
{
    struct Tab
    {
        QString id;
        int sortOrder = -1;
        QString title;
        QWidget* widget;
    };

    class ITorrentDetailsWidget
    {
    public:
        virtual void addTab(Tab const& tab) = 0;
    };

    class ITorrentListWidget
    {
    };

    class IPluginHost
    {
    public:
        virtual ITorrentDetailsWidget* torrentDetails() = 0;
        virtual ITorrentListWidget* torrentList() = 0;
    };

    class IPlugin
    {
    public:
        virtual ~IPlugin() {}
        virtual void load(IPluginHost * const host) = 0;
    };
}

Q_DECLARE_INTERFACE(pt::IPlugin, "org.picotorrent.IPlugin")
