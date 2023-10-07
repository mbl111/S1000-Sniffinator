/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "tusb.h"
#include "pico/unique_id.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]       MIDI | HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
				 _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

//Pacom Vendor ID
#define USB_VID 9910
#define USB_BCD 0x0200

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
	{
		.bLength = sizeof(tusb_desc_device_t),
		.bDescriptorType = TUSB_DESC_DEVICE,
		.bcdUSB = USB_BCD,

		// Use Interface Association Descriptor (IAD) for CDC
		// As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
		.bDeviceClass = TUSB_CLASS_MISC,
		.bDeviceSubClass = MISC_SUBCLASS_COMMON,
		.bDeviceProtocol = MISC_PROTOCOL_IAD,
		.bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

		.idVendor = USB_VID,
		.idProduct = USB_PID,
		.bcdDevice = 0x0100,

		.iManufacturer = 0x01,
		.iProduct = 0x02,
		.iSerialNumber = 0x03,

		.bNumConfigurations = 0x01

};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void)
{
	return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
enum
{
	ITF_NUM_CDC_0 = 0,
	ITF_NUM_CDC_0_DATA,
#ifndef SIMPLE_COM_PORTS
	ITF_NUM_CDC_1,
	ITF_NUM_CDC_1_DATA,
	ITF_NUM_CDC_2,
	ITF_NUM_CDC_2_DATA,
	ITF_NUM_CDC_3,
	ITF_NUM_CDC_3_DATA,
	ITF_NUM_CDC_4,
	ITF_NUM_CDC_4_DATA,
	ITF_NUM_CDC_5,
	ITF_NUM_CDC_5_DATA,
	ITF_NUM_CDC_6,
	ITF_NUM_CDC_6_DATA,
#endif
	ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN)

#if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
// LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
// 0 control, 1 In, 2 Bulk, 3 Iso, 4 In etc ...
#define EPNUM_CDC_0_NOTIF 0x81
#define EPNUM_CDC_0_OUT 0x02
#define EPNUM_CDC_0_IN 0x82

#define EPNUM_CDC_1_NOTIF 0x84
#define EPNUM_CDC_1_OUT 0x05
#define EPNUM_CDC_1_IN 0x85

#elif CFG_TUSB_MCU == OPT_MCU_SAMG || CFG_TUSB_MCU == OPT_MCU_SAMX7X
// SAMG & SAME70 don't support a same endpoint number with different direction IN and OUT
//    e.g EP1 OUT & EP1 IN cannot exist together
#define EPNUM_CDC_0_NOTIF 0x81
#define EPNUM_CDC_0_OUT 0x02
#define EPNUM_CDC_0_IN 0x83

#define EPNUM_CDC_1_NOTIF 0x84
#define EPNUM_CDC_1_OUT 0x05
#define EPNUM_CDC_1_IN 0x86

#else

#define EPNUM_CDC_0_NOTIF 0x81
#define EPNUM_CDC_0_OUT 0x02
#define EPNUM_CDC_0_IN 0x82

#define EPNUM_CDC_1_NOTIF 0x83
#define EPNUM_CDC_1_OUT 0x04
#define EPNUM_CDC_1_IN 0x84

#define EPNUM_CDC_2_NOTIF 0x85
#define EPNUM_CDC_2_OUT 0x06
#define EPNUM_CDC_2_IN 0x86

#define EPNUM_CDC_3_NOTIF 0x87
#define EPNUM_CDC_3_OUT 0x08
#define EPNUM_CDC_3_IN 0x88

#define EPNUM_CDC_4_NOTIF 0x89
#define EPNUM_CDC_4_OUT 0x0A
#define EPNUM_CDC_4_IN 0x8A

#define EPNUM_CDC_5_NOTIF 0x8B
#define EPNUM_CDC_5_OUT 0x0C
#define EPNUM_CDC_5_IN 0x8C

#define EPNUM_CDC_6_NOTIF 0x8D
#define EPNUM_CDC_6_OUT 0x0E
#define EPNUM_CDC_6_IN 0x8E
#endif

uint8_t const desc_fs_configuration[] =
	{
		// Config number, interface count, string index, total length, attribute, power in mA
		TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_INTERFACE_POWER, 200),

		// 1st CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size.
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_0, 4, EPNUM_CDC_0_NOTIF, 8, EPNUM_CDC_0_OUT, EPNUM_CDC_0_IN, 64),
#ifndef SIMPLE_COM_PORTS
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_1, 5, EPNUM_CDC_1_NOTIF, 8, EPNUM_CDC_1_OUT, EPNUM_CDC_1_IN, 64),
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_2, 6, EPNUM_CDC_2_NOTIF, 8, EPNUM_CDC_2_OUT, EPNUM_CDC_2_IN, 64),
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_3, 7, EPNUM_CDC_3_NOTIF, 8, EPNUM_CDC_3_OUT, EPNUM_CDC_3_IN, 64),
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_4, 8, EPNUM_CDC_4_NOTIF, 8, EPNUM_CDC_4_OUT, EPNUM_CDC_4_IN, 64),
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_5, 9, EPNUM_CDC_5_NOTIF, 8, EPNUM_CDC_5_OUT, EPNUM_CDC_5_IN, 64),
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_6, 10, EPNUM_CDC_6_NOTIF, 8, EPNUM_CDC_6_OUT, EPNUM_CDC_6_IN, 64),
#endif

};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
	(void)index; // for multiple configurations

	return desc_fs_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const *string_desc_arr[] =
	{
		(const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
		"Pacom Systems Pty. Ltd.",			// 1: Manufacturer
		"S1000 Sniffer/Debugger",   // 2: Product
		"NULL",						// 3: Serials, should use chip ID
		"S1000 UART",				// 4: CDC Interface
		"BLE Chip",					// 5: CDC Interface
		"S1000 OSDP 1 TX",			// 6: CDC Interface
		"S1000 OSDP 1 RX",			// 7: CDC Interface
		"S1000 OSDP 2 TX",			// 8: CDC Interface
		"S1000 OSDP 2 RX",			// 9: CDC Interface
		"Command Console",				// A: CDC Interface
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	(void)langid;

	uint8_t chr_count;

	if (index == 0)
	{
		memcpy(&_desc_str[1], string_desc_arr[0], 2);
		chr_count = 1;
	}
	else if (index == 3)
	{
		//Serial Number
		chr_count = 31;
		char picoUUID[32];

		pico_get_unique_board_id_string(picoUUID, 32);

		for (uint8_t i = 0; i < chr_count; i++)
		{
			_desc_str[1 + i] = picoUUID[i];
		}
	}
	else
	{
		// Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
		// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

		if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
			return NULL;

		const char *str = string_desc_arr[index];

		// Cap at max char
		chr_count = (uint8_t)strlen(str);
		if (chr_count > 31)
			chr_count = 31;

		//// Convert ASCII string into UTF-16
		for (uint8_t i = 0; i < chr_count; i++)
		{
			_desc_str[1 + i] = str[i];
		}
	}

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

	return _desc_str;
}