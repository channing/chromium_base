#include "stdafx.h"
#include "menu_item_model.h"

#include "base\stl_util.h"
#include "base\logging.h"

MenuItemModel::~MenuItemModel() {
    STLDeleteElements(&children_);
}

void MenuItemModel::SetView(MenuItemView* view) {
    DCHECK(view == NULL || view_ == NULL);
    view_ = view;
}

string16 MenuItemModel::GetLable() {
    return string16();
}

void MenuItemModel::Execute() {

}

void MenuItemModel::WillShowMenu() {

}