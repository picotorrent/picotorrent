#pragma once

#include <memory>

#include "detailstab.hpp"

namespace pt
{
    struct SessionState;

    class TorrentTrackersWidget : public DetailsTab
    {
    public:
        TorrentTrackersWidget(std::shared_ptr<SessionState> state);

        virtual void clear() override;
        virtual void refresh() override;

    private:
        std::shared_ptr<SessionState> m_state;
    };
}
