/**
 * @file networkparams.h
 *
 */
/* Copyright (C) 2017-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef NETWORKPARAMS_H_
#define NETWORKPARAMS_H_

#include <stdint.h>

#include "network.h"

struct TNetworkParams {
	uint32_t nSetList;
	uint32_t nLocalIp;
	uint32_t nNetmask;
	uint32_t nGatewayIp;
	uint32_t nNameServerIp;
	bool bIsDhcpUsed;
	char aHostName[NETWORK_HOSTNAME_SIZE];
	uint32_t nNtpServerIp;
	float fNtpUtcOffset;
};

struct NetworkParamsMask {
	static constexpr auto DHCP = (1U << 0);
	static constexpr auto IP_ADDRESS = (1U << 1);
	static constexpr auto NET_MASK = (1U << 2);
	static constexpr auto DEFAULT_GATEWAY = (1U << 3);
	static constexpr auto NAME_SERVER = (1U << 4);
	static constexpr auto HOSTNAME = (1U << 5);
	static constexpr auto NTP_SERVER = (1U << 6);
	static constexpr auto NTP_UTC_OFFSET = (1U << 7);
};

class NetworkParamsStore {
public:
	virtual ~NetworkParamsStore(void) {
	}

	virtual void Update(const struct TNetworkParams *pNetworkParams)=0;
	virtual void Copy(struct TNetworkParams *pNetworkParams)=0;
};

class NetworkParams {
public:
	NetworkParams(NetworkParamsStore *pNetworkParamsStore = 0);
	~NetworkParams(void);

	bool Load(void);
	void Load(const char *pBuffer, uint32_t nLength);

	void Builder(const struct TNetworkParams *ptNetworkParams, char *pBuffer, uint32_t nLength, uint32_t &nSize);
	void Save(char *pBuffer, uint32_t nLength, uint32_t &nSize);

	void Dump(void);

	bool isDhcpUsed(void) {
		return m_tNetworkParams.bIsDhcpUsed;
	}

	uint32_t GetIpAddress(void) {
		return m_tNetworkParams.nLocalIp;
	}

	uint32_t GetNetMask(void) {
		return m_tNetworkParams.nNetmask;
	}

	const char *GetHostName(void) {
		return m_tNetworkParams.aHostName;
	}

	uint32_t GetDefaultGateway(void) {
		return m_tNetworkParams.nGatewayIp;
	}

	uint32_t GetNameServer(void) {
		return m_tNetworkParams.nNameServerIp;
	}

	uint32_t GetNtpServer(void) {
		if (!isMaskSet(NetworkParamsMask::NTP_SERVER)) {
			return 0;
		}
		return m_tNetworkParams.nNtpServerIp;
	}

	float GetNtpUtcOffset(void) {
		if (!isMaskSet(NetworkParamsMask::NTP_UTC_OFFSET)) {
			return 0;
		}
		return m_tNetworkParams.fNtpUtcOffset;
	}

public:
    static void staticCallbackFunction(void *p, const char *s);

private:
    void callbackFunction(const char *s);
    bool isMaskSet(uint32_t nMask) {
    	return (m_tNetworkParams.nSetList & nMask) == nMask;
    }

private:
	NetworkParamsStore *m_pNetworkParamsStore;
	struct TNetworkParams m_tNetworkParams;
};

#endif /* NETWORKPARAMS_H_ */
