#include <picotorrent/client/ui/dialogs/remote_qr_dialog.hpp>

#include <picotorrent/core/version_info.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/qr/bit_buffer.hpp>
#include <picotorrent/client/qr/qr_code.hpp>
#include <picotorrent/client/qr/qr_data.hpp>
#include <picotorrent/client/qr/qr_8bit_byte.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>

#include <sstream>

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::core::version_info;
using picotorrent::client::qr::bit_buffer;
using picotorrent::client::qr::qr_8bit_byte;
using picotorrent::client::qr::qr_code;
using picotorrent::client::qr::qr_data;
using picotorrent::client::ui::dialogs::remote_qr_dialog;
using picotorrent::client::ui::scaler;

class qr_control
{
public:
    qr_control()
    {
        WNDCLASS wc = { 0 };

        wc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &qr_control::QrWndProc;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = TEXT("PicoTorrent/QR");
        RegisterClass(&wc);
    }

private:
    static void PaintQrCode(HWND hWnd)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rect;

        qr_code qr;
        qr.set_error_correct_level(2);
        qr.add_data("SOMETHING");
        qr.make();

        GetClientRect(hWnd, &rect);
        hdc = BeginPaint(hWnd, &ps);
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);


        HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, hWhite);

        rect.bottom -= 10;
        rect.left += 10;
        rect.right -= 10;
        rect.top += 10;

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        int module_count = qr.get_module_count();
        int width_per_square = width / module_count;
        int height_per_square = height / module_count;

        for (int row = 0; row < module_count; row++)
        {
            for (int col = 0; col < module_count; col++)
            {
                if (!qr.is_dark(row, col)) { continue; }

                RECT r = { 0 };
                r.left = rect.left + (col * width_per_square);
                r.top = rect.top + (row * height_per_square);
                r.right = r.left + width_per_square;
                r.bottom = r.top + height_per_square;

                FillRect(hdc, &r, hBlack);
            }
        }

        DeleteObject(hWhite);
        DeleteObject(hBlack);

        EndPaint(hWnd, &ps);
    }

    static LRESULT CALLBACK QrWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_PAINT:
            PaintQrCode(hWnd);
            return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
};

remote_qr_dialog::remote_qr_dialog()
    : dialog_base(IDD_REMOTE_QR)
{
    static qr_control init;
}

remote_qr_dialog::~remote_qr_dialog()
{
}

BOOL remote_qr_dialog::on_command(int id, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IDCANCEL:
    case IDOK:
        EndDialog(handle(), wParam);
        return TRUE;
    }

    return FALSE;
}

BOOL remote_qr_dialog::on_init_dialog()
{
    // TODO, this is a test which doesn't work.
    std::shared_ptr<qr_8bit_byte> data = std::make_shared<qr_8bit_byte>("ab01");
    std::vector<std::shared_ptr<qr_data>> dlist({ data });

    std::vector<char> act = qr_code::create_data(1, 2, dlist);
    std::vector<char> exp = { 64,70,22,35,3,16,-20,17,-20,91,-25,80,48,-87,54,40,-83,84,-117,33,87,54,-57,50,-84,49 };

    return TRUE;
}

BOOL remote_qr_dialog::on_notify(LPARAM lParam)
{
    return FALSE;
}
