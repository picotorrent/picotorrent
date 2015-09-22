#include <windows.h>

class MainWindow
{
public:
    void Create(HINSTANCE hInstance)
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

        HWND hWnd = CreateWindowEx(
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
            hInstance,
            static_cast<LPVOID>(this));
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
};

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int nCmdShow)
{
    MainWindow win;
    win.Create(hInstance);

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
