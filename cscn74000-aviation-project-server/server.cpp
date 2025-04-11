#include "server.h"

#pragma comment(lib, "Ws2_32.lib")

// Initialize the ground station with random weather
Ground_Station groundStation;
SOCKET ServerSocket;
sockaddr_in SvrAddr;
std::map<uint32_t, Country> idCountryMap;

extern std::vector<enum Country> allowedCountries = {
	Afghanistan, Albania, Algeria, Andorra, Angola, Antigua_and_Barbuda, Argentina, Armenia, Australia, Austria, Azerbaijan,
	Bahamas, Bahrain, Bangladesh, Barbados, Belarus, Belgium, Belize, Benin, Bhutan, Bolivia, Bosnia_and_Herzegovina, Botswana, Brazil, Brunei, Bulgaria, Burkina_Faso, Burundi,
	Cabo_Verde, Cambodia, Cameroon, Canada, Central_African_Republic, Chad, Chile, China, Colombia, Comoros, Costa_Rica, Croatia, Cuba, Cyprus, Czechia,
	Democratic_Republic_of_the_Congo, Denmark, Djibouti, Dominica, Dominican_Republic,
	Ecuador, Egypt, El_Salvador, Equatorial_Guinea, Eritrea, Estonia, Eswatini, Ethiopia,
	Fiji, Finland, France,
	Gabon, Gambia, Georgia, Germany, Ghana, Greece, Grenada, Guatemala, Guinea, Guinea_Bissau, Guyana,
	Haiti, Honduras, Hungary,
	Iceland, India, Indonesia, Iran, Iraq, Ireland, Israel, Italy, Ivory_Coast,
	Jamaica, Japan, Jordan,
	Kazakhstan, Kenya, Kiribati, Kosovo, Kuwait, Kyrgyzstan,
	Laos, Latvia, Lebanon, Lesotho, Liberia, Libya, Liechtenstein, Lithuania, Luxembourg,
	Madagascar, Malawi, Malaysia, Maldives, Mali, Malta, Marshall_Islands, Mauritania, Mauritius, Mexico, Micronesia, Moldova, Monaco, Mongolia, Montenegro, Morocco, Mozambique, Myanmar,
	Namibia, Nauru, Nepal, Netherlands, New_Zealand, Nicaragua, Niger, Nigeria, North_Korea, North_Macedonia, Norway,
	Oman,
	Pakistan, Palau, Palestine, Panama, Papua_New_Guinea, Paraguay, Peru, Philippines, Poland, Portugal,
	Qatar,
	Republic_of_the_Congo, Romania, Russia, Rwanda,
	Saint_Kitts_and_Nevis, Saint_Lucia, Saint_Vincent_and_the_Grenadines, Samoa, San_Marino, Sao_Tome_and_Principe, Saudi_Arabia, Senegal, Serbia, Seychelles, Sierra_Leone, Singapore, Slovakia, Slovenia, Solomon_Islands, Somalia, South_Africa, South_Korea, South_Sudan, Spain, Sri_Lanka, Sudan, Suriname, Sweden, Switzerland, Syria,
	Taiwan, Tajikistan, Tanzania, Thailand, Timor_Leste, Togo, Tonga, Trinidad_and_Tobago, Tunisia, Turkey, Turkmenistan, Tuvalu,
	Uganda, Ukraine, United_Arab_Emirates, United_Kingdom, United_States_of_America, Uruguay, Uzbekistan,
	Vanuatu, Vatican_City, Venezuela, Vietnam,
	Yemen,
	Zambia, Zimbabwe,
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
