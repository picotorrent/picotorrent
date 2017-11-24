#include "filecontextmenu.hpp"

#include "translator.hpp"

using pt::FileContextMenu;

FileContextMenu::FileContextMenu(std::shared_ptr<pt::Translator> tr)
{
    wxMenu* prioMenu = new wxMenu();
    prioMenu->Append(ptID_PRIO_MAXIMUM, i18n(tr, "maximum"));
    prioMenu->Append(ptID_PRIO_HIGH, i18n(tr, "high"));
    prioMenu->Append(ptID_PRIO_NORMAL, i18n(tr, "normal"));
    prioMenu->AppendSeparator();
    prioMenu->Append(ptID_PRIO_SKIP, i18n(tr, "do_not_download"));

    AppendSubMenu(prioMenu, i18n(tr, "priority"));
}
