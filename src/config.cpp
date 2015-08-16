#include "config.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "platform.h"

namespace fs = boost::filesystem;
namespace rj = rapidjson;

std::string Config::file_ = "PicoTorrent.json";

Config::Config()
{
    default_.SetObject();
    doc_.SetObject();

    auto& alloc = default_.GetAllocator();

    default_.AddMember("py.path", ".", alloc);
    default_.AddMember("py.runtime", "python27", alloc);

    Load();
}

std::string Config::GetPyPath()
{
    return FindOrDefault("py.path")->value.GetString();
}

std::string Config::GetPyRuntimePath()
{
    return FindOrDefault("py.runtime")->value.GetString();
}

void Config::Load()
{
    if (!fs::exists(file_))
    {
        return;
    }

    try
    {
        FILE* f = fopen(file_.c_str(), "r");
        char buf[65536];

        rj::FileReadStream stream(f, buf, sizeof(buf));
        doc_.ParseStream(stream);

        if (!doc_.IsObject())
        {
            // Log error.. Reset doc
            doc_ = rj::Document();
            doc_.SetObject();
        }
    }
    catch (const std::exception& ex)
    {
    }
}

void Config::Save()
{
    rj::StringBuffer sb;
    rj::PrettyWriter<rj::StringBuffer> writer(sb);
    doc_.Accept(writer);

    std::ofstream out(file_, std::ios::binary);
    out.write(sb.GetString(), sb.GetSize());
}

rj::Value::MemberIterator Config::FindOrDefault(const char* key)
{
    rj::Value::MemberIterator it = FindOrDefault(key, doc_);

    if (it != doc_.MemberEnd())
    {
        return it;
    }

    it = FindOrDefault(key, default_);

    if (it != default_.MemberEnd())
    {
        return it;
    }

    throw std::exception("Key not found");
}

rj::Value::MemberIterator Config::FindOrDefault(const char* key, rj::Document& doc)
{
    return doc.FindMember(key);
}

