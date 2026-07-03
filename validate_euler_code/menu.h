#ifndef menu_h
#define menu_h //Prevents double definition

#include <Arduino.h>
#include "hardware.h"

const uint8_t ROW_NONE = 0xff;

class MenuHardware //Gamemaster controls
{
public:
    MenuHardware(const LCD& lcd, const Button& upButton, const Button& toggleButton, const Button& downButton); //LCD & 3 buttons
    const LCD& getLCD() const;
    const Button& getUpButton() const;
    const Button& getToggleButton() const;
    const Button& getDownButton() const;
private:
    const LCD& lcd_;
    const Button& upButton_;
    const Button& toggleButton_;
    const Button& downButton_;
};

class MenuRow
{
public:
    MenuRow(const MenuHardware& hardware, bool (*isHidden)());
    virtual ~MenuRow() {}
    void update(uint8_t row, bool selected);
    virtual bool isHidden();
protected:
    const MenuHardware& hardware_;
    virtual void updateInternal() = 0;
    bool isSelected();
    bool selectedChanged();
    uint8_t getRow();
    bool rowChanged();
private:
    bool (*isHidden_)();
    /**
    * the current row of the screen that it is displayed on
    */
    uint8_t row_ = ROW_NONE;
    uint8_t prevRow_ = ROW_NONE;
    /**
     * whether the row is currently selected
     */
    bool selected_ = false;
    bool prevSelected_ = false;
};

class Menu
{
public:
    Menu(const MenuHardware& hardware, MenuRow* rows[], uint8_t rowCount);
    const MenuHardware& getHardware() const;
    void update();
private:
    const MenuHardware& hardware_;

    MenuRow** rows_;
    uint8_t rowCount_;
    /**
     * the currently selected row. controlled by `upButton_` and `downButton_`.
     */
    uint8_t selected_ = 0;
    uint8_t scroll_ = 0;
};

class MenuOptionRow: public MenuRow
{
public:
    MenuOptionRow(const MenuHardware& hardware, uint8_t* value, char* label, const char* options[], uint8_t optionsLen);
    MenuOptionRow(const MenuHardware& hardware, uint8_t* value, char* label, const char* options[], uint8_t optionsLen, bool (*isHidden)());
private:
    void updateInternal();
    /**
     * the value that is being controlled. it is used to index `options_` and must be less than `optionsLen_`.
     * 
     * this is a pointer to the value as the value will be mutated. this is controlled by `menu.toggleButton_`.
     */
    uint8_t* value_;
    uint8_t prevValue_;
    char* label_;
    const char** options_;
    uint8_t optionsLen_;
    long nextBlinkTime_ = 2147483647L;
    bool blinkHide_ = false;

    void printLabel_();
    void printOption_();
};

// TODO actually add an action or smth
class MenuActionRow: public MenuRow
{
public:
    MenuActionRow(const MenuHardware& hardware, char* label);
    MenuActionRow(const MenuHardware& hardware, char* label, bool (*isHidden)());
    void updateInternal();
private:
    char* label_;
    long nextBlinkTime_ = 2147483647L;
    bool blinkHide_ = false;

    void print_();
};

#endif