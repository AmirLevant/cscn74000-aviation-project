#include "../cscn74000-aviation-project-client/helper_functions.h"


// Function to find a file in a directory that matches the size and content of the input file
bool findMatchingFile(std::string& filePath, const std::string& directoryPath) {
    // Open the input file
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile) {
        return false; // Return empty string if file cannot be opened
    }

    // Get the size of the input file
    inputFile.seekg(0, std::ios::end);
    std::streamsize inputFileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Read the input file content
    uint8_t* inputBuffer = new uint8_t[inputFileSize];
    inputFile.read(reinterpret_cast<char*>(inputBuffer), inputFileSize);
    inputFile.close();

    // Iterate through all files in the specified directory
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_regular_file(entry)) {
            // Check if file size matches
            if (std::filesystem::file_size(entry) == inputFileSize) {
                // Open the potential matching file
                std::ifstream matchFile(entry.path(), std::ios::binary);
                if (!matchFile) {
                    continue; // Skip if file can't be opened
                }

                // Read the potential matching file content
                uint8_t* matchBuffer = new uint8_t[inputFileSize];
                matchFile.read(reinterpret_cast<char*>(matchBuffer), inputFileSize);
                matchFile.close();

                // Compare the content of both files
                bool isMatch = (memcmp(inputBuffer, matchBuffer, inputFileSize) == 0);
                delete[] matchBuffer;

                if (isMatch) {
                    // Found a match
                    std::string filename = std::filesystem::path(entry.path()).filename().string();  // "Zambia_flag.png"

                    // Remove "_flag.png" suffix
                    size_t pos = filename.find("_flag.png");
                    if (pos != std::string::npos) {
                        filename = filename.substr(0, pos);  // "Zambia"
                        filePath = filename;
                    }
                    delete[] inputBuffer;
                    return true;
                }
            }
        }
    }

    // No match found
    delete[] inputBuffer;

    return false;
}

