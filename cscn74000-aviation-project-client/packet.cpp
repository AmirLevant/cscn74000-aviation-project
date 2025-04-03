#include "packet.h"

Packet::Packet(uint32_t sendId, uint32_t destId, uint32_t transNum, uint16_t bodyLen, uint8_t* data)
{
	timestamp = get_timestamp();

	senderId = sendId;

	destinationId = destId;

	interactionType = InteractionType::Telemetry;

	requestType = RequestType::Non_Request;

	transactionNum = transNum;

	bodyLength = bodyLen;

	if (bodyLength > 0)
	{
		body = new uint8_t[bodyLength];
		memcpy(body, data, bodyLength);
		delete[] data;
	}
	else
		body = nullptr;

	checksum = calcChecksum();
}

Packet::Packet(uint32_t sendId, uint32_t destId, RequestType reqType, uint32_t transNum, uint16_t bodyLen, uint8_t* data)
{
	timestamp = get_timestamp();

	senderId = sendId;

	destinationId = destId;

	interactionType = InteractionType::Request;

	requestType = reqType;

	transactionNum = transNum;

	bodyLength = bodyLen;

	if (bodyLength > 0)
	{
		body = new uint8_t[bodyLength];
		memcpy(body, data, bodyLength);
		delete[] data;
	}
	else
		body = nullptr;

	checksum = calcChecksum();
}

Packet::Packet(uint8_t* buffer)
{
	int offset = 0;

	memcpy(&timestamp, buffer + offset, sizeof(timestamp));
	offset += sizeof(timestamp);

	memcpy(&senderId, buffer + offset, sizeof(senderId));
	offset += sizeof(senderId);

	memcpy(&destinationId, buffer + offset, sizeof(destinationId));
	offset += sizeof(destinationId);

	memcpy(&interactionType, buffer + offset, sizeof(interactionType));
	offset += sizeof(interactionType);

	memcpy(&requestType, buffer + offset, sizeof(requestType));
	offset += sizeof(requestType);

	memcpy(&transactionNum, buffer + offset, sizeof(transactionNum));
	offset += sizeof(transactionNum);

	memcpy(&bodyLength, buffer + offset, sizeof(bodyLength));
	offset += sizeof(bodyLength);

	memcpy(&checksum, buffer + offset, sizeof(checksum));
	offset += sizeof(checksum);

	if (bodyLength > 0)
	{
		body = new uint8_t[bodyLength];
		memcpy(body, buffer + offset, bodyLength);
	}
	else
		body = nullptr;

	offset += bodyLength;
}

// Create a packet which sends the plane's basic telemetry information. Not to be used for sending the flag.
Packet::Packet(Plane plane)
{
	timestamp = get_timestamp();

	senderId = plane.getId();

	destinationId = 0;

	interactionType = InteractionType::Telemetry;

	requestType = RequestType::Non_Request;

	transactionNum = plane.getAndIncreaseTransactionNum();

	bodyLength = PLANE_SERIALIZATION_SIZE;

	if (bodyLength > 0)
	{
		body = new uint8_t[bodyLength];
		// Serializes the necessary plane telemetry into the body of the packet message
		plane.serialize(body);
	}
	else
		body = nullptr;

	checksum = calcChecksum();
}

void Packet::Serialize(uint8_t* buffer)
{
	int offset = 0;

	memcpy(buffer + offset, &timestamp, sizeof(timestamp));
	offset += sizeof(timestamp);

	memcpy(buffer + offset, &senderId, sizeof(senderId));
	offset += sizeof(senderId);

	memcpy(buffer + offset, &destinationId, sizeof(destinationId));
	offset += sizeof(destinationId);

	memcpy(buffer + offset, &interactionType, sizeof(interactionType));
	offset += sizeof(interactionType);

	memcpy(buffer + offset, &requestType, sizeof(requestType));
	offset += sizeof(requestType);

	memcpy(buffer + offset, &transactionNum, sizeof(transactionNum));
	offset += sizeof(transactionNum);

	memcpy(buffer + offset, &bodyLength, sizeof(bodyLength));
	offset += sizeof(bodyLength);

	memcpy(buffer + offset, &checksum, sizeof(checksum));
	offset += sizeof(checksum);

	if (bodyLength > 0)
	{
		memcpy(buffer + offset, body, bodyLength);
		offset += bodyLength;
	}
}

void Packet::convertToAckPacket()
{
	timestamp = get_timestamp();

	uint32_t holder = senderId;

	senderId = destinationId;

	destinationId = holder;

	interactionType = InteractionType::Response;

	requestType = RequestType::Non_Request;

	bodyLength = 0;

	body = nullptr;

	checksum = calcChecksum(); 
}

uint16_t Packet::calcChecksum()
{
	uint16_t chksum = 0;
	uint8_t* dataPointer;

	dataPointer = (uint8_t*)&timestamp;
	for (int i = 0; i < sizeof(timestamp); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&senderId;
	for (int i = 0; i < sizeof(senderId); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&destinationId;
	for (int i = 0; i < sizeof(destinationId); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&interactionType;
	for (int i = 0; i < sizeof(interactionType); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&requestType;
	for (int i = 0; i < sizeof(requestType); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&transactionNum;
	for (int i = 0; i < sizeof(transactionNum); i++)
		chksum += *(dataPointer + i);

	dataPointer = (uint8_t*)&bodyLength;
	for (int i = 0; i < sizeof(bodyLength); i++)
		chksum += *(dataPointer + i);

	if (bodyLength > 0)
	{
		dataPointer = (uint8_t*)body;
		for (int i = 0; i < bodyLength; i++)
			chksum += *(dataPointer + i);
	}

	return ~chksum;
}

Packet::~Packet()
{
	if (bodyLength > 0 && body != nullptr)
	{
		delete[] body;
	}
}

uint32_t Packet::get_timestamp()
{
	const auto p1 = std::chrono::system_clock::now();

	return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
		p1.time_since_epoch()).count());
}

uint32_t Packet::get_packetSize()
{
	return PACKET_HEADER_SIZE + bodyLength;
}

InteractionType Packet::getInteractionType()
{
	return interactionType;
}

RequestType Packet::getRequestType()
{
	return requestType;
}

uint32_t Packet::getSenderId()
{
	return senderId;
}

uint32_t Packet::getTransactionNum()
{
	return transactionNum;
}

uint8_t* Packet::getBody()
{
	return body;
}
