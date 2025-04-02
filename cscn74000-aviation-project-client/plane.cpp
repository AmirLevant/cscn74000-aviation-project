#include "plane.h"
#include "helper_functions.h"

#define STARTING_DISTANCE 350

Plane::Plane()
{
    plane_carrier = static_cast<Carrier>(rand() % Carrier::Carrier_Total_Amount);
    country = static_cast<Country>(rand() % Country::Country_Total_Amount);
    model = static_cast<Model>(rand() % Model::Model_Total_Amount);

    distance_groundctrl = STARTING_DISTANCE;

}

// Constructor implementation
Plane::Plane(Carrier carrier, Country country, Model model) {
    plane_carrier = carrier;
    this->country = country;
    this->model = model;
    distance_groundctrl = STARTING_DISTANCE;

    // Create the flag lookup string
    //country_flag = country + "_flag";
}

Plane::Plane(uint8_t* buffer)
{
    int offset = 0;
    
    plane_carrier = static_cast<Carrier>(buffer[offset]);
    offset++;

    country = static_cast<Country>(buffer[offset]);
    offset++;

    model = static_cast<Model>(buffer[offset]);
    offset++;

    memcpy(&distance_groundctrl, buffer + offset, sizeof(distance_groundctrl));
    offset += sizeof(distance_groundctrl);


}

// Getter implementations

int Plane::getDistanceFromGround()  {
    return distance_groundctrl;
}

Carrier Plane::getCarrier()  {
    return plane_carrier;
}

Country Plane::getCountry()  {
    return country;
}

Model Plane::getModel()  {
    return model;
}

//std::string Plane::getCountryFlag()  {
//    return country_flag;
//}

// Setter implementations

void Plane::setCarrier(Carrier carrier) {
    plane_carrier = carrier;
}

void Plane::setDistanceFromGroundControl(int distance)
{
    distance_groundctrl = distance;
}

void Plane::decreaseDistance(int decrement)
{
    distance_groundctrl = distance_groundctrl - decrement;
    if (distance_groundctrl < 0)
    {
        distance_groundctrl = 0;
    }
}

void Plane::displayInfo() {
    std::cout << "Plane Information:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Carrier: " << plane_carrier << std::endl;
    //std::cout << "Country: " << country << " " << country_flag << std::endl;
    std::cout << "Distance from Ground Control: " << distance_groundctrl << " units" << std::endl;
}

