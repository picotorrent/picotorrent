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

    rj::Value iface(rj::kArrayType);
    iface.PushBack("0.0.0.0", alloc);
    iface.PushBack(6881, alloc);

    default_.AddMember("bt.net.interface", iface, alloc);
    default_.AddMember("savePath", Platform::GetDownloadsPath(), alloc);

    Load();
}

Config::~Config()
{
    Save();
}

std::string Config::GetDefaultSavePath()
{
    return FindOrDefault("savePath")->value.GetString();
}

std::pair<std::string, int> Config::GetListenInterface()
{
    rj::Value::MemberIterator it = FindOrDefault("bt.net.interface");

    return std::make_pair(
        it->value[0].GetString(),
        it->value[1].GetInt());
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

