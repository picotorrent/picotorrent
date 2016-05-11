#include <picotorrent/client/ui/dialogs/remote_qr_dialog.hpp>

#include <picotorrent/core/version_info.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/qr/bit_buffer.hpp>
#include <picotorrent/client/qr/qr_8bit_byte.hpp>
#include <picotorrent/client/qr/qr_code.hpp>
#include <picotorrent/client/qr/qr_data.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>

#include <sstream>

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>
#include <windowsx.h>

#define QR_SET WM_USER+1

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
    static void PaintQrCode(HWND hWnd, qr_code *qr)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rect;

        GetClientRect(hWnd, &rect);
        hdc = BeginPaint(hWnd, &ps);
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);

        HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, hWhite);

        int padding = scaler::x(10);
        rect.top += padding; rect.bottom -= padding;
        rect.left += padding; rect.right -= padding;

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        int module_count = qr->get_module_count();
        int width_per_square = width / module_count;
        int height_per_square = height / module_count;

        int w = width_per_square * module_count;
        int woffset = (width - w) / 2;
        rect.left += woffset; rect.right -= woffset;

        int h = height_per_square * module_count;
        int hoffset = (height - h) / 2;
        rect.top += hoffset; rect.bottom -= hoffset;

        for (int row = 0; row < module_count; row++)
        {
            for (int col = 0; col < module_count; col++)
            {
                if (!qr->is_dark(row, col)) { continue; }

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
        {
            qr_code *qr = (qr_code*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            PaintQrCode(hWnd, qr);
            return 0;
        }
        case QR_SET:
        {
            qr_code *qr = (qr_code*)lParam;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)qr);
            break;
        }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
};

remote_qr_dialog::remote_qr_dialog()
    : dialog_base(IDD_REMOTE_QR),
    qr_(std::make_shared<qr_code>())
{
    static qr_control init;

    qr_->set_error_correct_level(1);
    qr_->set_type_number(7);
}

remote_qr_dialog::~remote_qr_dialog()
{
}

void remote_qr_dialog::set_data(const std::string &data)
{
    // qr_->clear_data();
    qr_->add_data(data);
    qr_->make();
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
    HWND qr = GetDlgItem(handle(), ID_QR_CONTROL);
    SendMessage(qr, QR_SET, NULL, (LPARAM)qr_.get());
    return TRUE;
}

BOOL remote_qr_dialog::on_notify(LPARAM lParam)
{
    return FALSE;
}
