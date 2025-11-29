#ifndef _LUFA_CONFIG_H_
#define _LUFA_CONFIG_H_

/* General USB Driver Related Tokens: */

/**
 * This option tells LUFA that we are providing the USB initialization options here,
 * which simplifies the USB_Init() function call in our main.c. The options set
 * are standard for a full-speed, bus-powered device.
 */
#define USE_STATIC_OPTIONS               (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)

/**
 * This is a crucial optimization that tells LUFA to only compile the code
 * needed for USB Device mode, completely excluding the USB Host mode code.
 * This saves a significant amount of flash space.
 */
#define USB_DEVICE_ONLY


/* USB Device Mode Driver Related Tokens: */

/**
 * This tells LUFA that our USB descriptors are stored in flash memory (PROGMEM),
 * which is the standard way to do it. This also affects the function signature
 * of CALLBACK_USB_GetDescriptor().
 */
#define USE_FLASH_DESCRIPTORS

/**
 * This sets the size of the control endpoint, which is fundamental for USB
 * communication. This was required to fix a build error.
 */
#define FIXED_CONTROL_ENDPOINT_SIZE      8

/**
 * This tells LUFA that our device has only one configuration. This is true for
 * our simple device and was also required to fix a build error.
 */
#define FIXED_NUM_CONFIGURATIONS         1

#endif
