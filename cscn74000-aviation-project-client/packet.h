#pragma once
#include "helper_functions.h"
#include "plane.h"

#define PACKET_HEADER_SIZE 24
#define MAX_PACKET_SIZE 65507
#define MAX_READ_SIZE MAX_PACKET_SIZE - PACKET_HEADER_SIZE

class Packet
{
	uint32_t timestamp;
	uint32_t senderId;
	uint32_t destinationId;
	InteractionType interactionType;
	RequestType requestType;
	uint32_t transactionNum;
	uint16_t bodyLength;
	uint16_t checksum;
	uint8_t* body;

	uint16_t packetSize;

public:
	Packet(uint32_t sendId, uint32_t destId, uint32_t transNum, uint16_t length, uint8_t* data);
	Packet(uint32_t sendId, uint32_t destId, RequestType reqType, uint32_t transNum, uint16_t bodyLen, uint8_t* data);
	Packet(uint8_t* buffer);
	Packet(Plane plane);
	void Serialize(uint8_t* buffer);
	void convertToAckPacket();
	uint16_t calcChecksum();
	~Packet();
	uint32_t get_timestamp();
	uint32_t get_packetSize();
	InteractionType getInteractionType();
	RequestType getRequestType();
	uint32_t getSenderId();
	uint32_t getTransactionNum();
	uint8_t* getBody();
};