/*
 * Utilities.hpp
 *
 *  Created on: Dec 20, 2024
 *      Author: apajadon
 */

#ifndef TOOLS_UTILITIES_HPP_
#define TOOLS_UTILITIES_HPP_



template <typename T>
T clamp(const T& value, const T& low, const T& high)
{
    return (value < low) ? low : (value > high ? high : value);
}

#endif /* TOOLS_UTILITIES_HPP_ */
