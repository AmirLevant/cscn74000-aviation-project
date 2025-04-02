#pragma once
#include <windows.networking.sockets.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <string>

#define MAX_RECEIVE_SIZE 65565

enum InteractionType : uint8_t { Telemetry, Request, Response };
enum Carrier : uint8_t { WestJet, SunWing, JetBlue, Spirit, Frontier, Virgin, Vistara, Flair, Carrier_Total_Amount };
enum Country : uint8_t 
{
	Afghanistan, Albania, Algeria, Andorra, Angola, Antigua_and_Barbuda, Argentina, Armenia, Australia, Austria, Azerbaijan, 
	Bahamas, Bahrain, Bangladesh, Barbados, Belarus, Belgium, Belize, Benin, Bhutan, Bolivia, Bosnia_and_Herzegovina, Botswana, Brazil, Brunei, Bulgaria, Burkina_Faso, Burundi, 
	Cabo_Verde, Cambodia, Cameroon, Canada, Central_African_Republic, Chad, Chile, China, Colombia, Comoros, Costa_Rica, Croatia, Cuba, Cyprus, Czechia, 
	Democratic_Republic_of_the_Congo, Denmark, Djibouti, Dominica, Dominican_Republic, 
	Ecuador, Egypt, El_Salvador, Equatorial_Guinea, Eritrea, Estonia, Eswatini, Ethiopia, 
	Fiji, Finland, France, Gabon, Gambia, Georgia, Germany, Ghana, Greece, Grenada, Guatemala, Guinea, Guinea_Bissau, Guyana, 
	Haiti, Honduras, Hungary, 
	Iceland, India, Indonesia, Iran, Iraq, Ireland, Israel, Italy, Ivory_Coast, 
	Jamaica, Japan, Jordan, 
	Kazakhstan, Kenya, Kiribati, Kosovo, Kuwait, Kyrgyzstan, 
	Laos, Latvia, Lebanon, Lesotho, Liberia, Libya, Liechtenstein, Lithuania, Luxembourg, 
	Madagascar, Malawi, Malaysia, Maldives, Mali, Malta, Marshall_Islands, Mauritania, Mauritius, Mexico, Micronesia, Moldova, Monaco, Mongolia, Montenegro, Morocco, Mozambique, Myanmar, 
	Namibia, Nauru, Nepal, Netherlands, New_Zealand, Nicaragua, Niger, Nigeria, North_Korea, North_Macedonia, Norway, 
	Oman, 
	Pakistan, Palau, Palestine, Panama, Papua_New_Guinea, Paraguay, Peru, Philippines, Poland, Portugal, 
	Qatar, 
	Republic_of_the_Congo, Romania, Russia, Rwanda, 
	Saint_Kitts_and_Nevis, Saint_Lucia, Saint_Vincent_and_the_Grenadines, Samoa, San_Marino, Sao_Tome_and_Principe, Saudi_Arabia, Senegal, Serbia, Seychelles, Sierra_Leone, Singapore, Slovakia, Slovenia, Solomon_Islands, Somalia, South_Africa, South_Korea, South_Sudan, Spain, Sri_Lanka, Sudan, Suriname, Sweden, Switzerland, Syria, 
	Taiwan, Tajikistan, Tanzania, Thailand, Timor_Leste, Togo, Tonga, Trinidad_and_Tobago, Tunisia, Turkey, Turkmenistan, Tuvalu, 
	Uganda, Ukraine, United_Arab_Emirates, United_Kingdom, United_States, Uruguay, Uzbekistan, 
	Vanuatu, Vatican_City, Venezuela, Vietnam, 
	Yemen, 
	Zambia, Zimbabwe,
	Country_Total_Amount
};
enum Model : uint8_t { Boeing_747, Boeing_787_Dreamliner, Boeing_737_MAX, Airbust_A380, Airbus_A320neo, Airbus_A350, Embraer_E190, Embraer_E175, Bombardier_CRJ900, Bombardier_Global_7500,
Cessna_Citation_X, Gulfstream_G650, McDonnell_Douglas_MD_11, Concorde, Lockheed_Martin_C_130_Hercules, Model_Total_Amount};

