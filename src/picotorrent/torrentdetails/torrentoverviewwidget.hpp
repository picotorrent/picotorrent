#pragma once

#include <memory>

#include "detailstab.hpp"

namespace pt
{
    class ElidedLabel;
    struct SessionState;

    class TorrentOverviewWidget : public DetailsTab
    {
    public:
        TorrentOverviewWidget(std::shared_ptr<SessionState> state);

        virtual void clear() override;
        virtual void refresh() override;

    private:
        std::shared_ptr<SessionState> m_state;

        ElidedLabel* m_name;
        ElidedLabel* m_infoHash;
        ElidedLabel* m_savePath;
        ElidedLabel* m_pieces;
    };
}
