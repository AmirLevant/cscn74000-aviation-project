#pragma once
#include <fstream>
#include "packet.h"
#include "plane.h"

void sendFlag(Plane& plane);
void requestWeather(Plane& plane);
int waitForResponse(Plane& plane);
void transitionCSM(ClientStateMachine state, Plane& plane);

extern ClientStateMachine CSM;  