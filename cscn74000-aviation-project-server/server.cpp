#include "../cscn74000-aviation-project-client/packet.h"
#include "../cscn74000-aviation-project-client/helper_functions.h"
#include "../cscn74000-aviation-project-client/plane.h"
#include "ground_station.h"

#pragma comment(lib, "Ws2_32.lib")

void dealWithRecvPacket(Packet* RxPkt, sockaddr_in CltAddr);
void dealWithRequestPacket(Packet* RxPkt, sockaddr_in CltAddr);
bool processFile(Packet* RxPkt);
void initializeConnection(Packet* RxPkt);
void sendAcknowledgement(Packet* TxPkt, sockaddr_in CltAddr);
void sendWeather(Packet* RxPkt, sockaddr_in CltAddr);
void sendGo_NoGo(Packet* RxPkt, sockaddr_in CltAddr, enum Go_NoGo GoNoGo);

// Initialize the ground station with random weather
Ground_Station groundStation;
SOCKET ServerSocket;
sockaddr_in SvrAddr;
std::map<uint32_t, Country> idCountryMap;

extern std::vector<enum Country> allowedCountries = {
	Afghanistan, Algeria, Angola, Argentina, Australia, Azerbaijan,
	Bahrain, Barbados, Belgium, Benin, Bolivia, Botswana, Brunei, Burkina_Faso,
	Cabo_Verde, Cameroon, Central_African_Republic, Chile, Colombia, Costa_Rica, Cuba, Czechia,
	Denmark, Dominica,
	Ecuador, El_Salvador, Eritrea, Eswatini,
	Fiji, France,
	Gambia, Germany, Greece, Guatemala, Guinea_Bissau,
	Haiti, Hungary,
	India, Iran, Ireland, Israel, Ivory_Coast,
	Japan,
	Kazakhstan, Kiribati, Kuwait,
	Laos, Lebanon, Liberia, Liechtenstein, Luxembourg,
	Malawi, Maldives, Malta, Mauritania, Mauritius, Micronesia, Monaco, Montenegro, Mozambique, Myanmar,
	Nauru, Netherlands, Nicaragua, Nigeria, North_Macedonia,
	Oman,
	Palau, Panama, Paraguay, Philippines, Portugal,
	Republic_of_the_Congo, Russia,
	Saint_Lucia, Samoa, Sao_Tome_and_Principe, Senegal, Seychelles, Singapore, Slovenia, Somalia, South_Korea, Spain, Sudan, Sweden, Syria,
	Tajikistan, Thailand, Togo, Trinidad_and_Tobago, Turkey, Tuvalu,
	Ukraine, United_Arab_Emirates, United_States_of_America, Uzbekistan,
	Vatican_City, Vietnam,
	Zambia
};

void main()
{
	// Seed the random number generator
	std::srand(std::time(0));

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

	while (1)
	{
		uint8_t* RxBuffer = new uint8_t[MAX_PACKET_SIZE];	//	Buffer for receiving data
		sockaddr_in CltAddr;		//	Client Address for sending resp
		int length_recvfrom_parameter = sizeof(struct sockaddr_in);

		recvfrom(ServerSocket, (char*)RxBuffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&CltAddr, &length_recvfrom_parameter);

		Packet* RxPkt = new Packet((uint8_t*)RxBuffer);
		RxPkt->log(false);

		dealWithRecvPacket(RxPkt, CltAddr);

		delete[] RxBuffer;
		delete RxPkt;
	}

	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources
}

void dealWithRecvPacket(Packet* RxPkt, sockaddr_in CltAddr)
{
	switch (RxPkt->getInteractionType())
	{
	case InteractionType::Telemetry:
	{
		Plane RxPlane(RxPkt->getBody());
		RxPlane.log(idCountryMap);
		RxPlane.print(idCountryMap);
		sendAcknowledgement(RxPkt, CltAddr);
		break;
	}
	case InteractionType::Request:
		dealWithRequestPacket(RxPkt, CltAddr);
		break;
	case InteractionType::Response:
		break;
	}

}

