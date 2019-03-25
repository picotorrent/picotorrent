#include "gzipdecompressor.hpp"

#include <zlib.h>

#define INFLATE_BUFFER_SIZE 1024
#define INFLATE_WINDOW_BITS (15 + 32)

using pt::GZipDecompressor;

std::vector<char> GZipDecompressor::decompress(std::vector<char> const& input)
{
    std::vector<char> outBuffer(INFLATE_BUFFER_SIZE);

    z_stream strm;
    strm.avail_in  = static_cast<uInt>(input.size());
    strm.avail_out = static_cast<uInt>(outBuffer.size());
    strm.next_in   = reinterpret_cast<const Bytef*>(input.data());
    strm.next_out  = reinterpret_cast<Bytef*>(outBuffer.data());
    strm.opaque    = Z_NULL;
    strm.zalloc    = Z_NULL;
    strm.zfree     = Z_NULL;

    if (inflateInit2(&strm, INFLATE_WINDOW_BITS) != Z_OK)
    {
        return std::vector<char>();
    }

    std::vector<char> result;

    while (true)
    {
        int res = inflate(&strm, Z_NO_FLUSH);

        if (res < Z_OK)
        {
            inflateEnd(&strm);
            return std::vector<char>();
        }

        result.insert(
            result.end(),
            outBuffer.begin(),
            outBuffer.begin() + (outBuffer.size() - strm.avail_out));

        if (res == Z_STREAM_END)
        {
            break;
        }

        strm.avail_out = static_cast<uInt>(outBuffer.size());
        strm.next_out  = reinterpret_cast<Bytef*>(outBuffer.data());
    }

    inflateEnd(&strm);

    return result;
}
