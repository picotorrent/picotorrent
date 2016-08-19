#include "CheckBox.hpp"

using UI::CheckBox;

void CheckBox::Check()
{
    SetCheck(BST_CHECKED);
}

bool CheckBox::IsChecked()
{
    return GetCheck() == BST_CHECKED;
}

void CheckBox::Toggle()
{
    bool isChecked = GetCheck() == BST_CHECKED;
    SetCheck(isChecked ? BST_UNCHECKED : BST_CHECKED);
}
