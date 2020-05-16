/**
 * @file ltcparams.h
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#if !defined(__clang__)	// Needed for compiling on MacOS
 #pragma GCC push_options
 #pragma GCC optimize ("Os")
#endif

#include <stdint.h>
#include <string.h>
#ifndef NDEBUG
 #include <stdio.h>
#endif
#include <time.h>
#include <cassert>

#include "ltcparams.h"

#include "ltcparamsconst.h"

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

LtcParams::LtcParams(LtcParamsStore *pLtcParamsStore): m_pLTcParamsStore(pLtcParamsStore) {
	memset(&m_tLtcParams, 0, sizeof(struct TLtcParams));

	time_t ltime = time(0);
	struct tm *tm = localtime(&ltime);

	m_tLtcParams.tSource = LTC_READER_SOURCE_LTC;
	m_tLtcParams.nYear = tm->tm_year - 100;
	m_tLtcParams.nMonth = tm->tm_mon + 1;
	m_tLtcParams.nDay = tm->tm_mday;
	m_tLtcParams.nFps = 25;
	m_tLtcParams.nStopFrame = m_tLtcParams.nFps - 1;
	m_tLtcParams.nStopSecond = 59;
	m_tLtcParams.nStopMinute = 29;
	m_tLtcParams.nStopHour = 23;
	m_tLtcParams.nOscPort = 8000;
}

LtcParams::~LtcParams(void) {
}

bool LtcParams::Load(void) {
	m_tLtcParams.nSetList = 0;

	ReadConfigFile configfile(LtcParams::staticCallbackFunction, this);

	if (configfile.Read(LtcParamsConst::FILE_NAME)) {
		// There is a configuration file
		if (m_pLTcParamsStore != 0) {
			m_pLTcParamsStore->Update(&m_tLtcParams);
		}
	} else if (m_pLTcParamsStore != 0) {
		m_pLTcParamsStore->Copy(&m_tLtcParams);
	} else {
		return false;
	}

	return true;
}

void LtcParams::Load(const char* pBuffer, uint32_t nLength) {
	assert(pBuffer != 0);
	assert(nLength != 0);
	assert(m_pLTcParamsStore != 0);

	if (m_pLTcParamsStore == 0) {
		return;
	}

	m_tLtcParams.nSetList = 0;

	ReadConfigFile config(LtcParams::staticCallbackFunction, this);

	config.Read(pBuffer, nLength);

	m_pLTcParamsStore->Update(&m_tLtcParams);
}

void LtcParams::HandleDisabledOutput(const char *pLine, const char *pKeyword, unsigned nMaskDisabledOutputs) {
	uint8_t value8;

	if (Sscan::Uint8(pLine, pKeyword, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nDisabledOutputs |= nMaskDisabledOutputs;
			m_tLtcParams.nSetList |= LtcParamsMask::DISABLED_OUTPUTS;
		} else {
			m_tLtcParams.nDisabledOutputs &= ~(nMaskDisabledOutputs);
		}
	}
}

void LtcParams::callbackFunction(const char* pLine) {
	assert(pLine != 0);

	uint8_t value8;
	uint16_t value16;
	char source[16];
	uint8_t len = sizeof(source);

	if (Sscan::Char(pLine, LtcParamsConst::SOURCE, source, &len) == SSCAN_OK) {
		source[len] = '\0';
		m_tLtcParams.tSource = GetSourceType(source);
		m_tLtcParams.nSetList |= LtcParamsMask::SOURCE;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::AUTO_START, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nAutoStart = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::AUTO_START;
		} else {
			m_tLtcParams.nAutoStart = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::AUTO_START;
		}
	}

	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_DISPLAY, LtcParamsMaskDisabledOutputs::DISPLAY);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_MAX7219, LtcParamsMaskDisabledOutputs::MAX7219);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_LTC, LtcParamsMaskDisabledOutputs::LTC);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_MIDI, LtcParamsMaskDisabledOutputs::MIDI);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_ARTNET, LtcParamsMaskDisabledOutputs::ARTNET);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_TCNET, LtcParamsMaskDisabledOutputs::TCNET);
	HandleDisabledOutput(pLine, LtcParamsConst::DISABLE_RTPMIDI, LtcParamsMaskDisabledOutputs::RTPMIDI);

	if (Sscan::Uint8(pLine, LtcParamsConst::SHOW_SYSTIME, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nShowSysTime = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::SHOW_SYSTIME;
		} else {
			m_tLtcParams.nShowSysTime = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::SHOW_SYSTIME;
		}
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::DISABLE_TIMESYNC, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nDisableTimeSync = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::DISABLE_TIMESYNC;
		} else {
			m_tLtcParams.nDisableTimeSync = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::DISABLE_TIMESYNC;
		}
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::YEAR, &value8) == SSCAN_OK) {
		if (value8 >= 19) {
			m_tLtcParams.nYear = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::YEAR;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::MONTH, &value8) == SSCAN_OK) {
		if ((value8 >= 1) && (value8 <= 12)) {
			m_tLtcParams.nMonth = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::MONTH;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::DAY, &value8) == SSCAN_OK) {
		if ((value8 >= 1) && (value8 <= 31)) {
			m_tLtcParams.nDay = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::DAY;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::NTP_ENABLE, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nEnableNtp = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::ENABLE_NTP;
		} else {
			m_tLtcParams.nEnableNtp = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::ENABLE_NTP;
		}
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::FPS, &value8) == SSCAN_OK) {
		if ((value8 >= 24) && (value8 <= 30)) {
			m_tLtcParams.nFps = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::FPS;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::START_FRAME, &value8) == SSCAN_OK) {
		if (value8 <= 30) {
			m_tLtcParams.nStartFrame = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::START_FRAME;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::START_SECOND, &value8) == SSCAN_OK) {
		if (value8 <= 59) {
			m_tLtcParams.nStartSecond = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::START_SECOND;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::START_MINUTE, &value8) == SSCAN_OK) {
		if (value8 <= 59) {
			m_tLtcParams.nStartMinute = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::START_MINUTE;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::START_HOUR, &value8) == SSCAN_OK) {
		if (value8 <= 23) {
			m_tLtcParams.nStartHour = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::START_HOUR;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::STOP_FRAME, &value8) == SSCAN_OK) {
		if (value8 <= 30) {
			m_tLtcParams.nStopFrame = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::STOP_FRAME;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::STOP_SECOND, &value8) == SSCAN_OK) {
		if (value8 <= 59) {
			m_tLtcParams.nStopSecond = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::STOP_SECOND;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::STOP_MINUTE, &value8) == SSCAN_OK) {
		if (value8 <= 59) {
			m_tLtcParams.nStopMinute = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::STOP_MINUTE;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::STOP_HOUR, &value8) == SSCAN_OK) {
		if (value8 <= 99) {
			m_tLtcParams.nStopHour = value8;
			m_tLtcParams.nSetList |= LtcParamsMask::STOP_HOUR;
		}
		return;
	}

#if 0
	if (Sscan::Uint8(pLine, LtcParamsConst::SET_DATE, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nSetDate = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::SET_DATE;
		} else {
			m_tLtcParams.nSetDate = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::SET_DATE;
		}
	}
#endif

	if (Sscan::Uint8(pLine, LtcParamsConst::OSC_ENABLE, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nEnableOsc = 1;
			m_tLtcParams.nSetList |= LtcParamsMask::ENABLE_OSC;
		} else {
			m_tLtcParams.nEnableOsc = 0;
			m_tLtcParams.nSetList &= ~LtcParamsMask::ENABLE_OSC;
		}
	}

	if (Sscan::Uint16(pLine, LtcParamsConst::OSC_PORT, &value16) == SSCAN_OK) {
		if (value16 > 1023) {
			m_tLtcParams.nOscPort = value16;
			m_tLtcParams.nSetList |= LtcParamsMask::OSC_PORT;
		}
		return;
	}

	if (Sscan::Uint8(pLine, LtcParamsConst::WS28XX_ENABLE, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tLtcParams.nEnableWS28xx = 1;
			m_tLtcParams.nDisabledOutputs |= LtcParamsMaskDisabledOutputs::MAX7219;
#if !defined(USE_SPI_DMA)
			m_tLtcParams.nDisabledOutputs |= LtcParamsMaskDisabledOutputs::LTC;		// TODO Temporarily code until SPI DMA has been implemented
#endif
			m_tLtcParams.nSetList |= LtcParamsMask::ENABLE_WS28XX;
			m_tLtcParams.nSetList |= LtcParamsMask::DISABLED_OUTPUTS;
		} else {
			m_tLtcParams.nEnableWS28xx = 0;
			if (!isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::MAX7219)) {
				m_tLtcParams.nDisabledOutputs &= ~LtcParamsMaskDisabledOutputs::MAX7219;
			}
#if !defined(USE_SPI_DMA)
			if (!isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::LTC)) {			// TODO Temporarily code until SPI DMA has been implemented
				m_tLtcParams.nDisabledOutputs &= ~LtcParamsMaskDisabledOutputs::LTC;	// TODO Temporarily code until SPI DMA has been implemented
			}																// TODO Temporarily code until SPI DMA has been implemented
#endif
			m_tLtcParams.nSetList &= ~LtcParamsMask::ENABLE_WS28XX;
		}
	}
}

void LtcParams::Dump(void) {
#ifndef NDEBUG
	if (m_tLtcParams.nSetList == 0) {
		return;
	}

	printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, LtcParamsConst::FILE_NAME);

	if (isMaskSet(LtcParamsMask::SOURCE)) {
		printf(" %s=%d [%s]\n", LtcParamsConst::SOURCE, m_tLtcParams.tSource, GetSourceType(static_cast<TLtcReaderSource>(m_tLtcParams.tSource)));
	}

	if (isMaskSet(LtcParamsMask::AUTO_START)) {
		printf(" %s=%d\n", LtcParamsConst::AUTO_START, m_tLtcParams.nAutoStart);
	}

	if (isMaskSet(LtcParamsMask::DISABLED_OUTPUTS)) {
		printf(" Disabled outputs %.2x:\n", m_tLtcParams.nDisabledOutputs);

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::DISPLAY)) {
			printf("  Display\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::MAX7219)) {
			printf("  Max7219\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::MIDI)) {
			printf("  MIDI\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::RTPMIDI)) {
			printf("  RtpMIDI\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::ARTNET)) {
			printf("  Art-Net\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::TCNET)) {
			printf("  TCNet\n");
		}

		if (isDisabledOutputMaskSet(LtcParamsMaskDisabledOutputs::LTC)) {
			printf("  LTC\n");
		}
	}

	if (isMaskSet(LtcParamsMask::YEAR)) {
		printf(" %s=%d\n", LtcParamsConst::YEAR, m_tLtcParams.nYear);
	}

	if (isMaskSet(LtcParamsMask::MONTH)) {
		printf(" %s=%d\n", LtcParamsConst::MONTH, m_tLtcParams.nMonth);
	}

	if (isMaskSet(LtcParamsMask::DAY)) {
		printf(" %s=%d\n", LtcParamsConst::DAY, m_tLtcParams.nDay);
	}

	if (isMaskSet(LtcParamsMask::ENABLE_NTP)) {
		printf(" NTP is enabled\n");
	}

	if (isMaskSet(LtcParamsMask::FPS)) {
		printf(" %s=%d\n", LtcParamsConst::FPS, m_tLtcParams.nFps);
	}

	if (isMaskSet(LtcParamsMask::START_FRAME)) {
		printf(" %s=%d\n", LtcParamsConst::START_FRAME, m_tLtcParams.nStartFrame);
	}

	if (isMaskSet(LtcParamsMask::START_SECOND)) {
		printf(" %s=%d\n", LtcParamsConst::START_SECOND, m_tLtcParams.nStartSecond);
	}

	if (isMaskSet(LtcParamsMask::START_MINUTE)) {
		printf(" %s=%d\n", LtcParamsConst::START_MINUTE, m_tLtcParams.nStartMinute);
	}

	if (isMaskSet(LtcParamsMask::START_HOUR)) {
		printf(" %s=%d\n", LtcParamsConst::START_HOUR, m_tLtcParams.nStartHour);
	}

	if (isMaskSet(LtcParamsMask::STOP_FRAME)) {
		printf(" %s=%d\n", LtcParamsConst::STOP_FRAME, m_tLtcParams.nStopFrame);
	}

	if (isMaskSet(LtcParamsMask::STOP_SECOND)) {
		printf(" %s=%d\n", LtcParamsConst::STOP_SECOND, m_tLtcParams.nStopSecond);
	}

	if (isMaskSet(LtcParamsMask::STOP_MINUTE)) {
		printf(" %s=%d\n", LtcParamsConst::STOP_MINUTE, m_tLtcParams.nStopMinute);
	}

	if (isMaskSet(LtcParamsMask::STOP_HOUR)) {
		printf(" %s=%d\n", LtcParamsConst::STOP_HOUR, m_tLtcParams.nStopHour);
	}

#if 0
	if (isMaskSet(LtcParamsMask::SET_DATE)) {
		printf(" %s=%d\n", LtcParamsConst::SET_DATE, m_tLtcParams.nSetDate);
	}
#endif

	if (isMaskSet(LtcParamsMask::ENABLE_OSC)) {
		printf(" OSC is enabled\n");

		if (isMaskSet(LtcParamsMask::OSC_PORT)) {
			printf(" %s=%d\n", LtcParamsConst::OSC_PORT, m_tLtcParams.nOscPort);
		}
	}

	if (isMaskSet(LtcParamsMask::ENABLE_WS28XX)) {
		printf(" WS28xx is enabled\n");
	}
#endif
}

void LtcParams::StartTimeCodeCopyTo(TLtcTimeCode* ptStartTimeCode) {
	assert(ptStartTimeCode != 0);

	if ((isMaskSet(LtcParamsMask::START_FRAME)) || (isMaskSet(LtcParamsMask::START_SECOND)) || (isMaskSet(LtcParamsMask::START_MINUTE)) || (isMaskSet(LtcParamsMask::START_HOUR)) ) {
		memset(ptStartTimeCode, 0, sizeof(struct TLtcTimeCode));

		if (isMaskSet(LtcParamsMask::START_FRAME)) {
			ptStartTimeCode->nFrames = m_tLtcParams.nStartFrame;
		}

		if (isMaskSet(LtcParamsMask::START_SECOND)) {
			ptStartTimeCode->nSeconds = m_tLtcParams.nStartSecond;
		}

		if (isMaskSet(LtcParamsMask::START_MINUTE)) {
			ptStartTimeCode->nMinutes = m_tLtcParams.nStartMinute;
		}

		if (isMaskSet(LtcParamsMask::START_HOUR)) {
			ptStartTimeCode->nHours = m_tLtcParams.nStartHour;
		}
	} else {
		ptStartTimeCode->nFrames = m_tLtcParams.nStartFrame;
		ptStartTimeCode->nSeconds = m_tLtcParams.nStartSecond;
		ptStartTimeCode->nMinutes = m_tLtcParams.nStartMinute;
		ptStartTimeCode->nHours = m_tLtcParams.nStartHour;
	}

	ptStartTimeCode->nType = Ltc::GetType(m_tLtcParams.nFps);
}

void LtcParams::StopTimeCodeCopyTo(TLtcTimeCode* ptStopTimeCode) {
	assert(ptStopTimeCode != 0);

	if ((isMaskSet(LtcParamsMask::STOP_FRAME)) || (isMaskSet(LtcParamsMask::STOP_SECOND)) || (isMaskSet(LtcParamsMask::STOP_MINUTE)) || (isMaskSet(LtcParamsMask::STOP_HOUR)) ) {
		memset(ptStopTimeCode, 0, sizeof(struct TLtcTimeCode));

		if (isMaskSet(LtcParamsMask::STOP_FRAME)) {
			ptStopTimeCode->nFrames = m_tLtcParams.nStopFrame;
		}

		if (isMaskSet(LtcParamsMask::STOP_SECOND)) {
			ptStopTimeCode->nSeconds = m_tLtcParams.nStopSecond;
		}

		if (isMaskSet(LtcParamsMask::STOP_MINUTE)) {
			ptStopTimeCode->nMinutes = m_tLtcParams.nStopMinute;
		}

		if (isMaskSet(LtcParamsMask::STOP_HOUR)) {
			ptStopTimeCode->nHours = m_tLtcParams.nStopHour;
		}
	} else {
		ptStopTimeCode->nFrames = m_tLtcParams.nStopFrame;
		ptStopTimeCode->nSeconds = m_tLtcParams.nStopSecond;
		ptStopTimeCode->nMinutes = m_tLtcParams.nStopMinute;
		ptStopTimeCode->nHours = m_tLtcParams.nStopHour;
	}

	ptStopTimeCode->nType = Ltc::GetType(m_tLtcParams.nFps);
}

void LtcParams::staticCallbackFunction(void *p, const char *s) {
	assert(p != 0);
	assert(s != 0);

	(static_cast<LtcParams*>(p))->callbackFunction(s);
}
