#include "PeerListView.hpp"

#include "../Models/Peer.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#include <strsafe.h>

#define LV_COL_IP 0
#define LV_COL_CLIENT 1
#define LV_COL_FLAGS 2
#define LV_COL_DOWNLOAD 3
#define LV_COL_UPLOAD 4
#define LV_COL_PROGRESS 5

using UI::PeerListView;

PeerListView::PeerListView(HWND hWnd)
    : ListView::ListView(hWnd)
{
    AddColumn(
        std::wstring(TRW("ip")),
        LV_COL_IP,
        SX(110),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("client")),
        LV_COL_CLIENT,
        SX(140),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("flags")),
        LV_COL_FLAGS,
        SX(80),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("dl")),
        LV_COL_DOWNLOAD,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("ul")),
        LV_COL_UPLOAD,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("progress")),
        LV_COL_PROGRESS,
        SX(100),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT,
        true);
}

PeerListView::~PeerListView()
{
}

void PeerListView::Add(const Models::Peer& model)
{
    m_models.push_back(model);
}

void PeerListView::Remove(const Models::Peer& model)
{
    auto f = std::find(m_models.begin(), m_models.end(), model);
    if (f != m_models.end()) { m_models.erase(f); }
}

void PeerListView::Update(const Models::Peer& model)
{
    auto f = std::find(m_models.begin(), m_models.end(), model);

    if (f != m_models.end())
    {
        auto index = std::distance(m_models.begin(), f);
        m_models.at(index) = model;
    }
}

float PeerListView::GetItemProgress(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_PROGRESS:
        return m_models.at(itemIndex).progress;
    }

    return -1;
}

std::wstring PeerListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_IP:
        return m_models.at(itemIndex).endpoint;
    case LV_COL_CLIENT:
        return m_models.at(itemIndex).client;
    case LV_COL_FLAGS:
        return m_models.at(itemIndex).flags;
    case LV_COL_DOWNLOAD:
    case LV_COL_UPLOAD:
    {
        int rate = columnId == LV_COL_DOWNLOAD
            ? m_models.at(itemIndex).downloadRate
            : m_models.at(itemIndex).uploadRate;

        if (rate < 1024)
        {
            return TEXT("-");
        }

        TCHAR result[100];
        StrFormatByteSize64(rate, result, ARRAYSIZE(result));
        StringCchPrintf(result, ARRAYSIZE(result), L"%s/s", result);
        return result;
    }
    }

    return L"?unknown column?";
}

void PeerListView::ShowContextMenu(POINT p, const std::vector<int>& selectedIndices)
{
    
}
