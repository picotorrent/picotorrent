#include "filecontextmenu.hpp"

#include "translator.hpp"

using pt::FileContextMenu;

FileContextMenu::FileContextMenu(std::shared_ptr<pt::Translator> tr)
{
    wxMenu* prioMenu = new wxMenu();
    prioMenu->Append(ptID_PRIO_MAXIMUM, i18n(tr, "maximum"));
    prioMenu->Append(ptID_PRIO_NORMAL, i18n(tr, "normal"));
    prioMenu->Append(ptID_PRIO_LOW, i18n(tr, "low"));
    prioMenu->AppendSeparator();
    prioMenu->Append(ptID_PRIO_DO_NOT_DOWNLOAD, i18n(tr, "do_not_download"));

    AppendSubMenu(prioMenu, i18n(tr, "priority"));
}
