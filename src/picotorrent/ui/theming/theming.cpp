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

wxColour Theming::GetBackgroundColor(){
  return GetMSWDarkMode() ? wxColour(32,32,32) : wxColour(255,255,255);
}

void Theming::LoadThemes()
{
  bool systemUsesDarkTheme = false;
  HKEY hKey = 0;
  DWORD dwValue = 1;
  DWORD dwBufSize = sizeof(dwValue);
  if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
  {
    RegQueryValueEx(hKey, TEXT("AppsUseLightTheme"), NULL, NULL, (LPBYTE)&dwValue, &dwBufSize);
  }
  if (dwValue == 0)
  {
    systemUsesDarkTheme = true;
  }

  m_themes = {{
                  "light",
                  {"light",
                   "light_mode",
                   false},
              },
              {"dark",
               {"dark",
                "dark_mode",
                true}},
              {"system",
               {"system",
                "system_sets_theme_mode",
                systemUsesDarkTheme}}};
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
