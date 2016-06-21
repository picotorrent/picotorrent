#include "Configuration.h"

#include <msclr/marshal_cppstd.h>

#include <picojson.hpp>
#include <picotorrent/common/config/configuration.hpp>

namespace pj = picojson;
using picotorrent::common::config::configuration;

using PicoTorrent::Configuration;

Configuration::Configuration()
{
    _serializer = gcnew System::Web::Script::Serialization::JavaScriptSerializer();
}

generic<typename T>
T Configuration::Get(System::String^ key)
{
    configuration& cfg = configuration::instance();

    std::string k = msclr::interop::marshal_as<std::string>(key);
    pj::value v = cfg.get(k);

    if (v.is<pj::null>()) { return T(); }

    System::String^ json = msclr::interop::marshal_as<System::String^>(v.serialize());
    return _serializer->Deserialize<T>(json);
}

generic<typename T>
void Configuration::Set(System::String^ key, T value)
{
    configuration& cfg = configuration::instance();

    System::String^ json = _serializer->Serialize(value);

    std::string k = msclr::interop::marshal_as<std::string>(key);
    std::string j = msclr::interop::marshal_as<std::string>(json);

    pj::value v;
    std::string err = pj::parse(v, j);

    if (!err.empty())
    {
        throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(err));
    }

    cfg.set(k, v);
}
