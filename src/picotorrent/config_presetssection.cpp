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

        Preset preset;
        preset.move_completed_downloads = stored["move_completed_downloads"].get<bool>();
        preset.move_completed_path = stored["move_completed_path"].get<std::string>();
        preset.name = stored["name"].get<std::string>();
        preset.save_path = stored["save_path"].get<std::string>();

        presets.push_back(preset);
    }

    return presets;
}

pt::Preset Configuration::PresetsSection::GetByIndex(int index, bool* found)
{
    *found = false;

    if (m_part->find("presets") == m_part->end())
    {
        return Preset();
    }

    picojson::value& v = m_part->at("presets");
    picojson::array& a = v.get<picojson::array>();

    if (index + 1 > a.size())
    {
        return Preset();
    }

    picojson::object& stored = a.at(index).get<picojson::object>();

    Preset preset;
    preset.move_completed_downloads = stored["move_completed_downloads"].get<bool>();
    preset.move_completed_path = stored["move_completed_path"].get<std::string>();
    preset.name = stored["name"].get<std::string>();
    preset.save_path = stored["save_path"].get<std::string>();

    *found = true;
    return preset;
}

size_t Configuration::PresetsSection::Insert(pt::Preset const& preset)
{
    if (m_part->find("presets") == m_part->end())
    {
        m_part->insert({ "presets", picojson::value(picojson::array()) });
    }

    picojson::value& v = m_part->at("presets");
    picojson::array& a = v.get<picojson::array>();

    picojson::object p;
    p.insert({ "name", picojson::value(preset.name) });
    p.insert({ "save_path", picojson::value(preset.save_path.string()) });
    p.insert({ "move_completed_downloads", picojson::value(preset.move_completed_downloads) });
    p.insert({ "move_completed_path", picojson::value(preset.move_completed_path.string()) });

    a.push_back(picojson::value(p));

    return a.size();
}

void Configuration::PresetsSection::Remove(size_t index)
{
    if (m_part->find("presets") == m_part->end())
    {
        return;
    }

    picojson::value& v = m_part->at("presets");
    picojson::array& arr = v.get<picojson::array>();

    arr.erase(arr.begin() + index);
}

void Configuration::PresetsSection::Update(size_t index, pt::Preset const& preset)
{
    if (m_part->find("presets") == m_part->end())
    {
        return;
    }

    picojson::value& v = m_part->at("presets");
    picojson::array& arr = v.get<picojson::array>();

    picojson::object& presetObject = arr.at(index).get<picojson::object>();
    presetObject.at("name") = picojson::value(preset.name);
    presetObject.at("save_path") = picojson::value(preset.save_path.string());
    presetObject.at("move_completed_downloads") = picojson::value(preset.move_completed_downloads);
    presetObject.at("move_completed_path") = picojson::value(preset.move_completed_path.string());
}
