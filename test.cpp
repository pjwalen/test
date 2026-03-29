#include <stdio.h>
#include <array>
#include "pico/stdlib.h"
#include "tusb.h"

void configureRow(uint gpioPin);
void configureColumn(uint gpioPin);
void scan(uint currentRow, uint currentColumn);

std::array<uint, 3> columnGpio = {3, 4, 5};
std::array<uint, 3> rowGpio = {2, 0, 1};
enum class Key: uint {
    ESC = 0,
    SHIFT = 1,
    SPACE = 2,
    D = 3,
    W = 4,
    CTRL = 5,
    S = 6,
    A = 7,
    NONE = 1024
};
constexpr std::array<const char*, 9> keyStringMap = {"ESC", "SHIFT", "SPACE", "D", "W", "CTRL", "S", "A", "NONE"};
constexpr std::array<std::array<Key, 3>, 3> colRowMap = {{
    {{Key::ESC,   Key::SPACE,  Key::CTRL}},
    {{Key::SHIFT, Key::D,      Key::S}},
    {{Key::NONE,  Key::W,      Key::A}}
}};
constexpr uint64_t debounceTimeoutUs = 5000;

// Configure state tracking
std::array<bool, 9> keyState = {false};
std::array<uint64_t, 9> timeSinceStateChange = {0};


int main()
{
    stdio_init_all();
    tusb_init();
    while (1) {
        // Keep the USB stack running
        tud_task(); 
        
        // Your custom code to scan the keyboard matrix 
        // and send HID reports goes here!
    }
    // // Configure the GPIO for rows 0-2 of the keyboard matrix
    // configureRow(rowGpio[0]);
    // configureRow(rowGpio[1]);
    // configureRow(rowGpio[2]);

    // // Configure the GPIO for columns 0-2 of the keyboard matrix
    // configureColumn(columnGpio[0]);
    // configureColumn(columnGpio[1]);
    // configureColumn(columnGpio[2]);

    // while(true) {
    //     for(uint row = 0; row < rowGpio.size(); ++row) {
    //         for(uint col = 0; col < columnGpio.size(); ++col) {
    //             scan(row, col);
    //         }
    //     }
    // }

    return 0;
}


void configureRow(uint gpioPin) {
    gpio_init(gpioPin);
    gpio_set_dir(gpioPin, GPIO_OUT);
    gpio_put(gpioPin, true);
}


void configureColumn(uint gpioPin) {
    gpio_init(gpioPin);
    gpio_pull_up(gpioPin);
}


void scan(uint currentRow, uint currentColumn) {
        Key currentKey = colRowMap[currentColumn][currentRow];
        if(currentKey == Key::NONE) {
            return;
        }

        // Handle debounce
        uint64_t newTime = time_us_64();
        if (newTime - timeSinceStateChange[static_cast<size_t>(currentKey)] < debounceTimeoutUs) {
            return;
        }

        // Read the current row
        gpio_put(rowGpio[currentRow], false);
        bool isPressed = !gpio_get(columnGpio[currentColumn]);

        // There was a change in the pin state
        if(keyState[static_cast<size_t>(currentKey)] != isPressed) {
            // Update the state
            keyState[static_cast<size_t>(currentKey)] = isPressed;
            timeSinceStateChange[static_cast<size_t>(currentKey)] = newTime;

            if(keyState[static_cast<size_t>(currentKey)]) {
                printf("%s has been pressed.\n", keyStringMap[static_cast<size_t>(currentKey)]);
            } else {
                printf("%s has been released.\n", keyStringMap[static_cast<size_t>(currentKey)]);
            }
        }
        gpio_put(rowGpio[currentRow], true);
}

// ====================================================================
// TinyUSB HID Callbacks (Required for the library to compile)
// ====================================================================

extern "C" {

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // We don't need to do anything here for a basic keyboard
    return 0;
}

// Invoked when received SET_REPORT control request or received data on OUT endpoint
// (e.g., PC telling the keyboard to turn on the Caps Lock LED)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    // We will handle Caps Lock / Num lock LEDs here later!
}

} // end extern "C"