#include "menu.h"
/** 
 * the number of rows from the edge that the selected row can be before scrolling will happen
 */
const uint8_t SCROLL_THRESHOLD = 0;

const uint8_t LABEL_WIDTH = 12;
const int BLINK_INTERVAL = 500;

MenuHardware::MenuHardware(LCD& lcd, const Button& upButton, const Button& toggleButton, const Button& downButton): 
    lcd(lcd),
    upButton(upButton),
    toggleButton(toggleButton),
    downButton(downButton)
{}

MenuRow::MenuRow(const MenuHardware& hardware, bool (*isHidden)()): hardware_(hardware), isHidden_(isHidden)
{
}
void MenuRow::update(uint8_t row, bool selected)
{
    prevRow_ = row_;
    row_ = row;
    prevSelected_ = selected_;
    selected_ = selected;
    updateInternal();
}
bool MenuRow::isHidden() 
{
    return isHidden_ != NULL && isHidden_();
}
bool MenuRow::isSelected()
{
    return selected_;
}
bool MenuRow::selectedChanged()
{
    return selected_ != prevSelected_;
}
uint8_t MenuRow::getRow()
{
    return row_;
}
bool MenuRow::rowChanged()
{
    return row_ != prevRow_;
}

Menu::Menu(const MenuHardware& hardware, MenuRow* rows[], uint8_t rowCount): 
    hardware_(hardware),
    rows_(rows),
    rowCount_(rowCount)
{
}
const MenuHardware& Menu::getHardware() const
{
    return hardware_;
}
void Menu::update()
{
    uint8_t visibleRowCount = 0;
    uint8_t visibleRowIndex[rowCount_] = {};
    uint8_t lastVisible = 0xff;
    bool selOk = false;
    for (int i = 0; i < rowCount_; i++) {
        if (!rows_[i]->isHidden()) {
            visibleRowIndex[i] = visibleRowCount++;
            lastVisible = i;
        } else {
            visibleRowIndex[i] = ROW_NONE;
        }
        // if selected_ is hidden, select the previous visible row
        // if there is no previous visible row, then select the next visible row
        if ((i == selected_ && i >= lastVisible) || (!selOk && i == lastVisible && i > selected_)) {
            selected_ = lastVisible;
            selOk = true;
        }
    }
    // if selOk is not set to true it means there are no selectable (ie visible) rows

    // handle button clicks to update selected row and scroll
    if (selOk && selected_ > 0 && getHardware().upButton.toggled(true)) {
        // select the visible row above
        for (uint8_t sel = selected_ - 1; sel >= 0; sel--) {
            if (visibleRowIndex[sel] != ROW_NONE) {
                selected_ = sel;
                break;
            }
        }
    }
    if (selOk && selected_ < rowCount_ - 1 && getHardware().downButton.toggled(true)) {
        // select the visible row below
        for (uint8_t sel = selected_ + 1; sel < rowCount_; sel++) {
            if (visibleRowIndex[sel] != ROW_NONE) {
                selected_ = sel;
                break;
            }
        }
    }

    // clamp the scroll_ value
    if (scroll_ < 0) {
        scroll_ = 0;
    }
    if (scroll_ > visibleRowCount - 4) {
        scroll_ = visibleRowCount - 4;
    }

    // scroll selected_ into view
    // TODO decide if it should scroll when it hits the last or 2nd last row (just change the < to <= and vice versa)
    if (selOk) {
        while (visibleRowIndex[selected_] < scroll_ + SCROLL_THRESHOLD && scroll_ > 0) {
            scroll_--;
        }
        while (visibleRowIndex[selected_] > scroll_ + 3 - SCROLL_THRESHOLD && scroll_ < visibleRowCount - 4) {
            scroll_++;
        }
    }

    // update all the rows
    for (int i = 0; i < rowCount_; i++) {
        // rows that are either hidden or not in view due to scrolling will have a row value of ROW_NONE
        // which tells the MenuRow not to print anything
        short row = visibleRowIndex[i];
        if (row != ROW_NONE) {
            row -= scroll_;
            if (row < 0 || row >= 4) {
                row = ROW_NONE;
            }
        }
        int selected = selected_ == i && selOk;
        rows_[i]->update(row, selected);
    }
}

void Menu::disable()
{
    for (int i = 0; i < rowCount_; i++) {
        rows_[i]->update(ROW_NONE, false);
    }
}

MenuOptionRow::MenuOptionRow(const MenuHardware& hardware, uint8_t *value, char *label, const char *options[], uint8_t optionsLen):
    MenuOptionRow(hardware, value, label, options, optionsLen, NULL)
{
}
MenuOptionRow::MenuOptionRow(const MenuHardware& hardware, uint8_t *value, char *label, const char *options[], uint8_t optionsLen, bool (*isHidden)()):
    MenuRow(hardware, isHidden),
    value_(value),
    prevValue_(*value),
    label_(label),
    options_(options),
    optionsLen_(optionsLen)
{
}

