#include <windows.h>
#include <commctrl.h>

// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' \
                        processorArchitecture='*' \
                        publicKeyToken='6595b64144ccf1df' \
                        language='*'\"")

class MainWindow
{
public:
    MainWindow(HINSTANCE hInstance)
        : hInstance_(hInstance)
    {
        // Initialize common controls
        INITCOMMONCONTROLSEX icex = { 0 };
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);

        if (!InitCommonControlsEx(&icex))
        {
            DWORD err = GetLastError();
        }
    }

    void Create()
    {
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
            WS_EX_ACCEPTFILES,
            wnd.lpszClassName,
            TEXT("PicoTorrent"),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            400,
            300,
            NULL,
            NULL,
            hInstance_,
            static_cast<LPVOID>(this));

        RECT rcClient;
        GetClientRect(hWnd_, &rcClient);

        HWND hWnd_ListView = CreateWindowEx(
            0,
            WC_LISTVIEW,
            0,
            WS_CHILD | LVS_REPORT | WS_VISIBLE,
            0,
            0,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hWnd_,
            NULL,
            hInstance_,
            NULL);

        if (hWnd_ListView == NULL)
        {
            DWORD err = GetLastError();
        }

        // Add columns
        LVCOLUMN col;
        col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        col.iSubItem = 0;
        col.pszText = TEXT("Name");
        col.cx = 100;
        col.fmt = LVCFMT_LEFT;

        if (ListView_InsertColumn(hWnd_ListView, 0, &col) == -1)
        {
            DWORD err = GetLastError();
        }
    }

private:
    LRESULT CALLBACK WndProc(
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    static LRESULT CALLBACK WndProcProxy(
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
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

    HINSTANCE hInstance_;
    HWND hWnd_;
};

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int nCmdShow)
{
    MainWindow win(hInstance);
    win.Create();

    MSG msg;
    BOOL ret;

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (ret == -1)
        {
            // TODO(handle error)
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}
