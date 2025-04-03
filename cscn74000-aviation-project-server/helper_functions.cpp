#include "../cscn74000-aviation-project-client/helper_functions.h"


// Function to find a file in a directory that matches the size and content of the input file
std::string findMatchingFile(const std::string& filePath, const std::string& directoryPath) {
    // Open the input file
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile) {
        return ""; // Return empty string if file cannot be opened
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
                    std::string result = std::filesystem::path(entry.path()).string();
                    delete[] inputBuffer;
                    return result;
                }
            }
        }
    }

    // No match found
    delete[] inputBuffer;
    return "";
}