void MenuOptionRow::updateInternal()
{
    prevValue_ = *value_;

    if (getRow() == ROW_NONE) {
        return;
    }

    if (isSelected() && hardware_.toggleButton.toggled(true)) {
        *value_ = (*value_ + 1) % optionsLen_;
    }
    
    // this can probably be better organised
    // between update and print who is responsible for what

    if (isSelected()) {
        // reset blinking if just selected or value just changed
        if (selectedChanged() || prevValue_ != *value_) {
            blinkHide_ = false;
            nextBlinkTime_ = millis() + BLINK_INTERVAL;
        }
        long time = millis();
        if (time >= nextBlinkTime_) {
            blinkHide_ = !blinkHide_;
            while (nextBlinkTime_ <= time) {
                nextBlinkTime_ += BLINK_INTERVAL;
            }
            printOption_();
        }
    }

    if (rowChanged() || selectedChanged()) {
        if (!isSelected()) {
            // ensure there is no blinking if not selected
            blinkHide_ = false;
            nextBlinkTime_ = 2147483647L;
        }
        // reprint the entire thing
        printLabel_();
        printOption_();
    } else if (prevValue_ != *value_) {
        printOption_();
    }
}

void MenuOptionRow::printLabel_() {
    LCD& lcd = hardware_.lcd;
    if (isSelected()) {
        lcd.print(getRow(), 0, ' ');
        lcd.print(getRow(), 1, LABEL_WIDTH - 1);
    }
    int col = isSelected() ? 1 : 0;
    lcd.print(getRow(), col, LABEL_WIDTH - 1 - col, label_);
    lcd.print(getRow(), LABEL_WIDTH - 1, ' ');
}

void MenuOptionRow::printOption_()
{
    LCD& lcd = hardware_.lcd;
    uint8_t row = getRow();

    // draw the arrow if selected (only redraw when just selected or row changed)
    if (selectedChanged() || rowChanged()) {
        if (isSelected()) {
            lcd.write(row, LABEL_WIDTH, LCD_CHAR_ARROW);
            lcd.print(row, LABEL_WIDTH + 1, ' ');
        } else {
            lcd.print(row, LABEL_WIDTH, ' ');
        }
    }

    uint8_t col = LABEL_WIDTH + 1;
    if (isSelected()) {
        col += 1;
    }

    const char* opt = "";
    if (!blinkHide_) {
        opt = options_[*value_];
    }
    lcd.print(row, col, 20 - col, opt);
}

MenuActionRow::MenuActionRow(const MenuHardware& hardware, char* label, void (*action)()): MenuActionRow(hardware, label, action, NULL)
{}
MenuActionRow::MenuActionRow(const MenuHardware& hardware, char* label, void (*action)(), bool (*isHidden)()):
    MenuRow(hardware, isHidden),
    label_(label),
    action_(action)
{}
void MenuActionRow::updateInternal()
{
    if (getRow() == ROW_NONE) {
        return;
    }

    if (isSelected() && hardware_.toggleButton.toggled(true)) {
        if (action_ != NULL) {
            action_();
        }
    }

    if (isSelected()) {
        // reset blinking if just selected
        if (selectedChanged()) {
            blinkHide_ = false;
            nextBlinkTime_ = millis() + BLINK_INTERVAL;
        }
        long time = millis();
        if (time >= nextBlinkTime_) {
            blinkHide_ = !blinkHide_;
            while (nextBlinkTime_ <= time) {
                nextBlinkTime_ += BLINK_INTERVAL;
            }
            print_();
        }
    }

    if (selectedChanged() || rowChanged()) {
        if (!isSelected()) {
            // ensure there is no blinking if not selected
            blinkHide_ = false;
            nextBlinkTime_ = 2147483647L;
        }
        // reprint the entire thing
        print_();
    }
}
void MenuActionRow::print_()
{
    LCD& lcd = hardware_.lcd;
    uint8_t row = getRow();
    if (selectedChanged() || rowChanged()) {
        if (isSelected()) {
            lcd.write(row, 0, LCD_CHAR_ARROW);
            lcd.print(row, 1, ' ');
        } else {
            lcd.print(getRow(), 0, ' ');
        }
    }
    uint8_t col = 1;
    if (isSelected()) {
        col += 1;
    }
    char* label = "";
    if (!blinkHide_) {
        label = label_;
    }
    lcd.print(getRow(), col, 20 - col, label);
}

MenuController::MenuController(LCD& lcd):
    lcd_(lcd)
{}

void MenuController::use(Menu *menu)
{
    if (prevMenu_ != menu) {
        if (prevMenu_ != NULL) {
            prevMenu_->disable();
        }
        prevMenu_ = menu;
        lcd_.clear();
    }
    if (menu != NULL) {
        menu->update();
    }
}
