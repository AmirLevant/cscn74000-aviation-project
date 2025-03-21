#pragma once
#include <iostream>
#include <string>

// The Ground Station class
class Ground_Station {       
private:             // Access specifier
    double x_coordinate = 0; // current x coordinate of the ground station
    double y_coordinate = 0; // current y coordinate of the plane
    std::string weather;
};