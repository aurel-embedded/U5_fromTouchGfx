/*
 * printf.c
 *
 *  Created on: Sep 26, 2024
 *      Author: apajadon
 */
#include "main.h"
#include <stdint.h>
#include <stm32u5xx_hal_uart.h>
#include <Tools/Printf/printf.h>

extern UART_HandleTypeDef huart1;

// Printf On Uart
//----------------
//------------------------------------------------------------------------------
/// \fn         int _write(int file, char *ptr, int len)
/// \brief      Low-level function for writing data to  UART interfaces.
///				Linked with `printf` for custom I/O handling.
///
/// \param[in]  file: File descriptor.
/// \param[in]  ptr:  Pointer to the buffer holding the data to be written.
/// \param[in]  len:  Number of bytes to write from the buffer.
///
/// \return     int:  Number of bytes written, or -1 in case of error.
//------------------------------------------------------------------------------
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

