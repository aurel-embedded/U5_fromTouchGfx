/*
 * mdi_wrapper.cpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 */



#include "mdi_wrapper.h"
#include "mdi_thread.hpp" // Inclut la déclaration de la classe MDIThread

extern "C" {

// Wrapper for mdi_thread::init
HAL_StatusTypeDef MDI_Init(void)
{
    return mdi_thread::getInstance().init();
}

// Wrapper for mdi_thread::exit
HAL_StatusTypeDef MDI_Exit(void)
{
    return mdi_thread::getInstance().exit();
}

}
