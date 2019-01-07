#pragma once

#include <memory>

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

        QTreeView* m_filesView;
        FileStorageItemModel* m_filesModel;
    };
}
