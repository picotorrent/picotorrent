#pragma once

#include <QSortFilterProxyModel>

namespace pt
{
    class ScriptedTorrentFilter;

    class TorrentSortFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        TorrentSortFilterProxyModel(QObject* parent = nullptr);
        void setScriptedFilter(ScriptedTorrentFilter* filter);

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

    private:
        ScriptedTorrentFilter* m_scriptedFilter;
    };
}
