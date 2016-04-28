#include <picotorrent/client/qr/qr_util.hpp>

#include <picotorrent/client/qr/qr_code.hpp>

using picotorrent::client::qr::qr_code;
using picotorrent::client::qr::qr_util;

int qr_util::get_lost_point(qr_code &qr)
{
    int module_count = qr.get_module_count();
    int lost_point = 0;

    // LEVEL 1
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count; col++)
        {
            int same_count = 0;
            bool dark = qr.is_dark(row, col);

            for (int r = -1; r < 2; r++)
            {
                if (row + r < 0 || module_count <= row + r)
                {
                    continue;
                }

                for (int c = -1; c < 2; c++)
                {
                    if (col + c < 0 || module_count <= col + c)
                    {
                        continue;
                    }

                    if (r == 0 || c == 0)
                    {
                        continue;
                    }

                    if (dark == qr.is_dark(row + r, col + c))
                    {
                        same_count += 1;
                    }
                }
            }

            if (same_count > 5)
            {
                lost_point += (3 + same_count - 5);
            }
        }
    }

    // LEVEL 2
    for (int row = 0; row < module_count - 1; row++)
    {
        for (int col = 0; col < module_count - 1; col++)
        {
            int count = 0;
            if (qr.is_dark(row, col)) count += 1;
            if (qr.is_dark(row + 1, col)) count += 1;
            if (qr.is_dark(row, col + 1)) count += 1;
            if (qr.is_dark(row + 1, col + 1)) count += 1;

            if (count == 0 || count == 4)
            {
                lost_point += 3;
            }
        }
    }

    // LEVEL 3
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count - 6; col++)
        {
            if (qr.is_dark(row, col)
                && !qr.is_dark(row, col + 1)
                && qr.is_dark(row, col + 2)
                && qr.is_dark(row, col + 3)
                && qr.is_dark(row, col + 4)
                && !qr.is_dark(row, col + 5)
                && qr.is_dark(row, col + 6))
            {
                lost_point += 40;
            }
        }
    }

    for (int col = 0; col < module_count; col++)
    {
        for (int row = 0; row < module_count - 6; row++)
        {
            if (qr.is_dark(row, col)
                && !qr.is_dark(row, col + 1)
                && qr.is_dark(row, col + 2)
                && qr.is_dark(row, col + 3)
                && qr.is_dark(row, col + 4)
                && !qr.is_dark(row, col + 5)
                && qr.is_dark(row, col + 6))
            {
                lost_point += 40;
            }
        }
    }

    // LEVEL 4
    int dark_count = 0;
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count; col++)
        {
            if (qr.is_dark(row, col)) dark_count += 1;
        }
    }

    int ratio = std::abs(100 * dark_count / module_count / module_count - 50) / 5;
    lost_point += ratio * 10;

    return lost_point;
}
