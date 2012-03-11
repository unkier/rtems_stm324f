#ifndef LIBBSP_ARM_STM32F4_SERIAL_USART_H
#define LIBBSP_ARM_STM32F4_SERIAL_USART_H

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


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_STM32F4_SERIAL_USART_H */
