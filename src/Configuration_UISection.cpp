#include "Configuration.hpp"

bool Configuration::UISection::GetShowInNotificationArea()
{
    return Get("ui", "show_in_notification_area", true);
}

void Configuration::UISection::SetShowInNotificationArea(bool value)
{
    Set("ui", "show_in_notification_area", value);
}

bool Configuration::UISection::GetCloseToNotificationArea()
{
    return Get("ui", "close_to_notification_area", false);
}

void Configuration::UISection::SetCloseToNotificationArea(bool value)
{
    Set("ui", "close_to_notification_area", value);
}

bool Configuration::UISection::GetMinimizeToNotificationArea()
{
    return Get("ui", "minimize_to_notification_area", false);
}

void Configuration::UISection::SetMinimizeToNotificationArea(bool value)
{
    Set("ui", "minimize_to_notification_area", value);
}

bool Configuration::UISection::GetShowAddTorrentDialog()
{
    return Get("ui", "show_add_torrent_dialog", true);
}
