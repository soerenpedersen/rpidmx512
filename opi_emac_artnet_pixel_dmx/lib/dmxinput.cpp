/**
 * @file dmxinput.cpp
 *
 */
/* Copyright (C) 2020-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include <cstdint>
#include <cassert>

#include "dmxinput.h"
#include "dmx.h"

#include "debug.h"

using namespace dmx;
using namespace dmxsingle;

uint8_t DmxInput::s_nStarted;

static constexpr bool is_started(const uint8_t v, const uint8_t p) {
	return (v & (1U << p)) == (1U << p);
}

DmxInput::DmxInput() {
	DEBUG_ENTRY

	Stop(0);

	DEBUG_EXIT
}

void DmxInput::Start(uint8_t nPortIndex) {
	DEBUG_ENTRY

	assert(nPortIndex < CHAR_BIT);

	DEBUG_PRINTF("nPortIndex=%d", nPortIndex);

	if (is_started(s_nStarted, nPortIndex)) {
		DEBUG_EXIT
		return;
	}

	s_nStarted = static_cast<uint8_t>(s_nStarted | (1U << nPortIndex));

	SetPortDirection(nPortIndex, PortDirection::INP, true);

	DEBUG_EXIT
}

void DmxInput::Stop(uint8_t nPortIndex) {
	DEBUG_ENTRY

	assert(nPortIndex < CHAR_BIT);

	DEBUG_PRINTF("nPortIndex=%d -> %u", nPortIndex, is_started(s_nStarted, nPortIndex));

	if (!is_started(s_nStarted, nPortIndex)) {
		DEBUG_EXIT
		return;
	}

	s_nStarted = static_cast<uint8_t>(s_nStarted & ~(1U << nPortIndex));

	SetPortDirection(nPortIndex, PortDirection::INP, false);

	DEBUG_EXIT
}

const uint8_t *DmxInput::Handler(__attribute__((unused)) uint8_t nPortIndex, uint32_t& nLength, uint32_t &nUpdatesPerSecond) {
	const auto *pDmx = GetDmxAvailable();

	nUpdatesPerSecond = GetUpdatesPerSecond();

	if (pDmx != nullptr) {
		const auto *dmx_statistics = reinterpret_cast<const struct Data*>(pDmx);
		nLength = dmx_statistics->Statistics.nSlotsInPacket;
		return (pDmx + 1);
	}

	nLength = 0;
	return nullptr;
}
