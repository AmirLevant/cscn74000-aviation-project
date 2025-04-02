#pragma once
#include "helper_functions.h"

class Plane {
    uint32_t id;
    int distance_groundctrl;  // the current distance of the plane from the ground control
    Carrier plane_carrier;   // the carrier of the plane
    Country country;         // the plane's country of origin
    Model model;
    std::string country_flag_path;

    std::string plane_carrier_name;
    std::string country_name;
    std::string model_name;

public:
    // Constructor
    Plane(uint32_t id);
    Plane(uint32_t id, Carrier carrier, Country country, Model model);
    Plane(uint8_t* buffer);

    // Getters
    int getDistanceFromGround();
    Carrier getCarrier();
    Country getCountry();
    Model getModel();

    void findCarrierName();
    void findCountryName();
    void findModelName();

    // Setters
    void setCarrier(Carrier carrier);
    void setDistanceFromGroundControl(int distance);

    void decreaseDistance(int decrement);

    // Display functionality
    void displayInfo();
};