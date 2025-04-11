#pragma once
#include "../cscn74000-aviation-project-client/packet.h"
#include "../cscn74000-aviation-project-client/helper_functions.h"
#include "../cscn74000-aviation-project-client/plane.h"
#include "ground_station.h"

void dealWithRecvPacket(Packet* RxPkt, sockaddr_in CltAddr);
void dealWithRequestPacket(Packet* RxPkt, sockaddr_in CltAddr);
bool processFile(Packet* RxPkt);
void initializeConnection(Packet* RxPkt);
void sendAcknowledgement(Packet* TxPkt, sockaddr_in CltAddr);
void sendWeather(Packet* RxPkt, sockaddr_in CltAddr);
void sendGo_NoGo(Packet* RxPkt, sockaddr_in CltAddr, enum Go_NoGo GoNoGo);