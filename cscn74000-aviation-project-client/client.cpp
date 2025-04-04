#include <fstream>
#include "packet.h"
#include "plane.h"

#pragma comment(lib, "Ws2_32.lib")

void sendFlag(Plane& plane);
void requestWeather(Plane& plane);
int waitForResponse(Plane& plane);
void transitionCSM(ClientStateMachine state, Plane& plane);

SOCKET ClientSocket;
sockaddr_in SvrAddr;
ClientStateMachine CSM = ClientStateMachine::Standby;

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

	std::cout << "Client State Machine staring in Standby state" << std::endl;

	// Plane object created with the id from the command line
	Plane plane(planeId);
	Packet* firstPkt = new Packet(planeId, 0, RequestType::InitiateConnection, plane.getAndIncreaseTransactionNum(), 0, nullptr);

	uint8_t* TxBuffer = new uint8_t[firstPkt->get_packetSize()];
	firstPkt->Serialize(TxBuffer);

	transitionCSM(ClientStateMachine::Ready, plane);

	sendto(ClientSocket, (const char*)(TxBuffer), firstPkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
	firstPkt->log(true);
	delete[] TxBuffer;
	
	int timeWaited;
	transitionCSM(ClientStateMachine::Initialization, plane);
	timeWaited = waitForResponse(plane);

	int timeToSleep = 1000 - timeWaited;
	if (timeToSleep > 0)
		Sleep(1000 - timeWaited);

	bool sentFlag = false;
	bool requestedWeather = false;

	while (plane.getDistanceFromGround() <= 350 && plane.getDistanceFromGround() > 0)
	{
		if (plane.getGoNoGo() == Go_NoGo::Go)
			plane.decreaseDistance(1);
		else
			plane.increaseDistance(1);


		if (plane.getDistanceFromGround() <= 300 && sentFlag == false)
		{
			sendFlag(plane);
			sentFlag = true;
		}

		if (plane.getDistanceFromGround() <= 275 && requestedWeather == false)
		{
			requestWeather(plane);
			requestedWeather = true;
		}
		std::cout << "Plane is " << plane.getDistanceFromGround() << " km away from ground station" << std::endl;

		Packet* infoPkt = new Packet(&plane);

		TxBuffer = new uint8_t[infoPkt->get_packetSize()];
		infoPkt->Serialize(TxBuffer);

		sendto(ClientSocket, (const char*)(TxBuffer), infoPkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
		infoPkt->log(true);

		delete[] TxBuffer;
		delete infoPkt;

		timeWaited = waitForResponse(plane);

		timeToSleep = 1000 - timeWaited;
		if (timeToSleep > 0)
			Sleep(1000 - timeWaited);
	}

	transitionCSM(ClientStateMachine::Standby, plane);

	std::cout << "Plane disconnected from ground control" << std::endl;
	
	delete firstPkt;

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

void sendFlag(Plane& plane)
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
			filePkt->log(true);

			delete filePkt;
			delete[] TxBuffer;

			int timeWaited = waitForResponse(plane);
		}
	}

	ReadFile.close();
}

void requestWeather(Plane& plane)
{
	Packet* weatherPkt = new Packet(plane.getId(), 0, RequestType::Request_Weather, plane.getAndIncreaseTransactionNum(), 0, nullptr);

	uint8_t* TxBuffer = new uint8_t[weatherPkt->get_packetSize()];
	weatherPkt->Serialize(TxBuffer);

	sendto(ClientSocket, (const char*)(TxBuffer), weatherPkt->get_packetSize(), 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)); // thats how the library defines UDP sending, we need to typecast
	weatherPkt->log(true);

	delete weatherPkt;
	delete[] TxBuffer;

	int timeWaited = waitForResponse(plane);
}

