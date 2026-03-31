#include "keyboardMatrix.h"


void KeyboardMatrix::configureRowGpios(const std::array<uint, 3>& gpioPins) {
    rowGpio = gpioPins;
    for (const auto& gpioPin : gpioPins) {
        gpio_init(gpioPin);
        gpio_set_dir(gpioPin, GPIO_OUT);
        gpio_put(gpioPin, true);
    }
}


void KeyboardMatrix::configureColumnGpios(const std::array<uint, 3>& gpioPins) {
    columnGpio = gpioPins;
    for (const auto& gpioPin : gpioPins) {
        gpio_init(gpioPin);
        gpio_pull_up(gpioPin);
    }
}


void KeyboardMatrix::configureMapping(const std::array<std::array<Key, 3>, 3>& mapping) {
    keyMap = mapping;
}


void KeyboardMatrix::scan() {
    for(uint row = 0; row < rowGpio.size(); ++row) {
        gpio_put(rowGpio[row], false);
        for(uint col = 0; col < columnGpio.size(); ++col) {
            const Key currentKey = keyMap[col][row];
            if(currentKey == Key::NONE) {
                continue;
            }

            // Handle debounce
            uint64_t currentTime = time_us_64();
            if (currentTime - timeSinceStateChange[col][row] < debounceTimeoutUs) {
                continue;
            }

            // Read the current row
            bool isPressed = !gpio_get(columnGpio[col]);

            // There was a change in the pin state
            if(keyState[col][row] != isPressed) {
                // Update the state
                keyState[col][row] = isPressed;
                timeSinceStateChange[col][row] = currentTime;
                lastChangeTime = currentTime;

                if(keyState[col][row]) {
                    printf("%s has been pressed.\n", getKeyString(col, row));
                } else {
                    printf("%s has been released.\n", getKeyString(col, row));
                }
            }
        }
        gpio_put(rowGpio[row], true);
    }
}


uint64_t KeyboardMatrix::getLastChangeTime() const {
    return lastChangeTime;
}


void KeyboardMatrix::getHIDReport(hid_keyboard_report_t& report) {
    uint keyCount = 0;
    report.modifier = 0;
    report.reserved = 0;
    for(int i = 0; i < 6; ++i) {
        report.keycode[i] = 0;
    }
    for(uint col = 0; col < keyMap.size(); ++col) {
        for(uint row = 0; row < keyMap[col].size(); ++row) {
            Key currentKey = keyMap[col][row];
            if(currentKey == Key::NONE) {
                continue;
            }
            bool isPressed = keyState[col][row];
            if(!isPressed) {
                continue;
            }
            switch(currentKey) {
                case Key::W:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_W;
                        ++keyCount;
                    }
                    break;
                case Key::A:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_A;
                        ++keyCount;
                    }
                    break;
                case Key::S:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_S;
                        ++keyCount;
                    }
                    break;
                case Key::D:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_D;
                        ++keyCount;
                    }
                    break;
                case Key::SPACE:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_SPACE;
                        ++keyCount;
                    }
                    break;
                case Key::ESC:
                    if(keyCount < 6) {
                        report.keycode[keyCount] = HID_KEY_ESCAPE;
                        ++keyCount;
                    }
                    break;
                case Key::CTRL:
                    report.modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
                    break;
                case Key::SHIFT:
                    report.modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
                    break;
            }
        }
    }
}


const char* KeyboardMatrix::getKeyString(uint col, uint row) const {
    switch(keyMap[col][row]) {
        case Key::ESC: return "ESC";
        case Key::SPACE: return "SPACE";
        case Key::CTRL: return "CTRL";
        case Key::SHIFT: return "SHIFT";
        case Key::D: return "D";
        case Key::S: return "S";
        case Key::W: return "W";
        case Key::A: return "A";
        case Key::NONE: return nullptr;
    }
    return nullptr; // Should never reach here
}
