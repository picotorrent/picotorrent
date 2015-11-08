#pragma once

#include <string>
#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>

#define COL_NAME 0
#define COL_QUEUE_POSITION 1
#define COL_SIZE 2
#define COL_STATE 3
#define COL_PROGRESS 4
#define COL_DOWNLOAD_RATE 5
#define COL_UPLOAD_RATE 6

namespace picotorrent
{
namespace ui
{
    class torrent_list_item;

    class torrent_list_view
    {
    public:
        enum sort_order
        {
            none,
            asc,
            desc
        };

        torrent_list_view(HWND hParent);
        ~torrent_list_view();

        void add_column(LPCTSTR text, int width, int format);
        void create();
        int get_column_count();
        sort_order get_column_sort(int colIndex);
        HWND handle();
        LRESULT on_custom_draw(LPNMLVCUSTOMDRAW lpCustomDraw, const torrent_list_item&);
        void on_getdispinfo(NMLVDISPINFO*, const torrent_list_item&);
        void refresh();
        void resize(int width, int height);
        void set_column_sort(int colIndex, sort_order order);
        void set_item_count(int count);

    private:
        HWND hWnd_;
        HWND hWnd_parent_;
        HWND hWnd_progress_;
        HTHEME hTheme_progress_;
    };
}
}
