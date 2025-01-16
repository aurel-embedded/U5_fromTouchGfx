/*
 * pmgr_wrapper.cpp
 *
 *  Created on: Jan 6, 2025
 *      Author: apajadon
 */


#include "pmgr_wrapper.h"
#include "pmgr_thread.hpp"

extern "C" {

// Wrapper for pmgr_thread::init
HAL_StatusTypeDef PMGR_Init(void)
{
    return pmgr_thread::getInstance().init();
}

// Wrapper for pmgr_thread::exit
HAL_StatusTypeDef PMGR_Exit(void)
{
    return pmgr_thread::getInstance().exit();
}

}


