/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_UART_H
#define LIBBSP_ARM_STM32F4_UART_H

#include <libchip/serial.h>
#include <bsp/utility.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART6
 */ 
#define EVAL_COM1                        USART6
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART6

#define EVAL_COM1_TX_PIN                 GPIO_Pin_6
#define EVAL_COM1_TX_GPIO_PORT           GPIOC
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource6
#define EVAL_COM1_TX_AF                  GPIO_AF_USART6

#define EVAL_COM1_RX_PIN                 GPIO_Pin_7
#define EVAL_COM1_RX_GPIO_PORT           GPIOC
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource7
#define EVAL_COM1_RX_AF                  GPIO_AF_USART6

#define EVAL_COM1_IRQn                   USART6_IRQn

/**
 * @brief Definition for COM port2, connected to USART3
 */ 
#define EVAL_COM2                        USART3
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART3

#define EVAL_COM2_TX_PIN                 GPIO_Pin_8
#define EVAL_COM2_TX_GPIO_PORT           GPIOD
#define EVAL_COM2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM2_TX_SOURCE              GPIO_PinSource8
#define EVAL_COM2_TX_AF                  GPIO_AF_USART3

#define EVAL_COM2_RX_PIN                 GPIO_Pin_9
#define EVAL_COM2_RX_GPIO_PORT           GPIOD
#define EVAL_COM2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM2_RX_SOURCE              GPIO_PinSource9
#define EVAL_COM2_RX_AF                  GPIO_AF_USART3

#define EVAL_COM2_IRQn                   USART3_IRQn

typedef struct
{
  USART_TypeDef* usart;
  uint8_t irq;
  uint32_t  clk;
  uint16_t  tx_pin;
  uint16_t  rx_pin;
  GPIO_TypeDef* tx_gpio_port;
  GPIO_TypeDef* rx_gpio_port;
  uint32_t  tx_port_clk;
  uint32_t  rx_port_clk;
  uint8_t tx_source;
  uint8_t rx_source;
  uint8_t tx_af;
  uint8_t rx_af;
} USART_PinsTypeDef;


typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;   



void COMPreInit(COM_TypeDef COM);

extern console_fns stm32f4_uart_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_STM32F4_UART_H */
