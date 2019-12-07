/**
 ******************************************************************************
 * @file    console.c
 * @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
 * @brief   This file provides implementation of standard input/output
 ******************************************************************************
 * @attention
 *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
 
#include <stdio.h>
#include <stdlib.h>

#include "stm32l0xx_hal.h"
#include "console.h"

static UART_HandleTypeDef UartHandle;

void initConsole(void) {
  if(HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_RESET) {  
		__USART1_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();
		
		GPIO_InitTypeDef GPIO_InitStruct;

		/* Configure peripheral GPIO */
		/* UART TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = GPIO_PIN_9;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* UART RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* UART configuration */
		UartHandle.Instance        = USART1;
		UartHandle.Init.BaudRate   = 115200;
		UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
		UartHandle.Init.StopBits   = UART_STOPBITS_1;
		UartHandle.Init.Parity     = UART_PARITY_NONE;
		UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		UartHandle.Init.Mode       = UART_MODE_TX_RX;
		UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
		HAL_UART_Init(&UartHandle);
  }
}

/** @brief Sends a character to serial port
 * @param ch Character to send
 * @retval Character sent
 */
int uartSendChar(int ch)
{
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/** @brief Receives a character from serial port
 * @param None
 * @retval Character received
 */
int uartReceiveChar(void)
{
  uint8_t ch;
  HAL_UART_Receive(&UartHandle, &ch, 1, HAL_MAX_DELAY-1);
  
//  /* Echo character back to console */
//  HAL_UART_Transmit(&UartHandle, &ch, 1, HAL_MAX_DELAY);
//
//  /* And cope with Windows */
//  if(ch == '\r'){
//    uint8_t ret = '\n';
//    HAL_UART_Transmit(&UartHandle, &ret, 1, HAL_MAX_DELAY);
//  }

  return ch;
}


#if defined (__IAR_SYSTEMS_ICC__)

size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize);
size_t __read(int Handle, unsigned char *Buf, size_t Bufsize);

/** @brief IAR specific low level standard input
 * @param Handle IAR internal handle
 * @param Buf Buffer where to store characters read from stdin
 * @param Bufsize Number of characters to read
 * @retval Number of characters read
 */
size_t __read(int Handle, unsigned char *Buf, size_t Bufsize)
{
  int i;

  if (Handle != 0){
    return -1;
  }

  for(i=0; i<Bufsize; i++)
    Buf[i] = uartReceiveChar();

  return Bufsize;
}

/** @brief IAR specific low level standard output
 * @param Handle IAR internal handle
 * @param Buf Buffer containing characters to be written to stdout
 * @param Bufsize Number of characters to write
 * @retval Number of characters read
 */
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
  int i;

  if (Handle != 1 && Handle != 2){
    return -1;
  }

  for(i=0; i< Bufsize; i++)
    uartSendChar(Buf[i]);

  return Bufsize;
}

#elif defined (__CC_ARM)

/**
 * @brief fputc call for standard output implementation
 * @param ch Character to print
 * @param f File pointer
 * @retval Character printed
 */
int fputc(int ch, FILE *f)
{
  return uartSendChar(ch);
}

/** @brief fgetc call for standard input implementation
 * @param f File pointer
 * @retval Character acquired from standard input
 */
int fgetc(FILE *f)
{
  return uartReceiveChar();
}

#elif defined (__GNUC__)

/** @brief putchar call for standard output implementation
 * @param ch Character to print
 * @retval Character printed
 */
int __io_putchar(int ch)
{
  return uartSendChar(ch);
}

/** @brief getchar call for standard input implementation
 * @param None
 * @retval Character acquired from standard input
 */
int __io_getchar(void)
{
  return uartReceiveChar();
}


/**
 * @}
 */

#else
#error "Toolchain not supported"
#endif

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
