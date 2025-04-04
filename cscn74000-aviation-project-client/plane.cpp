#include "plane.h"
#include "helper_functions.h"

Plane::Plane(uint32_t id)
{
    this->id = id;
    plane_carrier = static_cast<Carrier>(rand() % Carrier::Carrier_Total_Amount);
    country = static_cast<Country>(rand() % Country::Country_Total_Amount);
    model = static_cast<Model>(rand() % Model::Model_Total_Amount);

    distance_groundctrl = STARTING_DISTANCE;

    findCarrierName();
    findCountryName();
    findModelName();

    country_flag_path = "../flags/" + country_name + "_flag.png";

    num_transactions = 0;
    goNoGo = Go_NoGo::Go;
}

// Constructor implementation
Plane::Plane(uint32_t id, Carrier carrier, Country country, Model model) {
    this->id = id;
    plane_carrier = carrier;
    this->country = country;
    this->model = model;
    distance_groundctrl = STARTING_DISTANCE;

    findCarrierName();
    findCountryName();
    findModelName();

    country_flag_path = "../flags/" + plane_carrier_name + "flag.png";

    num_transactions = 0;
    goNoGo = Go_NoGo::Go;
}

Plane::Plane(uint8_t* buffer)
{
    int offset = 0;

    memcpy(&id, buffer + offset, sizeof(id));
    offset += sizeof(id);

    plane_carrier = static_cast<Carrier>(buffer[offset]);
    offset++;

    model = static_cast<Model>(buffer[offset]);
    offset++;

    memcpy(&distance_groundctrl, buffer + offset, sizeof(distance_groundctrl));
    offset += sizeof(distance_groundctrl);

    country = Country_Unknown;
    goNoGo = Go_NoGo::Go;

    findCarrierName();
    findCountryName();
    findModelName();
}

// Getter implementations

uint32_t Plane::getDistanceFromGround()  {
    return distance_groundctrl;
}

