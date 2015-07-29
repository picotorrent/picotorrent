#ifndef _PT_UI_TORRENTDETAILSFRAME_H
#define _PT_UI_TORRENTDETAILSFRAME_H

#include "../stdafx.h"

class CTorrentDetailsFrame
    : public CFrameWindowImpl<CTorrentDetailsFrame>
{
public:
    CTorrentDetailsFrame();
    ~CTorrentDetailsFrame();

    DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_TORRENTDETAILSFRAME, 0, -1);

    BEGIN_MSG_MAP(CTorrentDetailsFrame)
        CHAIN_MSG_MAP(CFrameWindowImpl<CTorrentDetailsFrame>)
    END_MSG_MAP()
};

#endif
