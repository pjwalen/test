#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------+
// Common Configuration
// --------------------------------------------------------------------+

// Configure TinyUSB to run in Device Mode on Port 0 (The Pico's only USB port)
#define CFG_TUSB_RHPORT0_MODE    OPT_MODE_DEVICE

// --------------------------------------------------------------------+
// Device Configuration
// --------------------------------------------------------------------+

// Standard size for the Control Endpoint (Endpoint 0). 64 bytes is standard for Full Speed.
#define CFG_TUD_ENDPOINT0_SIZE   64

// --------------------------------------------------------------------+
// Class Driver Configuration
// --------------------------------------------------------------------+

// Enable 1 instance of the HID (Human Interface Device) class
#define CFG_TUD_HID              1

// Explicitly disable the classes we are NOT using right now
// (This saves RAM and flash space on your Pico 2)
#define CFG_TUD_CDC              0  // Serial COM port
#define CFG_TUD_MSC              0  // Mass Storage (USB Drive)
#define CFG_TUD_MIDI             0  // MIDI
#define CFG_TUD_VENDOR           0  // Custom Vendor class

// --------------------------------------------------------------------+
// HID Configuration
// --------------------------------------------------------------------+

// The buffer size for HID reports. 
// 16 bytes is plenty for standard keyboards. If you build advanced custom 
// macros or use Via/Vial later, you might increase this to 64.
#define CFG_TUD_HID_EP_BUFSIZE   16

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
