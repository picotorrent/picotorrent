#include "mainwindow.h"

#include "scaler.h"
#include "torrentlistview.h"

using pico::MainWindow;

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::Create()
{
    pico::Scaler scaler;

    WNDCLASSEX wnd = { 0 };
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbWndExtra = sizeof(MainWindow*);
    wnd.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.lpfnWndProc = &MainWindow::WndProcProxy;
    wnd.lpszClassName = TEXT("PicoTorrent/MainWindow");
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClassEx(&wnd);

    hWnd_ = CreateWindowEx(
        0,
        wnd.lpszClassName,
        TEXT("PicoTorrent"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        scaler.GetX(600),
        scaler.GetY(200),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        static_cast<LPVOID>(this));

    torrentsView_ = std::make_unique<pico::TorrentListView>(hWnd_);
    torrentsView_->Create();

    // Set up torrent list view (columns etc)
    torrentsView_->AddColumn(TEXT("Name"), scaler.GetX(150), LVCFMT_LEFT);
    torrentsView_->AddColumn(TEXT("#"), scaler.GetX(30), LVCFMT_RIGHT);
    torrentsView_->AddColumn(TEXT("Size"), scaler.GetX(80), LVCFMT_RIGHT);
    torrentsView_->AddColumn(TEXT("Status"), scaler.GetX(100), LVCFMT_LEFT);
    torrentsView_->AddColumn(TEXT("DL"), scaler.GetX(80), LVCFMT_RIGHT);
    torrentsView_->AddColumn(TEXT("UL"), scaler.GetX(80), LVCFMT_RIGHT);
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_WINDOWPOSCHANGED:
    {
        if (!torrentsView_)
        {
            return FALSE;
        }

        RECT rc;
        GetClientRect(hWnd, &rc);

        // Set new width and height for the torrent list view.
        torrentsView_->Resize(rc.right - rc.left, rc.bottom - rc.top);

        return TRUE;
    }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

LRESULT MainWindow::WndProcProxy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        LPVOID pThis = pCreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));
    }

    MainWindow* pWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, 0));
    return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
}
