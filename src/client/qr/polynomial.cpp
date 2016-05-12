#include <picotorrent/client/qr/polynomial.hpp>

#include <picotorrent/client/qr/qr_math.hpp>

using picotorrent::client::qr::polynomial;
using picotorrent::client::qr::qr_math;

polynomial::polynomial(const std::vector<int> &num)
    : polynomial(num, 0)
{
}

polynomial::polynomial(const std::vector<int> &num, int shift)
{
    int offset = 0;

    while (offset < (int)num.size() && num[offset] == 0)
    {
        offset++;
    }

    //                  src  srcPos  dest     destPos   length
    // System.arraycopy(num, offset, this.num, 0, num.length - offset);

    num_.resize(num.size() - offset + shift);
    int idx = 0;

    for (size_t i = offset; i < num.size() - offset; i++)
    {
        num_[idx] = num[i];
        idx += 1;
    }
}

int polynomial::get(int index) const
{
    return num_[index];
}

int polynomial::get_length() const
{
    return (int)num_.size();
}

polynomial polynomial::mod(const polynomial &other)
{
    if (get_length() - other.get_length() < 0)
    {
        return *this;
    }

    int ratio = qr_math::glog(get(0)) - qr_math::glog(other.get(0));

    std::vector<int> num(get_length());
    for (int i = 0; i < get_length(); i++)
    {
        num[i] = get(i);
    }

    for (int i = 0; i < other.get_length(); i++)
    {
        num[i] ^= qr_math::gexp(qr_math::glog(other.get(i)) + ratio);
    }

    return polynomial(num).mod(other);
}

polynomial polynomial::multiply(const polynomial &other)
{
    std::vector<int> num(get_length() + other.get_length() - 1);

    for (int i = 0; i < get_length(); i++)
    {
        for (int j = 0; j < other.get_length(); j++)
        {
            num[i + j] ^= qr_math::gexp(qr_math::glog(get(i)) + qr_math::glog(other.get(j)));
        }
    }

    return polynomial(num);
}
