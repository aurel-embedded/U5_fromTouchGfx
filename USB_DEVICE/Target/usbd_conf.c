/**
  ******************************************************************************
  * @file    usbd_conf_template.c
  * @author  MCD Application Team
  * @brief   USB Device configuration and interface file
  *          This template should be copied to the user folder,
  *          renamed and customized following user needs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_core.h"
#include "usbd_midi.h" /* Include class header file */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
/* Private function prototypes -----------------------------------------------*/
static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);
/* Private functions ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t *)hpcd->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;
  if ( hpcd->Init.speed != PCD_SPEED_FULL)
  {
    Error_Handler();
  }
    /* Set Speed. */
  USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);
  /* Reset Device. */
  USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}


/**
  * @brief  Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
	  /* Init USB Ip. */
	  /* Link the driver to the stack. */
	  hpcd_USB_OTG_HS.pData = pdev;
	  pdev->pData = &hpcd_USB_OTG_HS;

	  hpcd_USB_OTG_HS.Instance = USB_OTG_HS;
	  hpcd_USB_OTG_HS.Init.dev_endpoints = 9;
	  hpcd_USB_OTG_HS.Init.speed = PCD_SPEED_HIGH;
	  hpcd_USB_OTG_HS.Init.dma_enable = DISABLE;
	  hpcd_USB_OTG_HS.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	  hpcd_USB_OTG_HS.Init.Sof_enable = DISABLE;
	  hpcd_USB_OTG_HS.Init.low_power_enable = DISABLE;
	  hpcd_USB_OTG_HS.Init.lpm_enable = DISABLE;
	  hpcd_USB_OTG_HS.Init.vbus_sensing_enable = DISABLE;
	  hpcd_USB_OTG_HS.Init.use_dedicated_ep1 = DISABLE;
	  hpcd_USB_OTG_HS.Init.use_external_vbus = DISABLE;
	  if (HAL_PCD_Init(&hpcd_USB_OTG_HS) != HAL_OK)
	  {
		Error_Handler( );
	  }

	// Configurer les tailles des FIFOs pour l'USB OTG
	  HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_HS, 0x200);
	  HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_HS, 0, 0x80);
	  HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_HS, 1, 0x174);
  return USBD_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_DeInit(pdev->pData);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Starts the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_Start(pdev->pData);
    return  USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Stops the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_Stop(pdev->pData);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Opens an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  ep_type: Endpoint Type
  * @param  ep_mps: Endpoint Max Packet Size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                  uint8_t ep_type, uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Closes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev,
                                        uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;
    if((ep_addr & 0x80) == 0x80)
    {
           return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    }
    else
    {
           return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev,
                                         uint8_t dev_addr)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                    uint8_t *pbuf, uint32_t size)
{
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
    return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
                                          uint8_t ep_addr, uint8_t *pbuf,
                                          uint32_t size)
{
    HAL_StatusTypeDef hal_status;
     hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
     return USBD_Get_USB_Status(hal_status);
}

/**
  * @brief  Returns the last transferred packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Received Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

#ifdef USBD_HS_TESTMODE_ENABLE
/**
  * @brief  Set High speed Test mode.
  * @param  pdev: Device handle
  * @param  testmode: test mode
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_SetTestMode(USBD_HandleTypeDef *pdev, uint8_t testmode)
{
  UNUSED(pdev);
  UNUSED(testmode);

  return USBD_OK;
}
#endif /* USBD_HS_TESTMODE_ENABLE */

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size)
{
  UNUSED(size);
  static uint32_t mem[(sizeof(USBD_MIDI_HandleTypeDef) / 4) + 1]; /* On 32-bit boundary */
  return mem;
}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{
  UNUSED(p);
}

/**
  * @brief  Delays routine for the USB Device Library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);	// TODO: Use FreeRtos delay
}

USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
      USBD_StatusTypeDef usb_status = USBD_OK;
      switch (hal_status)
      {
      case HAL_OK :
             usb_status = USBD_OK;
             break;
      case HAL_ERROR :
             usb_status = USBD_FAIL;
             break;
      case HAL_BUSY :
             usb_status = USBD_BUSY;
             break;
      case HAL_TIMEOUT :
             usb_status = USBD_FAIL;
             break;
      default :
             usb_status = USBD_FAIL;
             break;
      }
      return usb_status;
}
