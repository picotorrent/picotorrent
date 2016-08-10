#include "Translator.h"

#include <msclr/marshal_cppstd.h>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/translator.hpp>

using PicoTorrent::Translator;

System::String^ Translator::Translate(System::String^ key)
{
    picotorrent::common::translator& translator = picotorrent::common::translator::instance();
    
    std::string k = msclr::interop::marshal_as<std::string>(key);
    std::wstring t = picotorrent::common::to_wstring(translator.translate(k));

    return msclr::interop::marshal_as<System::String^>(t);
}
