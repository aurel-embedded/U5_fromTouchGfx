/*
 * DataF.h
 *
 *  Created on: Apr 25, 2025
 *      Author: apajadon
 *
 *  Description: A utility class for formatting data into Data strings.
 */

#ifndef CMP_POTARMANAGER_FORMATTER_DATAFORMATTER_HPP_
#define CMP_POTARMANAGER_FORMATTER_DATAFORMATTER_HPP_
#include <Tools/UserTypes.h>
#include <string>

class DataFormatter {
public:
    // Convert 6 potentiometer values into a JSON string
    static std::string formatToJson(const userTypes_6Uint8_t& values);

    // Convert 6 potentiometer values into a CSV string
    static std::string formatToCsv(const userTypes_6Uint8_t& values);

};

#endif /* CMP_POTARMANAGER_FORMATTER_DATAFORMATTER_HPP_ */
