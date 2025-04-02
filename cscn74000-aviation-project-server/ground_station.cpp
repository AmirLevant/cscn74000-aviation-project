#include "ground_station.h"
#include <cmath>

// Constructor implementation
Ground_Station::Ground_Station() {
    this->weather = "Unknown";
}

// Overloaded constructor with default coordinates (0,0)
Ground_Station::Ground_Station(std::string weather) {
    this->weather = weather;
}

// Getter implementations

std::string Ground_Station::getWeather() {
    return weather;
}

// Setter implementations

void Ground_Station::setWeather(std::string& weather) {
    this->weather = weather;
}


// Display functionality
void Ground_Station::displayInfo() {
    std::cout << "Ground Station Information:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Current Weather: " << weather << std::endl;
}