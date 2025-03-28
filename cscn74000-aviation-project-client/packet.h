#pragma once
#include "helper_functions.h"

class Packet
{
	uint32_t timestamp;
	uint32_t senderId;
	uint32_t destinationId;
	InteractionType interactionType;
	uint8_t requestType;
	uint32_t transactionNum;
	uint16_t bodyLength;
	uint16_t checksum;
	uint8_t* body;

	Packet(uint32_t sendId, uint32_t destId, uint32_t transNum, uint16_t length, uint8_t* data);
	Packet(uint32_t sendId, uint32_t destId, uint8_t reqType, uint32_t transNum, uint16_t bodyLen, uint8_t* data);
	Packet(uint8_t* buffer);
	void Serialize(uint8_t* buffer);
	Packet createAckPacket(Packet recPkt, uint32_t transNum);
	uint16_t calcChecksum();
	~Packet();
};