#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    namespace UI
    {
        class MainFrame;
    }

    class PersistenceManager;

    class Application : public wxApp
    {
    public:
        Application();
        virtual ~Application();

        virtual bool OnInit();

    private:
        std::unique_ptr<PersistenceManager> m_persistence;
        UI::MainFrame* m_mainFrame;
    };
}
