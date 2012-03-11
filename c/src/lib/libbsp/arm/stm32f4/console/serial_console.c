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
 *
 * $Id$
 */

#include <bspopts.h>
#include <bsp/uart.h>
#include <libchip/sersupp.h>

#include <bsp/serial_console.h>
#include <rtems/score/armv7m.h>


uint8_t RxBuffer[0x20];
__IO uint16_t RxCounter = 0;

USART_InitTypeDef USART_Init_struct [] = 
{

  {
    .USART_BaudRate = 115200,
    .USART_WordLength = USART_WordLength_8b,
    .USART_StopBits = USART_StopBits_1,
    .USART_Parity = USART_Parity_No,
    .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None
  },
};

USART_PinsTypeDef USART_Pins [] =
{
  {
    .usart = USART6,
    .irq = USART6_IRQn,
    .clk = RCC_APB2Periph_USART6,
    .tx_pin = GPIO_Pin_6,
    .rx_pin = GPIO_Pin_7,
    .tx_gpio_port = GPIOC,
    .rx_gpio_port = GPIOC,
    .tx_port_clk = RCC_AHB1Periph_GPIOC,
    .rx_port_clk = RCC_AHB1Periph_GPIOC,
    .tx_source = GPIO_PinSource6,
    .rx_source = GPIO_PinSource7,
    .tx_af = GPIO_AF_USART6,
    .rx_af = GPIO_AF_USART6
  }
};

void LEDToggle1()
{
  GPIOD->ODR ^= GPIO_Pin_12;
}

static void usart_interrupt_handler(void)
{
  _ARMV7M_Interrupt_service_enter();
  GPIOD->ODR ^= GPIO_Pin_13;

  if(USART_GetITStatus(USART_Pins[0].usart, USART_IT_RXNE) != RESET)
  {
    
    RxBuffer[0] = USART_ReceiveData(USART_Pins[0].usart);
    USART_SendData(USART_Pins[0].usart,RxBuffer[0]);
    GPIOD->ODR ^= GPIO_Pin_14;
    
  }

  _ARMV7M_Interrupt_service_leave();
  
}


static void initialize(int minor)
{
  _ARMV7M_Set_exception_handler(ARMV7M_VECTOR_IRQ(USART6_IRQn),usart_interrupt_handler);
}

static int first_open(int major, int minor, void *arg)
{
  
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;

  console_tbl *ct = &Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;

  USART_Init_With_Irq(&USART_Init_struct[minor],&USART_Pins[minor]);

  return rtems_termios_set_initial_baud(tty, USART_Init_struct[minor].USART_BaudRate);
}

static int last_close(int major, int minor, void *arg)
{
  return 0;
}

static int read_polled(int minor)
{
  if (USART_GetFlagStatus(USART_Pins[minor].usart,USART_FLAG_RXNE) == SET)
  {
    return USART_ReceiveData(USART_Pins[minor].usart);
  }
  else
  {
    return -1;
  }
}

static void write_polled(int minor, char c)
{

  while(USART_GetFlagStatus(USART_Pins[minor].usart, USART_FLAG_TC) == RESET)
  {
  }

  USART_SendData(USART_Pins[minor].usart,c);
  LEDToggle1();
}

static ssize_t write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    write_polled(minor, s [i]);
  }

  return n;
}

static int set_attribues(int minor, const struct termios *term)
{
  return -1;
}

console_fns stm32f4_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = read_polled,
  .deviceWrite = write_support_polled,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attribues,
  .deviceOutputUsesInterrupts = false
};
