#pragma once
#include <iostream>
#include <string>

// The Ground Station class
class Ground_Station {
    std::string weather;    // current weather at the ground station

public:
    // Constructor
    Ground_Station();
    Ground_Station(std::string weather); // Overloaded constructor

    // Getters
    std::string getWeather();

    // Setters
    void setWeather(std::string& weather);

    // Display functionality
    void displayInfo();
};