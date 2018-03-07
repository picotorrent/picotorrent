#include "config.hpp"

using pt::Configuration;

bool Configuration::UISection::SkipAddTorrentDialog()
{
    return Get("ui", "skip_add_torrent_dialog", false);
}

void Configuration::UISection::SkipAddTorrentDialog(bool value)
{
    Set("ui", "skip_add_torrent_dialog", value);
}

bool Configuration::UISection::ShowInNotificationArea()
{
    return Get("ui", "show_in_notification_area", true);
}

void Configuration::UISection::ShowInNotificationArea(bool value)
{
    Set("ui", "show_in_notification_area", value);
}

bool Configuration::UISection::CloseToNotificationArea()
{
    return Get("ui", "close_to_notification_area", false);
}

void Configuration::UISection::CloseToNotificationArea(bool value)
{
    Set("ui", "close_to_notification_area", value);
}

bool Configuration::UISection::MinimizeToNotificationArea()
{
    return Get("ui", "minimize_to_notification_area", false);
}

void Configuration::UISection::MinimizeToNotificationArea(bool value)
{
    Set("ui", "minimize_to_notification_area", value);
}
