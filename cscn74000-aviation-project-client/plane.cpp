﻿#include "Plane.h" // Include your header file
#include <cmath> 
#include <algorithm> // For std::remove

// Constructor implementation
Plane::Plane(double x, double y, std::string carrier, std::string country, std::string model) {
    x_coordinate = x;
    y_coordinate = y;
    plane_carrier = carrier;
    this->country = country;
    this->model = model;
    distance_groundctrl = std::sqrt(x * x + y * y);



    // Create the flag lookup string
    country_flag = country + "_flag";

}

// Getter implementations
double Plane::getXCoordinate() const {
    return x_coordinate;
}

double Plane::getYCoordinate() const {
    return y_coordinate;
}

double Plane::getDistanceFromGround() const {
    return distance_groundctrl;
}

std::string Plane::getCarrier() const {
    return plane_carrier;
}

std::string Plane::getCountry() const {
    return country;
}

std::string Plane::getModel() const {
    return model;
}

std::string Plane::getCountryFlag() const {
    return country_flag;
}

// Setter implementations
void Plane::setCoordinates(double x, double y) {
    x_coordinate = x;
    y_coordinate = y;
    // Update distance after changing coordinates
    updateDistanceFromGroundControl();
}

void Plane::setCarrier(const std::string& carrier) {
    plane_carrier = carrier;
}

void Plane::updateDistanceFromGroundControl() {
    // Calculate Euclidean distance from origin
    distance_groundctrl = std::sqrt(x_coordinate * x_coordinate + y_coordinate * y_coordinate);
}


void Plane::displayInfo() const {
    std::cout << "Plane Information:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Carrier: " << plane_carrier << std::endl;
    std::cout << "Country: " << country << " " << country_flag << std::endl;
    std::cout << "Current Position: (" << x_coordinate << ", " << y_coordinate << ")" << std::endl;
    std::cout << "Distance from Ground Control: " << distance_groundctrl << " units" << std::endl;
}