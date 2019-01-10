#pragma once

#include <memory>

#include <libtorrent/sha1_hash.hpp>

#include "detailstab.hpp"

class QTreeView;

namespace pt
{
    class FileStorageItemModel;
    struct SessionState;

    class TorrentFilesWidget : public DetailsTab
    {
    public:
        TorrentFilesWidget(std::shared_ptr<SessionState> state);

        virtual void clear() override;
        virtual void refresh() override;

    private:
        std::shared_ptr<SessionState> m_state;

        libtorrent::sha1_hash m_currentSelection;
        QTreeView* m_filesView;
        FileStorageItemModel* m_filesModel;
    };
}
