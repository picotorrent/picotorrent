#include "applicationoptions.hpp"

#include <wx/filename.h>

#include "picojson.hpp"

using pt::ApplicationOptions;

std::shared_ptr<ApplicationOptions> ApplicationOptions::JsonDecode(wxString json)
{
    picojson::value val;
    std::string err = picojson::parse(val, json.ToStdString());

    if (!err.empty())
    {
        return nullptr;
    }
    
    picojson::object obj = val.get<picojson::object>();
    picojson::array files = obj.at("files").get<picojson::array>();
    picojson::array magnets = obj.at("magnet_links").get<picojson::array>();

    auto opts = std::make_shared<ApplicationOptions>();
    
    for (auto item : files)
    {
        opts->files.push_back(item.get<std::string>());
    }

    for (auto item : magnets)
    {
        opts->magnet_links.push_back(item.get<std::string>());
    }

    return opts;
}

wxString ApplicationOptions::JsonEncode(std::shared_ptr<ApplicationOptions> opts)
{
    picojson::array files;
    picojson::array magnets;

    for (auto item : opts->files)
    {
        wxFileName fn(item);
        fn.MakeAbsolute();

        files.push_back(picojson::value(fn.GetFullPath().ToStdString()));
    }

    for (auto item : opts->magnet_links)
    {
        magnets.push_back(picojson::value(item.ToStdString()));
    }

    picojson::object obj;
    obj.insert({ "files", picojson::value(files) });
    obj.insert({ "magnet_links", picojson::value(magnets) });

    return wxString::FromUTF8(picojson::value(obj).serialize());
}
