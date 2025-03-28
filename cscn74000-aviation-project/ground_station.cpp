#include "ground_station.h"
#include <cmath>

// Constructor implementation
Ground_Station::Ground_Station(double x, double y, std::string weather) {
    x_coordinate = x;
    y_coordinate = y;
    this->weather = weather;
}

// Overloaded constructor with default coordinates (0,0)
Ground_Station::Ground_Station(std::string weather) {
    x_coordinate = 0; // Explicitly set to 0 to ensure proper initialization
    y_coordinate = 0; // Explicitly set to 0 to ensure proper initialization
    this->weather = weather;
}

// Getter implementations
double Ground_Station::getXCoordinate() const {
    return x_coordinate;
}

double Ground_Station::getYCoordinate() const {
    return y_coordinate;
}

std::string Ground_Station::getWeather() const {
    return weather;
}

// Setter implementations
void Ground_Station::setCoordinates(double x, double y) {
    x_coordinate = x;
    y_coordinate = y;
}

void Ground_Station::setWeather(const std::string& weather) {
    this->weather = weather;
}


// Display functionality
void Ground_Station::displayInfo() const {
    std::cout << "Ground Station Information:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Location: (" << x_coordinate << ", " << y_coordinate << ")" << std::endl;
    std::cout << "Current Weather: " << weather << std::endl;
}