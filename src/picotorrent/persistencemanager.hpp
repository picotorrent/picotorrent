#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/persist.h>

namespace pt
{
namespace Core
{
    class Database;
}

    class PersistenceManager : public wxPersistenceManager
    {
    public:
        PersistenceManager(std::shared_ptr<Core::Database> db);
        virtual ~PersistenceManager();

        virtual bool RestoreValue(const wxPersistentObject& who, const wxString& name, bool* value);

        virtual bool RestoreValue(const wxPersistentObject& who, const wxString& name, int* value);

        virtual bool RestoreValue(const wxPersistentObject& who, const wxString& name, long* value);

        virtual bool RestoreValue(const wxPersistentObject& who, const wxString& name, wxString* value);

        virtual bool SaveValue(const wxPersistentObject& who, const wxString& name, bool value);

        virtual bool SaveValue(const wxPersistentObject& who, const wxString& name, int value);

        virtual bool SaveValue(const wxPersistentObject& who, const wxString& name, long value);

        virtual bool SaveValue(const wxPersistentObject& who, const wxString& name, wxString value);

    private:
        wxString GetKey(const wxPersistentObject& who, const wxString& name);
        void SaveValue(const wxString& key, std::string const& value);

        std::shared_ptr<Core::Database> m_db;
    };
}
