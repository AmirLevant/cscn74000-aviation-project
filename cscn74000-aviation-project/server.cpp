#include <windows.networking.sockets.h>
#include <iostream>
#include "../cscn74000-aviation-project-client/packet.h"

#pragma comment(lib, "Ws2_32.lib")



void main()
{
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	//create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return;
	}

	//binds socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	std::cout << "Waiting for client packet" << std::endl;

	while (1)
	{
		char RxBuffer[128] = {};	//	Buffer for receiving data
		sockaddr_in CltAddr;		//	Client Address for sending resp
		int length_recvfrom_parameter = sizeof(struct sockaddr_in);

		recvfrom(ServerSocket, RxBuffer, sizeof(RxBuffer), 0, (SOCKADDR*)&CltAddr, &length_recvfrom_parameter);
		Packet RxPkt(uint8_t*(RxBuffer));


	}



	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources
}