// Copyright (c) 2010, Alan Ott, Signal 11 Software
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HIDAPI_H__
#define HIDAPI_H__

#include <wchar.h>
#include <stddef.h>

#ifdef _WIN32
      #define HID_API_EXPORT __declspec(dllexport)
      #define HID_API_CALL
#else
      #define HID_API_EXPORT
      #define HID_API_CALL
#endif

#define HID_API_EXPORT_CALL HID_API_EXPORT HID_API_CALL

#ifdef __cplusplus
extern "C" {
#endif

struct hid_device_;
typedef struct hid_device_ hid_device;

struct hid_device_info {
	char *path;
	unsigned short vendor_id;
	unsigned short product_id;
	wchar_t *serial_number;
	unsigned short release_number;
	wchar_t *manufacturer_string;
	wchar_t *product_string;
	unsigned short usage_page;
	unsigned short usage;
	int interface_number;
	struct hid_device_info *next;
};

int HID_API_EXPORT HID_API_CALL hid_init(void);
int HID_API_EXPORT HID_API_CALL hid_exit(void);

struct hid_device_info HID_API_EXPORT * HID_API_CALL hid_enumerate(unsigned short vendor_id, unsigned short product_id);
void HID_API_EXPORT HID_API_CALL hid_free_enumeration(struct hid_device_info *devs);

HID_API_EXPORT hid_device * HID_API_CALL hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number);
HID_API_EXPORT hid_device * HID_API_CALL hid_open_path(const char *path);

int HID_API_EXPORT HID_API_CALL hid_write(hid_device *dev, const unsigned char *data, size_t length);
int HID_API_EXPORT HID_API_CALL hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds);
int HID_API_EXPORT HID_API_CALL hid_read(hid_device *dev, unsigned char *data, size_t length);
int HID_API_EXPORT HID_API_CALL hid_set_nonblocking(hid_device *dev, int nonblock);
int HID_API_EXPORT HID_API_CALL hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length);
int HID_API_EXPORT HID_API_CALL hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length);
void HID_API_EXPORT HID_API_CALL hid_close(hid_device *dev);

int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen);
int HID_API_EXPORT_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen);
int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen);
int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen);
const wchar_t HID_API_EXPORT * HID_API_CALL hid_error(hid_device *dev);

#ifdef __cplusplus
}
#endif

#endif
