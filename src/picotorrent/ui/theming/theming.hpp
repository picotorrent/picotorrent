#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <wx/colour.h>

namespace pt
{
  namespace UI
  {
    class Theming
    {
    public:
      struct Theme
      {
        std::string id;
        std::string i18n_name;
        bool dark_mode;
      };
      static Theming &GetInstance();
      Theming(Theming const &) = delete;
      void operator=(Theming const &) = delete;

      bool GetMSWDarkMode();
      std::string GetCurrentTheme();
      std::vector<Theme> GetThemes();
      void LoadThemes();
      void SetCurrentTheme(std::string const &theme_id);

    private:
      Theming();
      std::string m_currentTheme;
      std::map<std::string, Theme> m_themes;
    };
  } // namespace UI
} // namespace pt