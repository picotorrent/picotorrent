#include "PeerListView.hpp"

#include "../Models/Peer.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define LV_COL_IP 1
#define LV_COL_CLIENT 2
#define LV_COL_FLAGS 3
#define LV_COL_DOWNLOAD 4
#define LV_COL_UPLOAD 5

using UI::PeerListView;

PeerListView::PeerListView(HWND hWnd)
    : ListView::ListView(hWnd)
{
    AddColumn(LV_COL_IP, TRW("ip"), SX(110), ColumnType::Text);
    AddColumn(LV_COL_CLIENT, TRW("client"), SX(140), ColumnType::Text);
    AddColumn(LV_COL_FLAGS, TRW("flags"), SX(80), ColumnType::Text);
    AddColumn(LV_COL_DOWNLOAD, TRW("dl"), SX(80), ColumnType::Number);
    AddColumn(LV_COL_UPLOAD, TRW("ul"), SX(80), ColumnType::Number);
}

PeerListView::~PeerListView()
{
}

void PeerListView::Add(const Models::Peer& model)
{
    m_models.push_back(model);
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

std::wstring PeerListView::GetItemText(int columnId, int itemIndex)
{
    return L"?unknown column?";
}

void PeerListView::ShowContextMenu(POINT p, const std::vector<int>& selectedIndices)
{
    
}
