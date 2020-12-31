/**
 * @file widgetconfiguration.h
 *
 */
/* Copyright (C) 2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef WIDGETCONFIGURATION_H_
#define WIDGETCONFIGURATION_H_

#include <stdint.h>

#include "widget.h"

#if defined (H3)
#else
# include "../lib-hal/ff12c/ff.h"
#endif

#define DEVICE_TYPE_ID_LENGTH	2	///<

enum {
	WIDGET_MIN_BREAK_TIME = 9,
	WIDGET_DEFAULT_BREAK_TIME = 9,
	WIDGET_MAX_BREAK_TIME = 127
};

enum {
	WIDGET_MIN_MAB_TIME = 1,
	WIDGET_DEFAULT_MAB_TIME = 1,
	WIDGET_MAX_MAB_TIME = 127,
};

enum {
	WIDGET_DEFAULT_REFRESH_RATE = 40
} ;

typedef enum {
	WIDGET_DEFAULT_FIRMWARE_LSB = 4	///< x.4
} _firmware_version_lsb;


enum {
	FIRMWARE_NORMAL_DMX = 1,	///< Normal DMX firmware. Supports all messages except Send RDM (label=7), Send RDM Discovery Request(label=11) and receive RDM .
	FIRMWARE_RDM = 2,			///< RDM firmware. This enables the Widget to act as an RDM Controller.
	FIRMWARE_RDM_SNIFFER = 3	///< RDM Sniffer firmware. This is for use with the Openlighting RDM packet monitoring application.
};

struct _widget_params {
	uint8_t firmware_lsb;			///< Firmware version LSB. Valid range is 0 to 255.
	uint8_t firmware_msb;			///< Firmware version MSB. Valid range is 0 to 255.
	uint8_t break_time;				///< DMX output break time in 10.67 microsecond units. Valid range is 9 to 127.
	uint8_t mab_time;				///< DMX output Mark After Break time in 10.67 microsecond units. Valid range is 1 to 127.
	uint8_t refresh_rate;			///< DMX output rate in packets per second. Valid range is 1 to 40.
};

struct _widget_params_data {
	/*@shared@*/uint8_t *data;
	uint8_t length;
};

struct WidgetConfiguration {
	static void Get(struct _widget_params *widget_params) {
		widget_params->break_time = s_nBreakTime;
		widget_params->firmware_lsb = s_nFirmwareLsb;
		widget_params->firmware_msb = s_nFirmwareMsb;
		widget_params->mab_time = s_nMabTime;
		widget_params->refresh_rate = s_nRefreshRate;
	}

	static void GetTypeId(struct _widget_params_data *info) {
		info->data = const_cast<uint8_t*>(s_aDeviceTypeId);
		info->length = DEVICE_TYPE_ID_LENGTH;
	}

	static void Store(const struct _widget_params *widget_params);

	static void SetMode(widget::Mode tMode);
	static void SetBreakTime(uint8_t nBreakTime);
	static void SetMabTime(uint8_t nMabTime);
	static void SetRefreshRate(uint8_t nRefreshRate);
	static void SetThrottle(uint8_t nThrottle);

private:
#if defined (H3)
#else
	static void UpdateConfigFile();
	static void ProcessLineUpdate(const char *pLine, FIL *file_object_wr);
#endif

private:
	static uint8_t s_aDeviceTypeId[DEVICE_TYPE_ID_LENGTH];
	static uint8_t s_nFirmwareLsb;
	static uint8_t s_nFirmwareMsb;
	static uint8_t s_nBreakTime;
	static uint8_t s_nMabTime;
	static uint8_t s_nRefreshRate;
};

#endif /* WIDGETCONFIGURATION_H_ */
