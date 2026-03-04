#include "usbd_cdc_if.h"
#include <stdarg.h>

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceHS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

// 这个函数用来注册接收区和发送区
static int8_t CDC_Init_FS(void)
{
  USBD_CDC_SetTxBuffer(&hUsbDeviceHS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceHS, UserRxBufferFS);
  return (USBD_OK);
}


static int8_t CDC_DeInit_FS(void)
{
	return (USBD_OK);
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
}

// 这个函数就是中断最终会调用的函数了
// 实际上 Buf 就是 UserRxBufferFS
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
	CDC_Transmit_FS(Buf, *Len);														// 回环测试
	
	USBD_CDC_SetRxBuffer(&hUsbDeviceHS, &Buf[0]);									// 重新设置接收缓冲区
	USBD_CDC_ReceivePacket(&hUsbDeviceHS);											// 通知 USB 内核，设备已准备好接收下一包数据
	return (USBD_OK);
}

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	uint8_t result = USBD_OK;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
	if (hcdc->TxState != 0)
	{
		return USBD_BUSY;
	}
	USBD_CDC_SetTxBuffer(&hUsbDeviceHS, Buf, Len);
	result = USBD_CDC_TransmitPacket(&hUsbDeviceHS);
	return result;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
	uint8_t result = USBD_OK;
	UNUSED(Buf);
	UNUSED(Len);
	UNUSED(epnum);
	return result;
}
 
void USB_printf(const char *format, ...)
{
	va_list args;																	// va_list型变量,指向参数的指针
	uint32_t length;																// 数据的长度
	uint16_t	 USB_TimeOut;														// 超时判断时间
	uint32_t  Tickstart;															// 计时起始时间
	
	va_start(args, format);															// 读取可变参数
	
	// 向字符串打印指定格式的数据，并获取最终的数据长度
	length = vsnprintf((char *)UserTxBufferFS, APP_TX_DATA_SIZE, (char *)format, args);
	
	va_end(args);																	// 结束可变参数的读取

	// 超时等待时间，这里取3ms，实测单次发送2K的数据将近2ms，用户可以根据实际需求进行调整
	USB_TimeOut = 3;

	if( hUsbDeviceHS.dev_state == USBD_STATE_CONFIGURED )							// 判断USB是否处于连接状态
	{
		// 获取相应的CDC状态
		USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
	
		Tickstart = HAL_GetTick();													// 获取当前时间
		while(hcdc->TxState !=0)													// 等待发送完成
		{
			if((HAL_GetTick() - Tickstart) > USB_TimeOut) 							// 达到超时时间
			{
				break;	// 跳出等待
			}
		}
	}		

	CDC_Transmit_FS(UserTxBufferFS, length); 										// 调用 USB CDC函数发送数据
		
}
