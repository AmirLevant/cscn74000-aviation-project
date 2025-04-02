#pragma once
#include "helper_functions.h"

class Plane {
    int distance_groundctrl;  // the current distance of the plane from the ground control
    Carrier plane_carrier;   // the carrier of the plane
    Country country;         // the plane's country of origin
    Model model;
    std::string country_flag_path;

public:
    // Constructor
    Plane();
    Plane(Carrier carrier, Country country, Model model);
    Plane(uint8_t* buffer);

    // Getters
    int getDistanceFromGround();
    Carrier getCarrier();
    Country getCountry();
    Model getModel();
    std::string getCountryFlag();

    // Setters
    void setCarrier(Carrier carrier);
    void setDistanceFromGroundControl(int distance);

    void decreaseDistance(int decrement);

    // Display functionality
    void displayInfo();
};