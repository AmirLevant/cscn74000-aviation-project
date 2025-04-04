#include "ground_station.h"
#include <cmath>

// Define the static weather types
const std::vector<std::string> Ground_Station::weatherTypes = {
    "Clear",
    "Partly Cloudy",
    "Cloudy",
    "Rain",
    "Thunderstorm",
    "Snow",
    "Fog",
    "Windy",
    "Hail",
    "Freezing Rain"
};

// Constructor implementation
Ground_Station::Ground_Station() {
    // Generate random weather on initialization
    this->weather = generateRandomWeather();
    num_transactions = 0;
}

// Overloaded constructor with specified weather
Ground_Station::Ground_Station(std::string weather) {
    this->weather = weather;
    num_transactions = 0;
}

// Random weather generator implementation
std::string Ground_Station::generateRandomWeather() {

    // Get a random index within the weatherTypes vector
    int index = std::rand() % weatherTypes.size();

    return weatherTypes[index];
}

// Getter implementations
std::string Ground_Station::getWeather() {
    return weather;
}

uint32_t Ground_Station::getAndIncreaseTransactionNum()
{
    num_transactions++;
    return num_transactions;
}

uint32_t Ground_Station::getCurrentTransactionNum()
{
    return num_transactions;
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