Country getCountryEnumFromString(const std::string& name) {
    static const std::map<std::string, Country> countryMap = {
        {"Afghanistan", Afghanistan},
        {"Albania", Albania},
        {"Algeria", Algeria},
        {"Andorra", Andorra},
        {"Angola", Angola},
        {"Antigua and Barbuda", Antigua_and_Barbuda},
        {"Argentina", Argentina},
        {"Armenia", Armenia},
        {"Australia", Australia},
        {"Austria", Austria},
        {"Azerbaijan", Azerbaijan},
        {"Bahamas", Bahamas},
        {"Bahrain", Bahrain},
        {"Bangladesh", Bangladesh},
        {"Barbados", Barbados},
        {"Belarus", Belarus},
        {"Belgium", Belgium},
        {"Belize", Belize},
        {"Benin", Benin},
        {"Bhutan", Bhutan},
        {"Bolivia", Bolivia},
        {"Bosnia and Herzegovina", Bosnia_and_Herzegovina},
        {"Botswana", Botswana},
        {"Brazil", Brazil},
        {"Brunei", Brunei},
        {"Bulgaria", Bulgaria},
        {"Burkina Faso", Burkina_Faso},
        {"Burundi", Burundi},
        {"Cabo Verde", Cabo_Verde},
        {"Cambodia", Cambodia},
        {"Cameroon", Cameroon},
        {"Canada", Canada},
        {"Central African Republic", Central_African_Republic},
        {"Chad", Chad},
        {"Chile", Chile},
        {"China", China},
        {"Colombia", Colombia},
        {"Comoros", Comoros},
        {"Costa Rica", Costa_Rica},
        {"Croatia", Croatia},
        {"Cuba", Cuba},
        {"Cyprus", Cyprus},
        {"Czechia", Czechia},
        {"Democratic Republic of the Congo", Democratic_Republic_of_the_Congo},
        {"Denmark", Denmark},
        {"Djibouti", Djibouti},
        {"Dominica", Dominica},
        {"Dominican Republic", Dominican_Republic},
        {"Ecuador", Ecuador},
        {"Egypt", Egypt},
        {"El Salvador", El_Salvador},
        {"Equatorial Guinea", Equatorial_Guinea},
        {"Eritrea", Eritrea},
        {"Estonia", Estonia},
        {"Eswatini", Eswatini},
        {"Ethiopia", Ethiopia},
        {"Fiji", Fiji},
        {"Finland", Finland},
        {"France", France},
        {"Gabon", Gabon},
        {"Gambia", Gambia},
        {"Georgia", Georgia},
        {"Germany", Germany},
        {"Ghana", Ghana},
        {"Greece", Greece},
        {"Grenada", Grenada},
        {"Guatemala", Guatemala},
        {"Guinea", Guinea},
        {"Guinea Bissau", Guinea_Bissau},
        {"Guyana", Guyana},
        {"Haiti", Haiti},
        {"Honduras", Honduras},
        {"Hungary", Hungary},
        {"Iceland", Iceland},
        {"India", India},
        {"Indonesia", Indonesia},
        {"Iran", Iran},
        {"Iraq", Iraq},
        {"Ireland", Ireland},
        {"Israel", Israel},
        {"Italy", Italy},
        {"Ivory Coast", Ivory_Coast},
        {"Jamaica", Jamaica},
        {"Japan", Japan},
        {"Jordan", Jordan},
        {"Kazakhstan", Kazakhstan},
        {"Kenya", Kenya},
        {"Kiribati", Kiribati},
        {"Kosovo", Kosovo},
        {"Kuwait", Kuwait},
        {"Kyrgyzstan", Kyrgyzstan},
        {"Laos", Laos},
        {"Latvia", Latvia},
        {"Lebanon", Lebanon},
        {"Lesotho", Lesotho},
        {"Liberia", Liberia},
        {"Libya", Libya},
        {"Liechtenstein", Liechtenstein},
        {"Lithuania", Lithuania},
        {"Luxembourg", Luxembourg},
        {"Madagascar", Madagascar},
        {"Malawi", Malawi},
        {"Malaysia", Malaysia},
        {"Maldives", Maldives},
        {"Mali", Mali},
        {"Malta", Malta},
        {"Marshall Islands", Marshall_Islands},
        {"Mauritania", Mauritania},
        {"Mauritius", Mauritius},
        {"Mexico", Mexico},
        {"Micronesia", Micronesia},
        {"Moldova", Moldova},
        {"Monaco", Monaco},
        {"Mongolia", Mongolia},
        {"Montenegro", Montenegro},
        {"Morocco", Morocco},
        {"Mozambique", Mozambique},
        {"Myanmar", Myanmar},
        {"Namibia", Namibia},
        {"Nauru", Nauru},
        {"Nepal", Nepal},
        {"Netherlands", Netherlands},
        {"New Zealand", New_Zealand},
        {"Nicaragua", Nicaragua},
        {"Niger", Niger},
        {"Nigeria", Nigeria},
        {"North Korea", North_Korea},
        {"North Macedonia", North_Macedonia},
        {"Norway", Norway},
        {"Oman", Oman},
        {"Pakistan", Pakistan},
        {"Palau", Palau},
        {"Palestine", Palestine},
        {"Panama", Panama},
        {"Papua New Guinea", Papua_New_Guinea},
        {"Paraguay", Paraguay},
        {"Peru", Peru},
        {"Philippines", Philippines},
        {"Poland", Poland},
        {"Portugal", Portugal},
        {"Qatar", Qatar},
        {"Republic of the Congo", Republic_of_the_Congo},
        {"Romania", Romania},
        {"Russia", Russia},
        {"Rwanda", Rwanda},
        {"Saint Kitts and Nevis", Saint_Kitts_and_Nevis},
        {"Saint Lucia", Saint_Lucia},
        {"Saint Vincent and the Grenadines", Saint_Vincent_and_the_Grenadines},
        {"Samoa", Samoa},
        {"San Marino", San_Marino},
        {"Sao Tome and Principe", Sao_Tome_and_Principe},
        {"Saudi Arabia", Saudi_Arabia},
        {"Senegal", Senegal},
        {"Serbia", Serbia},
        {"Seychelles", Seychelles},
        {"Sierra Leone", Sierra_Leone},
        {"Singapore", Singapore},
        {"Slovakia", Slovakia},
        {"Slovenia", Slovenia},
        {"Solomon Islands", Solomon_Islands},
        {"Somalia", Somalia},
        {"South Africa", South_Africa},
        {"South Korea", South_Korea},
        {"South Sudan", South_Sudan},
        {"Spain", Spain},
        {"Sri Lanka", Sri_Lanka},
        {"Sudan", Sudan},
        {"Suriname", Suriname},
        {"Sweden", Sweden},
        {"Switzerland", Switzerland},
        {"Syria", Syria},
        {"Taiwan", Taiwan},
        {"Tajikistan", Tajikistan},
        {"Tanzania", Tanzania},
        {"Thailand", Thailand},
        {"Timor Leste", Timor_Leste},
        {"Togo", Togo},
        {"Tonga", Tonga},
        {"Trinidad and Tobago", Trinidad_and_Tobago},
        {"Tunisia", Tunisia},
        {"Turkey", Turkey},
        {"Turkmenistan", Turkmenistan},
        {"Tuvalu", Tuvalu},
        {"Uganda", Uganda},
        {"Ukraine", Ukraine},
        {"United Arab Emirates", United_Arab_Emirates},
        {"United Kingdom", United_Kingdom},
        {"United States of America", United_States_of_America},
        {"Uruguay", Uruguay},
        {"Uzbekistan", Uzbekistan},
        {"Vanuatu", Vanuatu},
        {"Vatican City", Vatican_City},
        {"Venezuela", Venezuela},
        {"Vietnam", Vietnam},
        {"Yemen", Yemen},
        {"Zambia", Zambia},
        {"Zimbabwe", Zimbabwe}
    };

    auto it = countryMap.find(name);
    if (it != countryMap.end()) {
        return it->second;
    }
    else {
        return Country_Unknown;
    }
}