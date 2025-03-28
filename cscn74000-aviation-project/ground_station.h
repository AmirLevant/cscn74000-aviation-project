#pragma once
#include <iostream>
#include <string>

// The Ground Station class
class Ground_Station {
private:             // Access specifier
    double x_coordinate = 0; // current x coordinate of the ground station
    double y_coordinate = 0; // current y coordinate of the ground station
    std::string weather;    // current weather at the ground station

public:
    // Constructor
    Ground_Station(double x, double y, std::string weather);
    Ground_Station(std::string weather); // Overloaded constructor

    // Getters
    double getXCoordinate() const;
    double getYCoordinate() const;
    std::string getWeather() const;

    // Setters
    void setCoordinates(double x, double y);
    void setWeather(const std::string& weather);

    // Calculate distance to a plane
    double distanceToPlane(double plane_x, double plane_y) const;

    // Display functionality
    void displayInfo() const;
};