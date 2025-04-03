#include "packet.h"
#include "plane.h"

#pragma comment(lib, "Ws2_32.lib")

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
	std::cout << "C++ version: " << __cplusplus << std::endl;

	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 1;
	}

	//initializes socket.
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address

	std::string InputStr = "";
	Packet newPkt(1, 0, 1, 0, nullptr);

	// Plane object created with the id from the command line
	Plane plane(planeId);

	uint8_t* TxBuffer = new uint8_t[128];
	newPkt.Serialize(TxBuffer);

	while (true)
	{
		sendto(ClientSocket, (const char*)(TxBuffer), 128, 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
		Sleep(1000);
	}

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}