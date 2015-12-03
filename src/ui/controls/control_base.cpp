#include <picotorrent/ui/controls/control_base.hpp>

using picotorrent::ui::controls::control_base;

control_base::control_base(HWND handle)
    : handle_(handle)
{
}

HWND control_base::handle() const
{
    return handle_;
}
