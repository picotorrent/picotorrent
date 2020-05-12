#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

namespace pt
{
namespace API
{
    class IPlugin;
}

    class PersistenceManager;

    class Application : public wxApp
    {
    public:
        Application();
        virtual ~Application();

        virtual bool OnInit();

    private:
        std::vector<API::IPlugin*> m_plugins;
        std::unique_ptr<PersistenceManager> m_persistence;
    };
}
