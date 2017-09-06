/*
 * artnetpolltable.cpp
 *
 *  Created on: Aug 11, 2017
 *      Author: pi
 */


#include <stdbool.h>
#include <stdio.h>

#if defined (__linux__) || defined (__CYGWIN__)
#include <string.h>
#else
#include "util.h"
#endif

#include "artnetpolltable.h"

#include "packets.h"

#define IP2STR(addr) (uint8_t)(addr & 0xFF), (uint8_t)((addr >> 8) & 0xFF), (uint8_t)((addr >> 16) & 0xFF), (uint8_t)((addr >> 24) & 0xFF)
#define IPSTR "%d.%d.%d.%d"

#define MAC2STR(mac)	(int)(mac[0]),(int)(mac[1]),(int)(mac[2]),(int)(mac[3]), (int)(mac[4]), (int)(mac[5])
#define MACSTR "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x"

union uip {
	uint32_t u32;
	uint8_t u8[4];
} static ip;

ArtNetPollTable::ArtNetPollTable(void) : m_bIsChanged(false), m_nEntries(0), m_nLastUpdate(0) {
	m_pPollTable = new TArtNetNodeEntry[255];
}

ArtNetPollTable::~ArtNetPollTable(void) {
	delete[] m_pPollTable;
}

bool ArtNetPollTable::isChanged(void) {
	return m_bIsChanged;
}

const uint8_t ArtNetPollTable::GetEntries(void) {
	return m_nEntries;
}

bool ArtNetPollTable::Add(const struct TArtPollReply *pPollReply) {
	bool bFound = false;
	uint8_t i;

	m_nLastUpdate = time(NULL);

	memcpy(ip.u8, pPollReply->IPAddress, 4);

	for (i = 0; i < m_nEntries; i++) {
		if (m_pPollTable[i].IPAddress == ip.u32) {
			bFound = true;
			break;
		}
	}

	if(bFound) {
		m_bIsChanged = false;
	} else {
		m_nEntries++;
		m_bIsChanged = true;
		m_pPollTable[i].IpProg.IPAddress = 0;
		m_pPollTable[i].IpProg.SubMask = 0;
		m_pPollTable[i].IpProg.Status = 0;
	}

	m_pPollTable[i].IPAddress = ip.u32;
	memcpy(m_pPollTable[i].Mac, pPollReply->MAC, ARTNET_MAC_SIZE);
	memcpy(m_pPollTable[i].ShortName, pPollReply->ShortName, ARTNET_SHORT_NAME_LENGTH);
	memcpy(m_pPollTable[i].LongName, pPollReply->LongName, ARTNET_LONG_NAME_LENGTH);
	m_pPollTable[i].Status1 = pPollReply->Status1;
	m_pPollTable[i].Status2 = pPollReply->Status2;
	m_pPollTable[i].LastUpdate = m_nLastUpdate;

	return bFound;
}

bool ArtNetPollTable::Add(const struct TArtIpProgReply *pIpProgReply) {
	bool bFound = false;
	uint8_t i;

	memcpy(ip.u8, &pIpProgReply->ProgIpHi, 4);

	for (i = 0; i < m_nEntries; i++) {
		if (m_pPollTable[i].IPAddress == ip.u32) {
			bFound = true;
			break;
		}
	}

	m_pPollTable[i].IpProg.IPAddress = ip.u32;
	memcpy(ip.u8, &pIpProgReply->ProgSmHi, 4);
	m_pPollTable[i].IpProg.SubMask = ip.u32;
	m_pPollTable[i].IpProg.Status = pIpProgReply->Status;

	return bFound;
}

void ArtNetPollTable::Dump(void) {
	printf("Entries : %d\n", m_nEntries);

	for (uint8_t i = 0; i < m_nEntries; i++) {
		printf("\t" IPSTR " [" MACSTR "] %.18s:%.64s:%x:%x:%d\n", IP2STR(m_pPollTable[i].IPAddress), MAC2STR(m_pPollTable[i].Mac), m_pPollTable[i].ShortName, m_pPollTable[i].LongName, m_pPollTable[i].Status1, m_pPollTable[i].Status2, (int)(m_nLastUpdate - m_pPollTable[i].LastUpdate));
		printf("\t\t" IPSTR IPSTR "\n", IP2STR(m_pPollTable[i].IpProg.IPAddress), IP2STR(m_pPollTable[i].IpProg.SubMask));
	}

	m_bIsChanged = false;
}

bool ArtNetPollTable::GetEntry(const uint8_t nEntry, struct TArtNetNodeEntry *pEntry) {
	if ((pEntry == 0) || (nEntry == 0)) {
		return false;
	}

	memcpy((void *)pEntry, (void *)&m_pPollTable[nEntry-1], sizeof(struct TArtNetNodeEntry));

	return true;
}
