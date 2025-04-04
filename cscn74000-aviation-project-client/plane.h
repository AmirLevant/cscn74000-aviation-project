#pragma once
#include "helper_functions.h"

#define STARTING_DISTANCE 350
#define PLANE_SERIALIZATION_SIZE 10

class Plane 
{
    uint32_t id;
    uint32_t distance_groundctrl;  // the current distance of the plane from the ground control
    Carrier plane_carrier;   // the carrier of the plane
    Country country;         // the plane's country of origin
    Model model;
    uint32_t num_transactions;
    Go_NoGo goNoGo;
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
    uint32_t getDistanceFromGround();
    uint32_t getId();
    Carrier getCarrier();
    Country getCountry();
    Model getModel();
    uint32_t getAndIncreaseTransactionNum();
    uint32_t getCurrentTransactionNum();
    Go_NoGo getGoNoGo();
    std::string getFlagPath();


    void findCarrierName();
    void findCountryName();
    void findModelName();

    // Setters
    void setCarrier(Carrier carrier);
    void setDistanceFromGroundControl(uint32_t distance);
    void setGoNoGo(Go_NoGo goNoGo);
    void setCountry(Country country);

    void decreaseDistance(uint32_t decrement);
    void increaseDistance(uint32_t increment);

    // Display functionality
    void displayInfo();

    void serialize(uint8_t* buffer);

    void log(std::map<uint32_t, Country> idCountryMap);
};