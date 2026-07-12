#ifndef menu_h
#define menu_h

#include <Arduino.h>
#include "hardware.h"

const uint8_t ROW_NONE = 0xff;

class MenuHardware
{
public:
    MenuHardware(LCD& lcd, const Button& upButton, const Button& toggleButton, const Button& downButton);
    LCD& lcd;
    const Button& upButton;
    const Button& toggleButton;
    const Button& downButton;
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
    Menu(const MenuHardware& hardware, MenuRow* rows[], uint8_t rowCount, uint8_t initSelected);
    const MenuHardware& getHardware() const;
    void update();
    /**
     * Called when menu stops being displayed.
     * This ensures that all the states are correct when the menu is displayed again.
     */
    void disable();
private:
    const MenuHardware& hardware_;

    MenuRow** rows_;
    uint8_t rowCount_;
    uint8_t initSelected_ = 0;
    /**
     * the currently selected row. controlled by `upButton_` and `downButton_`.
     */
    uint8_t selected_ = 0;
    uint8_t scroll_ = 0;
};

// TODO can this be done without using Menu* (rather using Menu&)
class MenuController
{
public:
    MenuController(LCD& lcd);
    void use(Menu* menu);
private:
    Menu* prevMenu_ = nullptr;
    LCD& lcd_;
};

class MenuOptionRow: public MenuRow
{
public:
    MenuOptionRow(const MenuHardware& hardware, uint8_t* value, const char* label, const char* options[], uint8_t optionsLen);
    MenuOptionRow(const MenuHardware& hardware, uint8_t* value, const char* label, const char* options[], uint8_t optionsLen, bool (*isHidden)());
private:
    void updateInternal();
    /**
     * the value that is being controlled. it is used to index `options_` and must be less than `optionsLen_`.
     * 
     * this is a pointer to the value as the value will be mutated. this is controlled by `menu.toggleButton_`.
     */
    uint8_t* value_;
    uint8_t prevValue_;
    const char* label_;
    const char** options_;
    uint8_t optionsLen_;
    long nextBlinkTime_ = 2147483647L;
    bool blinkHide_ = false;

    void printLabel_();
    void printOption_();
};

class MenuActionRow: public MenuRow
{
public:
    MenuActionRow(const MenuHardware& hardware, const char* label, void (*action)());
    MenuActionRow(const MenuHardware& hardware, const char* label, void (*action)(), bool (*isHidden)());
private:
    void updateInternal();
    void (*action_)();
    const char* label_;
    long nextBlinkTime_ = 2147483647L;
    bool blinkHide_ = false;

    void print_();
};

class MenuInfoRow: public MenuRow
{
public:
    MenuInfoRow(const MenuHardware& hardware, char* info);
    MenuInfoRow(const MenuHardware& hardware, char* info, bool (*isHidden)());
private:
    void updateInternal();
    char* info_;
    void print_();
};

#endif