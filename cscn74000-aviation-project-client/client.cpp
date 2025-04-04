#include <fstream>
#include "packet.h"
#include "plane.h"

#pragma comment(lib, "Ws2_32.lib")

void sendFlag(Plane plane);
int waitForResponse(uint32_t transNum);

SOCKET ClientSocket;
sockaddr_in SvrAddr;

int main(int argc, char* argv[])
{
	srand(time(0));

	// Check if exactly one command line argument is provided
	if (argc != 2) {
		std::cout << "Error: Program requires exactly one argument - a positive number for plane ID" << std::endl;
		return 1;
	}
	// Parsing and validating the plane ID
	uint32_t planeId;
	try {
		planeId = std::stoi(argv[1]);
		if (planeId <= 0) {
			std::cout << "Error: Plane ID must be a positive number" << std::endl;
			return 1;
		}
	}
	catch (const std::exception&) {
		std::cout << "Error: Invalid input. Plane ID must be a positive number" << std::endl;
		return 1;
	}

	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 1;
	}

	//initializes socket.
	ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	//Connect socket to specified server
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address

	// Set a 100ms timeout for receiving data
	DWORD timeout = 100;  // Timeout in milliseconds
	if (setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
		std::cerr << "setsockopt failed: " << WSAGetLastError() << "\n";
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}


	// Plane object created with the id from the command line
	Plane plane(planeId);
	Packet* firstPkt = new Packet(planeId, 0, RequestType::InitiateConnection, plane.getAndIncreaseTransactionNum(), 0, nullptr);

	uint8_t* TxBuffer = new uint8_t[firstPkt->get_packetSize()];
	firstPkt->Serialize(TxBuffer);

	sendto(ClientSocket, (const char*)(TxBuffer), firstPkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
	delete[] TxBuffer;
	
	int timeWaited;
	timeWaited = waitForResponse(plane.getCurrentTransactionNum());

	int timeToSleep = 1000 - timeWaited;
	if (timeToSleep > 0)
		Sleep(1000 - timeWaited);

	bool sentFlag = false;

	while (true)
	{
		plane.decreaseDistance(1);

		if (plane.getDistanceFromGround() <= 300 && sentFlag == false)
		{
			sendFlag(plane);
			sentFlag = true;
		}
		Packet* infoPkt = new Packet(&plane);

		TxBuffer = new uint8_t[infoPkt->get_packetSize()];
		infoPkt->Serialize(TxBuffer);

		sendto(ClientSocket, (const char*)(TxBuffer), infoPkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
		delete[] TxBuffer;
		delete infoPkt;

		timeWaited = waitForResponse(plane.getCurrentTransactionNum());

		timeToSleep = 1000 - timeWaited;
		if (timeToSleep > 0)
			Sleep(1000 - timeWaited);
	}
	
	delete[] TxBuffer;
	delete firstPkt;

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

void sendFlag(Plane plane)
{
	std::ifstream ReadFile(plane.getFlagPath(), std::ifstream::binary);

	if (ReadFile.is_open())
	{
		char* buffer = new char[MAX_READ_SIZE];
		while (ReadFile.read(buffer, MAX_READ_SIZE) || ReadFile.gcount() > 0)
		{
			Packet* filePkt = new Packet(plane.getId(), 0, RequestType::SendingFileData, plane.getAndIncreaseTransactionNum(), ReadFile.gcount(), (uint8_t*)buffer);

			uint8_t* TxBuffer = new uint8_t[filePkt->get_packetSize()];
			filePkt->Serialize(TxBuffer);

			sendto(ClientSocket, (const char*)(TxBuffer), filePkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast

			delete filePkt;
			delete[] TxBuffer;
		}
	}
}

int waitForResponse(uint32_t transNum)
{
	auto beginningTime = std::chrono::high_resolution_clock::now();

	uint8_t* RxBuffer = new uint8_t[MAX_PACKET_SIZE];	//	Buffer for receiving data

	int length_recvfrom_parameter = sizeof(struct sockaddr_in);
	recvfrom(ClientSocket, (char*)RxBuffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&SvrAddr, &length_recvfrom_parameter);

	Packet* RxPkt = new Packet((uint8_t*)RxBuffer);

	if (RxPkt->getInteractionType() == InteractionType::Response && (RxPkt->getTransactionNum() != transNum))
	{
		std::cout << "Transaction number of acknowledgement packet does not match current local transaction number. Plane will disconnect from server and divert its course outside of ground station radius." << std::endl;
	}
	else if (RxPkt->getInteractionType() == InteractionType::Response && (RxPkt->getTransactionNum() == transNum))
	{
		std::cout << "Acknowledgement for packet number " << transNum << " received." << std::endl;
	}

	delete[] RxBuffer;
	delete RxPkt;

	return static_cast<int>(duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginningTime).count());
}