#pragma once
#include <iostream>
#include <string>

class Plane {       
private:            
    double x_coordinate;         // current x coordinate of the plane
    double y_coordinate;         // current y coordinate of the plane
    double distance_groundctrl;  // the current distance of the plane from the ground control
    std::string plane_carrier;   // the carrier of the plane
    std::string country;         // the plane's country of origin
    std::string model;
    std::string country_flag;

public:
    // Constructor
    Plane(double x, double y, std::string carrier, std::string country, std::string model);

    // Getters
    double getXCoordinate() const;
    double getYCoordinate() const;
    double getDistanceFromGround() const;
    std::string getCarrier() const;
    std::string getCountry() const;
    std::string getModel() const;
    std::string getCountryFlag() const;

    // Setters
    void setCoordinates(double x, double y);
    void setCarrier(const std::string& carrier);
    void updateDistanceFromGroundControl();

    // Display functionality
    void displayInfo() const;
};