/*
 * DataFormatter.cpp
 *
 *  Created on: Apr 25, 2025
 *      Author: apajadon

 *  Description: Implementation of the DataFormatter class.
 */

#include <PotarManager/Formatter/DataFormatter.hpp>
#include <sstream>

std::string DataFormatter::formatToJson(const userTypes_6Uint8_t& values) {
    std::ostringstream jsonStream;

    // Build the JSON string
    jsonStream << "{";
    jsonStream << "\"val1\":" << static_cast<int>(values.val1) << ",";
    jsonStream << "\"val2\":" << static_cast<int>(values.val2) << ",";
    jsonStream << "\"val3\":" << static_cast<int>(values.val3) << ",";
    jsonStream << "\"val4\":" << static_cast<int>(values.val4) << ",";
    jsonStream << "\"val5\":" << static_cast<int>(values.val5) << ",";
    jsonStream << "\"val6\":" << static_cast<int>(values.val6);
    jsonStream << "}";

    return jsonStream.str();
}

// Convert 6 potentiometer values into a CSV string
std::string DataFormatter::formatToCsv(const userTypes_6Uint8_t& values) {
    std::ostringstream csvStream;

    // Build the CSV string
    csvStream << static_cast<int>(values.val1) << ",";
    csvStream << static_cast<int>(values.val2) << ",";
    csvStream << static_cast<int>(values.val3) << ",";
    csvStream << static_cast<int>(values.val4) << ",";
    csvStream << static_cast<int>(values.val5) << ",";
    csvStream << static_cast<int>(values.val6);

    return csvStream.str();
}
