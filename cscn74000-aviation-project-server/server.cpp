#include "../cscn74000-aviation-project-client/packet.h"
#include "../cscn74000-aviation-project-client/helper_functions.h"
#include "../cscn74000-aviation-project-client/plane.h"
#include "ground_station.h"
#include <iostream>
#include <string>
#include <thread>
#include <cctype>
#include <cstdlib>  // For exit()
#pragma comment(lib, "Ws2_32.lib")

void dealWithRecvPacket(Packet* RxPkt, sockaddr_in CltAddr);
void dealWithRequestPacket(Packet* RxPkt);
void processFile(Packet* RxPkt);
void initializeConnection(Packet* RxPkt);
void sendAcknowledgement(Packet* TxPkt, sockaddr_in CltAddr);
void displayMenu();
int getMenuChoice();
void processMenuChoice(int choice);
void runMenu();

SOCKET ServerSocket;
sockaddr_in SvrAddr;

void main()
{
	// Seed the random number generator
	std::srand(std::time(0));
	// Initialize the ground station with random weather
	Ground_Station groundStation;
	// Display the ground station information (including weather)
	std::cout << "Server starting with initial conditions:" << std::endl;
	groundStation.displayInfo();
	std::cout << std::endl;
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;
	//create server socket
	ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return;
	}
	//binds socket to address

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

	// Create a separate thread for handling the menu
	std::thread menuThread(runMenu);

	// Main server loop for processing packets
	while (1)
	{
		uint8_t* RxBuffer = new uint8_t[MAX_PACKET_SIZE];	//	Buffer for receiving data
		sockaddr_in CltAddr;		//	Client Address for sending resp
		int length_recvfrom_parameter = sizeof(struct sockaddr_in);
		recvfrom(ServerSocket, (char*)RxBuffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&CltAddr, &length_recvfrom_parameter);
		Packet* RxPkt = new Packet((uint8_t*)RxBuffer);
		dealWithRecvPacket(RxPkt, CltAddr);
		//Plane RxPlane(RxPkt->getBody());
		delete[] RxBuffer;
		delete RxPkt;
	}

	// Wait for the menu thread to complete (though this won't normally be reached)
	if (menuThread.joinable()) {
		menuThread.join();
	}

	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources
}

void dealWithRecvPacket(Packet* RxPkt, sockaddr_in CltAddr)
{
	switch (RxPkt->getInteractionType())
	{
	case InteractionType::Telemetry:
		sendAcknowledgement(RxPkt, CltAddr);
		break;
	case InteractionType::Request:
		dealWithRequestPacket(RxPkt);
		sendAcknowledgement(RxPkt, CltAddr);
		break;
	case InteractionType::Response:
		break;
	}
}

void dealWithRequestPacket(Packet* RxPkt)
{
	switch (RxPkt->getRequestType())
	{
	case RequestType::InitiateConnection:
		initializeConnection(RxPkt);
		break;
	case RequestType::SendingFileData:
		processFile(RxPkt);
		break;
	}
}

void sendAcknowledgement(Packet* TxPkt, sockaddr_in CltAddr)
{
	TxPkt->convertToAckPacket();
	uint8_t* TxBuffer = new uint8_t[TxPkt->get_packetSize()];
	TxPkt->Serialize(TxBuffer);
	sendto(ServerSocket, (const char*)(TxBuffer), TxPkt->get_packetSize(), 0, (SOCKADDR*)&CltAddr, sizeof(CltAddr)); // thats how the library defines UDP sending, we need to typecast
}

void initializeConnection(Packet* RxPkt)
{
	std::string folderName = "../PlaneTelemetry/" + std::to_string(RxPkt->getSenderId());  // Convert int to string
	if (std::filesystem::create_directory(folderName)) {
		std::cout << "Folder created: " << folderName << std::endl;
	}
	else {
		std::cout << "Failed to create folder (it may already exist)." << std::endl;
	}
}

void processFile(Packet* RxPkt)
{
}

void displayMenu()
{
	std::cout << "\n===== Plane Control System =====\n";
	std::cout << "1. View what planes are currently connected \n";
	std::cout << "2. Modify flight parameters\n";
	std::cout << "3. Send flag manually\n";
	std::cout << "4. Exit simulation\n";
	std::cout << "Selection: ";
}

// Function to validate user input for menu selection
int getMenuChoice() {
	int choice = 0;
	std::string input;
	bool validInput = false;

	while (!validInput) {
		std::getline(std::cin, input);

		// Check if input is a single digit between 1-4
		if (input.length() == 1 && isdigit(input[0])) {
			choice = input[0] - '0';  // Convert char to int
			if (choice >= 1 && choice <= 4) {
				validInput = true;
			}
			else {
				std::cout << "Invalid selection. Please enter a number between 1 and 4.\n";
				std::cout << "Selection: ";
			}
		}
		else {
			std::cout << "Invalid input. Please enter a number between 1 and 4.\n";
			std::cout << "Selection: ";
		}
	}

	return choice;
}

// Function to handle menu operations
void processMenuChoice(int choice) {
	switch (choice) {
	case 1:
		std::cout << "View connected planes selected. Implementation pending.\n";
		break;
	case 2:
		std::cout << "Modify flight parameters selected. Implementation pending.\n";
		break;
	case 3:
		std::cout << "Send flag manually selected. Implementation pending.\n";
		break;
	case 4:
		std::cout << "Exiting program...\n";
		exit(0);  // Terminate the program
		break;
	default:
		// This should never happen due to input validation
		std::cout << "Invalid selection.\n";
		break;
	}
}

// Main menu function to be called from main()
void runMenu() {
	int choice = 0;

	while (choice != 4) {
		displayMenu();
		choice = getMenuChoice();
		processMenuChoice(choice);
	}
}