/**
 * @file tcnetreader.h
 *
 */
/* Copyright (C) 2019 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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

#ifndef TCNETREADER_H_
#define TCNETREADER_H_

#include "tcnettimecode.h"
#include "artnetnode.h"
#include "ltc.h"

class TCNetReader : public TCNetTimeCode {
public:
	TCNetReader(ArtNetNode* pNode, struct TLtcDisabledOutputs *pLtcDisabledOutputs);
	~TCNetReader(void);

	void Start(void);
	void Stop(void);

	void Run(void);

	void Handler(const struct TTCNetTimeCode *pTimeCode);

private:
	ArtNetNode *m_pNode;
	struct TLtcDisabledOutputs *m_ptLtcDisabledOutputs;
	uint32_t m_nTimeCodePrevious;
	TTimecodeTypes m_tTimeCodeTypePrevious;
	char m_aTimeCode[TC_CODE_MAX_LENGTH];
};


#endif /* TCNETREADER_H_ */