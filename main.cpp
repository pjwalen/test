#include <stdio.h>
#include <array>
#include "pico/stdlib.h"
#include "keyboardMatrix.h"
#include "tusb.h"


int main()
{
    stdio_init_all();
    KeyboardMatrix keyboardMatrix = KeyboardMatrix();
    keyboardMatrix.configureRowGpios({{2, 0, 1}});
    keyboardMatrix.configureColumnGpios({{3, 4, 5}});
    keyboardMatrix.configureMapping({{
        {{Key::ESC,   Key::SPACE, Key::CTRL}},
        {{Key::SHIFT, Key::D,     Key::S}},
        {{Key::NONE,  Key::W,     Key::A}}
    }});
    tusb_init();
    uint64_t lastChangeTime = 0;
    while (1) {
        // Keep the USB stack running
        tud_task(); 
        
        keyboardMatrix.scan();
        if(lastChangeTime == keyboardMatrix.getLastChangeTime()) {
            continue;
        }
        if(tud_hid_ready()) {
            lastChangeTime = keyboardMatrix.getLastChangeTime();
            hid_keyboard_report_t report;
            keyboardMatrix.getHIDReport(report);
            tud_hid_report(0, &report, sizeof(report));
        }
    }

    return 0;
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