int waitForResponse(Plane& plane)
{
	auto beginningTime = std::chrono::high_resolution_clock::now();

	uint8_t* RxBuffer = new uint8_t[MAX_PACKET_SIZE];	//	Buffer for receiving data

	int length_recvfrom_parameter = sizeof(struct sockaddr_in);
	int bytesReceived = recvfrom(ClientSocket, (char*)RxBuffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&SvrAddr, &length_recvfrom_parameter);

	if (bytesReceived == SOCKET_ERROR)
	{
		std::cout << "Client timed out waiting for response from packet number " << plane.getCurrentTransactionNum() << std::endl;
		transitionCSM(ClientStateMachine::Failed, plane);
	}
	else
	{
		Packet* RxPkt = new Packet((uint8_t*)RxBuffer);
		RxPkt->log(true);

		if ((RxPkt->getInteractionType() == InteractionType::Response) && (RxPkt->getTransactionNum() != plane.getCurrentTransactionNum()))
		{
			std::cout << "Transaction number of acknowledgement packet does not match current local transaction number." << std::endl;
			transitionCSM(ClientStateMachine::Failed, plane);
		}
		else if ((RxPkt->getInteractionType() == InteractionType::Response) && (RxPkt->getTransactionNum() == plane.getCurrentTransactionNum()))
		{
			std::cout << "Acknowledgement for packet number " << plane.getCurrentTransactionNum() << " received." << std::endl;
			transitionCSM(ClientStateMachine::Connected, plane);
		}
		else if ((RxPkt->getInteractionType() == InteractionType::Request) && (RxPkt->getRequestType() == RequestType::Go_NoGo_Decision))
		{
			if ((Go_NoGo) * (RxPkt->getBody()) == Go_NoGo::Go)
			{
				std::cout << "Plane has been cleared to land. Proceeding to landing strip." << std::endl;
				plane.setGoNoGo(Go_NoGo::Go);
			}
			else
			{
				std::cout << "Plane has been told not to land. Plane will divert its course outside of ground station radius." << std::endl;
				plane.setGoNoGo(Go_NoGo::NoGo);
			}

		}
		else if ((RxPkt->getInteractionType() == InteractionType::Request) && (RxPkt->getRequestType() == RequestType::Request_Weather))
		{
			std::string weather((char*)RxPkt->getBody(), RxPkt->getBodyLength());
			std::cout << "Received weather info: " << weather << std::endl;
		}
		delete RxPkt;
	}

	delete[] RxBuffer;

	return static_cast<int>(duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beginningTime).count());
}

void transitionCSM(ClientStateMachine state, Plane& plane)
{
	if (CSM != ClientStateMachine::Failed && state == ClientStateMachine::Failed)
	{
		std::cout << "Client State Machine transitioning to Failed state. Plane will divert its course outside of ground station radius." << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::NoGo);
	}
	else if (CSM == ClientStateMachine::Standby && state == ClientStateMachine::Ready)
	{
		std::cout << "Client State Machine transitioning to Ready state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}
	else if (CSM == ClientStateMachine::Standby && (state != ClientStateMachine::Failed && state != ClientStateMachine::Ready && state != ClientStateMachine::Standby))
	{
		std::cout << "Client State Machine currently in Standby state and is unable to transition to requested state, now transitioning to failed state. Plane will divert its course outside of ground station radius." << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::NoGo);
	}
	else if (CSM == ClientStateMachine::Ready && state == ClientStateMachine::Initialization)
	{
		std::cout << "Client State Machine transitioning to Initialization state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}
	else if (CSM == ClientStateMachine::Ready && (state != ClientStateMachine::Failed && state != ClientStateMachine::Initialization && state != ClientStateMachine::Ready))
	{
		std::cout << "Client State Machine currently in Ready state and is unable to transition to requested state, now transitioning to failed state. Plane will divert its course outside of ground station radius." << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::NoGo);
	}
	else if (CSM == ClientStateMachine::Initialization && state == ClientStateMachine::Connected)
	{
		std::cout << "Client State Machine transitioning to Connected state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}
	else if (CSM == ClientStateMachine::Initialization && (state != ClientStateMachine::Failed && state != ClientStateMachine::Connected && state != ClientStateMachine::Initialization))
	{
		std::cout << "Client State Machine currently in Initialization state and is unable to transition to requested state, now transitioning to failed state. Plane will divert its course outside of ground station radius." << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::NoGo);
	}
	else if (CSM == ClientStateMachine::Connected && state == ClientStateMachine::Standby)
	{
		std::cout << "Client State Machine transitioning to Standby state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}
	else if (CSM == ClientStateMachine::Connected && (state != ClientStateMachine::Failed && state != ClientStateMachine::Standby && state != ClientStateMachine::Connected))
	{
		std::cout << "Client State Machine currently in Connected state and is unable to transition to requested state, now transitioning to failed state. Plane will divert its course outside of ground station radius." << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::NoGo);
	}
	else if (CSM == ClientStateMachine::Failed && state == ClientStateMachine::Connected)
	{
		transitionCSM(ClientStateMachine::Ready, plane);
		transitionCSM(ClientStateMachine::Initialization, plane);
		std::cout << "Client State Machine transitioning to Connected state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}
	else if (CSM == ClientStateMachine::Failed && state == ClientStateMachine::Ready)
	{
		std::cout << "Client State Machine transitioning to Ready state" << std::endl;
		CSM = state;
		plane.setGoNoGo(Go_NoGo::Go);
	}

}
