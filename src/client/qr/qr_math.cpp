#include <picotorrent/client/qr/qr_math.hpp>

#include <vector>

using picotorrent::client::qr::qr_math;

struct tables
{
    tables()
    {
        exp = std::vector<int>(256);

        for (int i = 0; i < 8; i++)
        {
            exp[i] = 1 << i;
        }

        for (int i = 8; i < 256; i++)
        {
            exp[i] = exp[i - 4]
                ^ exp[i - 5]
                ^ exp[i - 6]
                ^ exp[i - 8];
        }

        log = std::vector<int>(256);
        for (int i = 0; i < 255; i++)
        {
            log[exp[i]] = i;
        }
    }

    std::vector<int> exp;
    std::vector<int> log;
};

static tables table;

int qr_math::glog(int n)
{
    if (n < 1)
    {
        throw std::exception("arithmetic error");
    }

    return table.log[n];
}

int qr_math::gexp(int n)
{
    while (n < 0)
    {
        n += 255;
    }

    while (n >= 256)
    {
        n -= 255;
    }

    return table.exp[n];
}
