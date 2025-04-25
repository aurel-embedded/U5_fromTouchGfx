/*
 * Jsonformatter.cpp
 *
 *  Created on: Apr 25, 2025
 *      Author: apajadon

 *  Description: Implementation of the JsonFormatter class.
 */

#include <PotarManager/Formatter/Jsonformatter.hpp>
#include <sstream>

std::string JsonFormatter::formatToJson(const userTypes_6Uint8_t& values) {
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

