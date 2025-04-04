// Add to ground_station.h
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

// The Ground Station class
class Ground_Station {
    std::string weather;    // current weather at the ground station
    uint32_t num_transactions;

    // Static vector to store possible weather conditions
    static const std::vector<std::string> weatherTypes;

public:
    // Constructor
    Ground_Station();
    Ground_Station(std::string weather); // Overloaded constructor

    // Weather generator
    static std::string generateRandomWeather();

    // Getters
    std::string getWeather();
    uint32_t getAndIncreaseTransactionNum();
    uint32_t getCurrentTransactionNum();

    // Setters
    void setWeather(std::string& weather);

    // Display functionality
    void displayInfo();
};