uint32_t Plane::getId()
{
    return id;
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

uint32_t Plane::getAndIncreaseTransactionNum()
{
    num_transactions++;
    return num_transactions;
}

uint32_t Plane::getCurrentTransactionNum()
{
    return num_transactions;
}

Go_NoGo Plane::getGoNoGo()
{
    return goNoGo;
}

std::string Plane::getFlagPath()
{
    return country_flag_path;
}

// Setter implementations

void Plane::setCarrier(Carrier carrier) {
    plane_carrier = carrier;
}

void Plane::setDistanceFromGroundControl(uint32_t distance)
{
    distance_groundctrl = distance;
}

void Plane::setGoNoGo(Go_NoGo goNoGo)
{
    this->goNoGo = goNoGo;
}

void Plane::setCountry(Country country)
{
    this->country = country;
    findCountryName();
}

void Plane::decreaseDistance(uint32_t decrement)
{
    distance_groundctrl = distance_groundctrl - decrement;
    if (distance_groundctrl < 0)
    {
        distance_groundctrl = 0;
    }
}

void Plane::increaseDistance(uint32_t increment)
{
    distance_groundctrl = distance_groundctrl + increment;
}

void Plane::serialize(uint8_t* buffer)
{
    int offset = 0;

    memcpy(buffer + offset, &id, sizeof(id));
    offset += sizeof(id);

    buffer[offset] = static_cast<uint8_t>(plane_carrier);
    offset++;

    buffer[offset] = static_cast<uint8_t>(model);
    offset++;

    memcpy(buffer + offset, &distance_groundctrl, sizeof(distance_groundctrl));
    offset += sizeof(distance_groundctrl);
}

void Plane::log(std::map<uint32_t, Country> idCountryMap)
{
    std::string fileName = "../PlaneTelemetry/" + std::to_string(id) + "/Telemetry.csv";  // Convert int to string

    std::ofstream WriteFile;

    if (std::filesystem::exists(fileName) != true)
    {
        std::stringstream ss;
        WriteFile.open(fileName, std::ios::app);
        ss << "PlaneId,PlaneCarrier,PlaneModel,PlaneCountry,DistanceFromGround";
        WriteFile.write(ss.str().c_str(), ss.str().length());
    }
    else
        WriteFile.open(fileName, std::ios::app);

    std::stringstream s;

    country = idCountryMap[id];
    findCountryName();

    s << "\n" << id << "," << plane_carrier_name << "," << model_name << "," << country_name << "," << distance_groundctrl;
    WriteFile.write(s.str().c_str(), s.str().length());

    WriteFile.close();
}

void Plane::print(std::map<uint32_t, Country> idCountryMap)
{
    country = idCountryMap[id];
    findCountryName();

    std::cout << "Plane Information:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Carrier: " << plane_carrier_name << std::endl;
    std::cout << "Model: " << model_name << std::endl;
    std::cout << "Country: " << country_name << std::endl;
    std::cout << "Distance from Ground Control: " << distance_groundctrl << " km" << std::endl << std::endl;
}

void Plane::findCarrierName()
{
    switch (plane_carrier)
    {
    case WestJet:
        plane_carrier_name = "West Jet";
        break;
    case SunWing:
        plane_carrier_name = "SunWing";
        break;
    case JetBlue:
        plane_carrier_name = "Jet Blue";
        break;
    case Spirit:
        plane_carrier_name = "Spirit";
        break;
    case Frontier:
        plane_carrier_name = "Frontier";
        break;
    case Virgin:
        plane_carrier_name = "Virgin";
        break;
    case Vistara:
        plane_carrier_name = "Vistara";
        break;
    case Flair:
        plane_carrier_name = "Flair";
        break;
    default:
        plane_carrier_name = "Unknown";
        break;
    }
}

void Plane::findCountryName()
{
    switch (country)
    {
    case Afghanistan:
        country_name = "Afghanistan";
        break;
    case Albania:
        country_name = "Albania";
        break;
    case Algeria:
        country_name = "Algeria";
        break;
    case Andorra:
        country_name = "Andorra";
        break;
    case Angola:
        country_name = "Angola";
        break;
    case Antigua_and_Barbuda:
        country_name = "Antigua and Barbuda";
        break;
    case Argentina:
        country_name = "Argentina";
        break;
    case Armenia:
        country_name = "Armenia";
        break;
    case Australia:
        country_name = "Australia";
        break;
    case Austria:
        country_name = "Austria";
        break;
    case Azerbaijan:
        country_name = "Azerbaijan";
        break;
    case Bahamas:
        country_name = "Bahamas";
        break;
    case Bahrain:
        country_name = "Bahrain";
        break;
    case Bangladesh:
        country_name = "Bangladesh";
        break;
    case Barbados:
        country_name = "Barbados";
        break;
    case Belarus:
        country_name = "Belarus";
        break;
    case Belgium:
        country_name = "Belgium";
        break;
    case Belize:
        country_name = "Belize";
        break;
    case Benin:
        country_name = "Benin";
        break;
    case Bhutan:
        country_name = "Bhutan";
        break;
    case Bolivia:
        country_name = "Bolivia";
        break;
    case Bosnia_and_Herzegovina:
        country_name = "Bosnia and Herzegovina";
        break;
    case Botswana:
        country_name = "Botswana";
        break;
    case Brazil:
        country_name = "Brazil";
        break;
    case Brunei:
        country_name = "Brunei";
        break;
    case Bulgaria:
        country_name = "Bulgaria";
        break;
    case Burkina_Faso:
        country_name = "Burkina Faso";
        break;
    case Burundi:
        country_name = "Burundi";
        break;
    case Cabo_Verde:
        country_name = "Cabo Verde";
        break;
    case Cambodia:
        country_name = "Cambodia";
        break;
    case Cameroon:
        country_name = "Cameroon";
        break;
    case Canada:
        country_name = "Canada";
        break;
    case Central_African_Republic:
        country_name = "Central African Republic";
        break;
    case Chad:
        country_name = "Chad";
        break;
    case Chile:
        country_name = "Chile";
        break;
    case China:
        country_name = "China";
        break;
    case Colombia:
        country_name = "Colombia";
        break;
    case Comoros:
        country_name = "Comoros";
        break;
    case Costa_Rica:
        country_name = "Costa Rica";
        break;
    case Croatia:
        country_name = "Croatia";
        break;
    case Cuba:
        country_name = "Cuba";
        break;
    case Cyprus:
        country_name = "Cyprus";
        break;
    case Czechia:
        country_name = "Czechia";
        break;
    case Democratic_Republic_of_the_Congo:
        country_name = "Democratic Republic of the Congo";
        break;
    case Denmark:
        country_name = "Denmark";
        break;
    case Djibouti:
        country_name = "Djibouti";
        break;
    case Dominica:
        country_name = "Dominica";
        break;
    case Dominican_Republic:
        country_name = "Dominican Republic";
        break;
    case Ecuador:
        country_name = "Ecuador";
        break;
    case Egypt:
        country_name = "Egypt";
        break;
    case El_Salvador:
        country_name = "El Salvador";
        break;
    case Equatorial_Guinea:
        country_name = "Equatorial Guinea";
        break;
    case Eritrea:
        country_name = "Eritrea";
        break;
    case Estonia:
        country_name = "Estonia";
        break;
    case Eswatini:
        country_name = "Eswatini";
        break;
    case Ethiopia:
        country_name = "Ethiopia";
        break;
    case Fiji:
        country_name = "Fiji";
        break;
    case Finland:
        country_name = "Finland";
        break;
    case France:
        country_name = "France";
        break;
    case Gabon:
        country_name = "Gabon";
        break;
    case Gambia:
        country_name = "Gambia";
        break;
    case Georgia:
        country_name = "Georgia";
        break;
    case Germany:
        country_name = "Germany";
        break;
    case Ghana:
        country_name = "Ghana";
        break;
    case Greece:
        country_name = "Greece";
        break;
    case Grenada:
        country_name = "Grenada";
        break;
    case Guatemala:
        country_name = "Guatemala";
        break;
    case Guinea:
        country_name = "Guinea";
        break;
    case Guinea_Bissau:
        country_name = "Guinea Bissau";
        break;
    case Guyana:
        country_name = "Guyana";
        break;
    case Haiti:
        country_name = "Haiti";
        break;
    case Honduras:
        country_name = "Honduras";
        break;
    case Hungary:
        country_name = "Hungary";
        break;
    case Iceland:
        country_name = "Iceland";
        break;
    case India:
        country_name = "India";
        break;
    case Indonesia:
        country_name = "Indonesia";
        break;
    case Iran:
        country_name = "Iran";
        break;
    case Iraq:
        country_name = "Iraq";
        break;
    case Ireland:
        country_name = "Ireland";
        break;
    case Israel:
        country_name = "Israel";
        break;
    case Italy:
        country_name = "Italy";
        break;
    case Ivory_Coast:
        country_name = "Ivory Coast";
        break;
    case Jamaica:
        country_name = "Jamaica";
        break;
    case Japan:
        country_name = "Japan";
        break;
    case Jordan:
        country_name = "Jordan";
        break;
    case Kazakhstan:
        country_name = "Kazakhstan";
        break;
    case Kenya:
        country_name = "Kenya";
        break;
    case Kiribati:
        country_name = "Kiribati";
        break;
    case Kosovo:
        country_name = "Kosovo";
        break;
    case Kuwait:
        country_name = "Kuwait";
        break;
    case Kyrgyzstan:
        country_name = "Kyrgyzstan";
        break;
    case Laos:
        country_name = "Laos";
        break;
    case Latvia:
        country_name = "Latvia";
        break;
    case Lebanon:
        country_name = "Lebanon";
        break;
    case Lesotho:
        country_name = "Lesotho";
        break;
    case Liberia:
        country_name = "Liberia";
        break;
    case Libya:
        country_name = "Libya";
        break;
    case Liechtenstein:
        country_name = "Liechtenstein";
        break;
    case Lithuania:
        country_name = "Lithuania";
        break;
    case Luxembourg:
        country_name = "Luxembourg";
        break;
    case Madagascar:
        country_name = "Madagascar";
        break;
    case Malawi:
        country_name = "Malawi";
        break;
    case Malaysia:
        country_name = "Malaysia";
        break;
    case Maldives:
        country_name = "Maldives";
        break;
    case Mali:
        country_name = "Mali";
        break;
    case Malta:
        country_name = "Malta";
        break;
    case Marshall_Islands:
        country_name = "Marshall Islands";
        break;
    case Mauritania:
        country_name = "Mauritania";
        break;
    case Mauritius:
        country_name = "Mauritius";
        break;
    case Mexico:
        country_name = "Mexico";
        break;
    case Micronesia:
        country_name = "Micronesia";
        break;
    case Moldova:
        country_name = "Moldova";
        break;
    case Monaco:
        country_name = "Monaco";
        break;
    case Mongolia:
        country_name = "Mongolia";
        break;
    case Montenegro:
        country_name = "Montenegro";
        break;
    case Morocco:
        country_name = "Morocco";
        break;
    case Mozambique:
        country_name = "Mozambique";
        break;
    case Myanmar:
        country_name = "Myanmar";
        break;
    case Namibia:
        country_name = "Namibia";
        break;
    case Nauru:
        country_name = "Nauru";
        break;
    case Nepal:
        country_name = "Nepal";
        break;
    case Netherlands:
        country_name = "Netherlands";
        break;
    case New_Zealand:
        country_name = "New Zealand";
        break;
    case Nicaragua:
        country_name = "Nicaragua";
        break;
    case Niger:
        country_name = "Niger";
        break;
    case Nigeria:
        country_name = "Nigeria";
        break;
    case North_Korea:
        country_name = "North Korea";
        break;
    case North_Macedonia:
        country_name = "North Macedonia";
        break;
    case Norway:
        country_name = "Norway";
        break;
    case Oman:
        country_name = "Oman";
        break;
    case Pakistan:
        country_name = "Pakistan";
        break;
    case Palau:
        country_name = "Palau";
        break;
    case Palestine:
        country_name = "Palestine";
        break;
    case Panama:
        country_name = "Panama";
        break;
    case Papua_New_Guinea:
        country_name = "Papua New Guinea";
        break;
    case Paraguay:
        country_name = "Paraguay";
        break;
    case Peru:
        country_name = "Peru";
        break;
    case Philippines:
        country_name = "Philippines";
        break;
    case Poland:
        country_name = "Poland";
        break;
    case Portugal:
        country_name = "Portugal";
        break;
    case Qatar:
        country_name = "Qatar";
        break;
    case Republic_of_the_Congo:
        country_name = "Republic of the Congo";
        break;
    case Romania:
        country_name = "Romania";
        break;
    case Russia:
        country_name = "Russia";
        break;
    case Rwanda:
        country_name = "Rwanda";
        break;
    case Saint_Kitts_and_Nevis:
        country_name = "Saint Kitts and Nevis";
        break;
    case Saint_Lucia:
        country_name = "Saint Lucia";
        break;
    case Saint_Vincent_and_the_Grenadines:
        country_name = "Saint Vincent and the Grenadines";
        break;
    case Samoa:
        country_name = "Samoa";
        break;
    case San_Marino:
        country_name = "San Marino";
        break;
    case Sao_Tome_and_Principe:
        country_name = "Sao Tome and Principe";
        break;
    case Saudi_Arabia:
        country_name = "Saudi Arabia";
        break;
    case Senegal:
        country_name = "Senegal";
        break;
    case Serbia:
        country_name = "Serbia";
        break;
    case Seychelles:
        country_name = "Seychelles";
        break;
    case Sierra_Leone:
        country_name = "Sierra Leone";
        break;
    case Singapore:
        country_name = "Singapore";
        break;
    case Slovakia:
        country_name = "Slovakia";
        break;
    case Slovenia:
        country_name = "Slovenia";
        break;
    case Solomon_Islands:
        country_name = "Solomon Islands";
        break;
    case Somalia:
        country_name = "Somalia";
        break;
    case South_Africa:
        country_name = "South Africa";
        break;
    case South_Korea:
        country_name = "South Korea";
        break;
    case South_Sudan:
        country_name = "South Sudan";
        break;
    case Spain:
        country_name = "Spain";
        break;
    case Sri_Lanka:
        country_name = "Sri Lanka";
        break;
    case Sudan:
        country_name = "Sudan";
        break;
    case Suriname:
        country_name = "Suriname";
        break;
    case Sweden:
        country_name = "Sweden";
        break;
    case Switzerland:
        country_name = "Switzerland";
        break;
    case Syria:
        country_name = "Syria";
        break;
    case Taiwan:
        country_name = "Taiwan";
        break;
    case Tajikistan:
        country_name = "Tajikistan";
        break;
    case Tanzania:
        country_name = "Tanzania";
        break;
    case Thailand:
        country_name = "Thailand";
        break;
    case Timor_Leste:
        country_name = "Timor Leste";
        break;
    case Togo:
        country_name = "Togo";
        break;
    case Tonga:
        country_name = "Tonga";
        break;
    case Trinidad_and_Tobago:
        country_name = "Trinidad and Tobago";
        break;
    case Tunisia:
        country_name = "Tunisia";
        break;
    case Turkey:
        country_name = "Turkey";
        break;
    case Turkmenistan:
        country_name = "Turkmenistan";
        break;
    case Tuvalu:
        country_name = "Tuvalu";
        break;
    case Uganda:
        country_name = "Uganda";
        break;
    case Ukraine:
        country_name = "Ukraine";
        break;
    case United_Arab_Emirates:
        country_name = "United Arab Emirates";
        break;
    case United_Kingdom:
        country_name = "United Kingdom";
        break;
    case United_States_of_America:
        country_name = "United States of America";
        break;
    case Uruguay:
        country_name = "Uruguay";
        break;
    case Uzbekistan:
        country_name = "Uzbekistan";
        break;
    case Vanuatu:
        country_name = "Vanuatu";
        break;
    case Vatican_City:
        country_name = "Vatican City";
        break;
    case Venezuela:
        country_name = "Venezuela";
        break;
    case Vietnam:
        country_name = "Vietnam";
        break;
    case Yemen:
        country_name = "Yemen";
        break;
    case Zambia:
        country_name = "Zambia";
        break;
    case Zimbabwe:
        country_name = "Zimbabwe";
        break;
    default:
        country_name = "Unknown";
        break;
    }
}

void Plane::findModelName()
{
    switch(model)
    {
    case Boeing_747:
        model_name = "Boeing 747";
        break;
    case Boeing_787_Dreamliner:
        model_name = "Boeing 787 Dreamliner";
        break;
    case Boeing_737_MAX:
        model_name = "Boeing 737 MAX";
        break;
    case Airbus_A380:
        model_name = "Airbus A380";
        break;
    case Airbus_A320neo:
        model_name = "Airbus A320neo";
        break;
    case Airbus_A350:
        model_name = "Airbus A350";
        break;
    case Embraer_E190:
        model_name = "Embraer E190";
        break;
    case Embraer_E175:
        model_name = "Embraer E175";
        break;
    case Bombardier_CRJ900:
        model_name = "Bombardier CRJ900";
        break;
    case Bombardier_Global_7500:
        model_name = "Bombardier Global 7500";
        break;
    case Cessna_Citation_X:
        model_name = "Cessna Citation X";
        break;
    case Gulfstream_G650:
        model_name = "Gulfstream G650";
        break;
    case McDonnell_Douglas_MD_11:
        model_name = "McDonnell Douglas MD 11";
        break;
    case Concorde:
        model_name = "Concorde";
        break;
    case Lockheed_Martin_C_130_Hercules:
        model_name = "Lockheed Martin C 130 Hercules";
        break;
    }
}
