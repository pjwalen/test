#define KEYBOARDMATRIX_H
#ifdef KEYBOARDMATRIX_H

#include <array>
#include "pico/stdlib.h"
#include "tusb.h"

enum class Key {
    ESC, SPACE, CTRL, SHIFT, D, S, W, A, NONE
};

class KeyboardMatrix {
    public:
    KeyboardMatrix() =  default;
    ~KeyboardMatrix() = default;
    void configureRowGpios(const std::array<uint, 3>& gpioPins);
    void configureColumnGpios(const std::array<uint, 3>& gpioPins);
    void configureMapping(const std::array<std::array<Key, 3>, 3>& mapping);
    void getHIDReport(hid_keyboard_report_t& report);
    const char* getKeyString(uint col, uint row) const;
    uint64_t getLastChangeTime() const;
    void scan();

    private:
    std::array<uint, 3> columnGpio {0};
    const uint64_t debounceTimeoutUs = 5000;
    std::array<std::array<Key, 3>, 3> keyMap = {{Key::NONE}};
    std::array<std::array<bool, 3>, 3> keyState = {false};
    // This indicates the last time any key state changed, to determine if we need to send a new HID report
    uint64_t lastChangeTime = 0;
    std::array<uint, 3> rowGpio {0};
    std::array<std::array<uint64_t, 3>, 3> timeSinceStateChange = {0};
};

#endif /* KEYBOARDMATRIX_H */