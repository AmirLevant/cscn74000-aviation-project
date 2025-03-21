#pragma once
#include <iostream>
#include <string>


class Plane {       // The plane class
private:             // Access specifier
    double x_coordinate; // current x coordinate of the plane
    double y_coordinate; // current y coordinate of the plane
    double distance_groundctrl; // the current distance of the plane from the ground control
    std::string plane_carrier; // the carrier of the plane
    std::string country; // the plane's country of origin
    std::string model;
    std::string country_flag;
};