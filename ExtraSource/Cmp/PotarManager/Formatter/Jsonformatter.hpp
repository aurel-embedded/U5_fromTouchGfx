/*
 * Jsonformatter.h
 *
 *  Created on: Apr 25, 2025
 *      Author: apajadon
 *
 *  Description: A utility class for formatting data into JSON strings.
 */

#ifndef CMP_POTARMANAGER_FORMATTER_JSONFORMATTER_HPP_
#define CMP_POTARMANAGER_FORMATTER_JSONFORMATTER_HPP_
#include <Tools/UserTypes.h>
#include <string>

class JsonFormatter {
public:
    // Convert 6 potentiometer values into a JSON string
    static std::string formatToJson(const userTypes_6Uint8_t& values);
};

#endif /* CMP_POTARMANAGER_FORMATTER_JSONFORMATTER_HPP_ */