void dealWithRequestPacket(Packet* RxPkt, sockaddr_in CltAddr)
{
	switch (RxPkt->getRequestType())
	{
	case RequestType::InitiateConnection:
		initializeConnection(RxPkt);
		sendAcknowledgement(RxPkt, CltAddr);
		break;
	case RequestType::SendingFileData:
	{
		bool foundMatch = processFile(RxPkt);
		if (foundMatch)
		{
			if (std::find(allowedCountries.begin(), allowedCountries.end(), idCountryMap[RxPkt->getSenderId()]) != allowedCountries.end())
				// Country was found and is not banned
				sendGo_NoGo(RxPkt, CltAddr, Go_NoGo::Go);
			else
				// Country not found or is banned
				sendGo_NoGo(RxPkt, CltAddr, Go_NoGo::NoGo);
		}
		else
			sendGo_NoGo(RxPkt, CltAddr, Go_NoGo::NoGo);
		break;
	}
	case RequestType::Request_Weather:
		sendWeather(RxPkt, CltAddr);
		break;
	}
}

void sendGo_NoGo(Packet* RxPkt, sockaddr_in CltAddr, enum Go_NoGo GoNoGo)
{
	uint8_t* buffer = new uint8_t[sizeof(Go_NoGo)];
	memcpy(buffer, &GoNoGo, sizeof(GoNoGo));
	Packet* TxPkt = new Packet(RxPkt->getDestinationId(), RxPkt->getSenderId(), RequestType::Go_NoGo_Decision, groundStation.getAndIncreaseTransactionNum(), sizeof(GoNoGo), buffer);

	uint8_t* TxBuffer = new uint8_t[TxPkt->get_packetSize()];
	TxPkt->Serialize(TxBuffer);

	sendto(ServerSocket, (const char*)(TxBuffer), TxPkt->get_packetSize(), 0, (SOCKADDR*)&CltAddr, sizeof(CltAddr)); // thats how the library defines UDP sending, we need to typecast
	TxPkt->log(false);

	delete[] TxBuffer;
	delete TxPkt;
}

void sendAcknowledgement(Packet* TxPkt, sockaddr_in CltAddr)
{
	TxPkt->convertToAckPacket();

	uint8_t* TxBuffer = new uint8_t[TxPkt->get_packetSize()];
	TxPkt->Serialize(TxBuffer);

	sendto(ServerSocket, (const char*)(TxBuffer), TxPkt->get_packetSize(), 0, (SOCKADDR*)&CltAddr, sizeof(CltAddr)); // thats how the library defines UDP sending, we need to typecast
	TxPkt->log(false);
}

void sendWeather(Packet* RxPkt, sockaddr_in CltAddr)
{
	uint8_t* buffer = new uint8_t[groundStation.getWeather().length()];
	memcpy(buffer, groundStation.getWeather().c_str(), groundStation.getWeather().length());
	Packet* TxPkt = new Packet(RxPkt->getDestinationId(), RxPkt->getSenderId(), RequestType::Request_Weather, groundStation.getAndIncreaseTransactionNum(), groundStation.getWeather().length(), buffer);

	uint8_t* TxBuffer = new uint8_t[TxPkt->get_packetSize()];
	TxPkt->Serialize(TxBuffer);

	sendto(ServerSocket, (const char*)(TxBuffer), TxPkt->get_packetSize(), 0, (SOCKADDR*)&CltAddr, sizeof(CltAddr)); // thats how the library defines UDP sending, we need to typecast
	TxPkt->log(false);
}

void initializeConnection(Packet* RxPkt)
{
	std::string folderNum = std::to_string(RxPkt->getSenderId());
	std::string folderName = "../PlaneTelemetry/" + folderNum;  // Convert int to string

	if (std::filesystem::create_directory(folderName)) {
		std::cout << "Folder created: " << folderName << std::endl;
	}
	else {
		std::cout << "Failed to create folder for Plane with ID of " << folderNum << " (it may already exist)." << std::endl;
	}
}

bool processFile(Packet* RxPkt)
{
	std::string fileName = "../PlaneTelemetry/" + std::to_string(RxPkt->getSenderId()) + "/flag.png";

	std::ofstream WriteFile(fileName, std::ofstream::binary | std::ios::app);

	WriteFile.write((const char*)RxPkt->getBody(), RxPkt->getBodyLength());

	WriteFile.close();

	if (RxPkt->getBodyLength() < MAX_READ_SIZE)
	{
		if (findMatchingFile(fileName, "../flags/"))
		{
			std::cout << "Found match of received flag with flag of " << fileName << " for plane with ID of " << RxPkt->getSenderId() << std::endl;

			Country country = getCountryEnumFromString(fileName);
			idCountryMap[RxPkt->getSenderId()] = country;
			return true;
		}
		else
		{
			std::cout << "Did not find match of flag for plane with ID of " << RxPkt->getSenderId() << ". Informing plane they cannot land." << std::endl;
			idCountryMap[RxPkt->getSenderId(), Country::Country_Unknown];
			return false;
		}
	}
}
