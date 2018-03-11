#include "config.hpp"

#include "preset.hpp"

using pt::Configuration;

std::vector<pt::Preset> Configuration::PresetsSection::GetAll()
{
    if (m_part->find("presets") == m_part->end())
    {
        return std::vector<Preset>();
    }

    picojson::value& v = m_part->at("presets");
    picojson::array& a = v.get<picojson::array>();

    std::vector<Preset> presets;

    for (auto& p : a)
    {
        picojson::object& stored = p.get<picojson::object>();

        Preset preset(stored["name"].get<std::string>());
        preset.move_completed_downloads = stored["move_completed_downloads"].get<bool>();
        preset.move_completed_path = stored["move_completed_path"].get<std::string>();
        preset.save_path = stored["save_path"].get<std::string>();

        presets.push_back(preset);
    }

    return presets;
}

void Configuration::PresetsSection::SetAll(std::vector<pt::Preset> const& presets)
{
    picojson::array a;

    for (Preset const& preset : presets)
    {
        picojson::object p;
        p.insert({ "name", picojson::value(preset.name) });
        p.insert({ "save_path", picojson::value(preset.save_path.string()) });
        p.insert({ "move_completed_downloads", picojson::value(preset.move_completed_downloads) });
        p.insert({ "move_completed_path", picojson::value(preset.move_completed_path.string()) });

        a.push_back(picojson::value(p));
    }

    if (m_part->find("presets") == m_part->end())
    {
        m_part->insert({ "presets", picojson::value(picojson::array()) });
    }

    (*m_part)["presets"] = picojson::value(a);
}
