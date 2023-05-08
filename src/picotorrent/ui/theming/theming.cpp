#include "theming.hpp"

#include <wx/colour.h>

#include <string>

#include "../translator.hpp"

using pt::UI::Theming;

Theming::Theming() : m_currentTheme("light") {}

Theming &Theming::GetInstance()
{
  static Theming theming;
  return theming;
}

std::string Theming::GetCurrentTheme() { return m_currentTheme; }

bool Theming::GetMSWDarkMode()
{
  auto theme = m_themes.find(m_currentTheme);
  if (theme == m_themes.end())
  {
    theme = m_themes.find("light");
  }
  if (theme == m_themes.end())
  {
    return false;
  }
  auto dark_mode = theme->second.dark_mode;
  return dark_mode;
}

void Theming::LoadThemes()
{
  m_themes = {{
                  "light",
                  {"light",
                   "light_mode",
                   false},
              },
              {"dark",
               {"dark",
                "dark_mode",
                true}}};
}

std::vector<Theming::Theme> Theming::GetThemes()
{
  std::vector<Theme> result;

  for (auto &p : m_themes)
  {
    result.push_back(p.second);
  }

  std::sort(
      result.begin(), result.end(),
      [](Theme const &lhs, Theme const &rhs)
      { return lhs.i18n_name < rhs.i18n_name; });

  return result;
}

void Theming::SetCurrentTheme(std::string const &theme_id)
{
  m_currentTheme = theme_id;
}
