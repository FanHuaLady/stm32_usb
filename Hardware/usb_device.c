#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceHS;

void MX_USB_DEVICE_Init(void)
{
	uint16_t	 	USB_TimeOut;														// 超时判断时间
	uint32_t  		Tickstart;															// 计时起始时间
	
	if (USBD_Init(&hUsbDeviceHS, &FS_Desc, DEVICE_HS) != USBD_OK)						// USB初始化
	{
		Error_Handler();
	}
	if (USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK)
	{
		Error_Handler();
	}
	if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_FS) != USBD_OK)	// 这里的USBD_Interface_fops_FS包含很多函数
	{
		Error_Handler();
	}
	if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}

	HAL_PWREx_EnableUSBVoltageDetector();

 	USB_TimeOut = 1000;																	// 这里取1000ms
	Tickstart = HAL_GetTick();															// 获取systick当前时间
	while(  hUsbDeviceHS.dev_state != USBD_STATE_CONFIGURED )							// 等待USB建立连接
	{
		if((HAL_GetTick() - Tickstart) > USB_TimeOut) 									// 判断是否超时
		{
			break;																		// 若超时则跳出
		}
	}

	HAL_Delay(200);																		// 建立连接之后，延时等待一段时间，不然第一次发送的数据容易丢失		
	 
}
