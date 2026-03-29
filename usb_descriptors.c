#include "tusb.h"

// --------------------------------------------------------------------+
// 1. Device Descriptor (The Passport)
// --------------------------------------------------------------------+
// This tells the PC the Vendor ID (VID), Product ID (PID), and USB version.
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200, // USB 2.0
    .bDeviceClass       = 0x00,   // Class specified in interface
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    
    .idVendor           = 0xCafe, // Generic testing VID
    .idProduct          = 0x4001, // Generic testing PID
    .bcdDevice          = 0x0100, // Hardware version 1.0

    .iManufacturer      = 0x01,   // Index of String Descriptor 1
    .iProduct           = 0x02,   // Index of String Descriptor 2
    .iSerialNumber      = 0x00,   // 0 means no serial number
    .bNumConfigurations = 0x01
};

// Callback TinyUSB uses to get the Device Descriptor
uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

// --------------------------------------------------------------------+
// 2. HID Report Descriptor (The Data Dictionary)
// --------------------------------------------------------------------+
// This is the "map" that tells the PC exactly what a keystroke packet looks like.
// TinyUSB provides a brilliant macro so we don't have to write raw hex!
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

// Callback TinyUSB uses to get the HID Report Descriptor
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
    return desc_hid_report;
}

// --------------------------------------------------------------------+
// 3. Configuration Descriptor (The Blueprint)
// --------------------------------------------------------------------+
// Tells the PC how much power we need and defines our endpoints.
enum { ITF_NUM_KEYBOARD = 0, ITF_NUM_TOTAL };

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_KEYBOARD    0x81 // Endpoint 1, Direction IN (Device to Host)

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_KEYBOARD, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_KEYBOARD, CFG_TUD_HID_EP_BUFSIZE, 10)
};

// Callback TinyUSB uses to get the Configuration Descriptor
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    return desc_configuration;
}

// --------------------------------------------------------------------+
// 4. String Descriptors (The Human-Readable Names)
// --------------------------------------------------------------------+
// Array of pointer to string descriptors
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, // 0: Supported language is English (0x0409)
    "Walentsiny Tech",               // 1: Manufacturer
    "Quartermaster Keyboard",             // 2: Product
};

static uint16_t _desc_str[32];

// Callback TinyUSB uses to get String Descriptors
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    uint8_t chr_count;

    if ( index == 0 ) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;

        // Convert ASCII string into UTF-16
        for(uint8_t i=0; i<chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    // First byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
}