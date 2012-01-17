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



void USART_Init_With_Irq(USART_InitTypeDef *init, USART_PinsTypeDef *pins)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  NVIC_InitTypeDef NVIC_InitStructure;

  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = pins->irq;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  

  /* Enable UART clock */
  if ((pins->usart == USART1) || (pins->usart == USART6))
  {
    RCC_APB2PeriphClockCmd(pins->clk, ENABLE);
  }
  else
  {
    RCC_APB1PeriphClockCmd(pins->clk, ENABLE);
  }
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(pins->tx_port_clk | pins->rx_port_clk, ENABLE);

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(pins->tx_gpio_port,pins->tx_source,pins->tx_af);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(pins->rx_gpio_port,pins->rx_source,pins->rx_af);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = pins->tx_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(pins->tx_gpio_port, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = pins->rx_pin;
  GPIO_Init(pins->rx_gpio_port, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(pins->usart, init);
    
  /* Enable USART */
  USART_Cmd(pins->usart, ENABLE);

  USART_ITConfig(pins->usart,USART_IT_RXNE,ENABLE);
}


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
  
  _ARMV7M_Set_exception_handler(ARMV7M_VECTOR_IRQ(EXTI1_IRQn),usart_interrupt_handler);
    /*_ARMV7M_Set_exception_handler(ARMV7M_VECTOR_IRQ(USART6_IRQn),usart_interrupt_handler);*/

}

static int first_open(int major, int minor, void *arg)
{
  
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;

  console_tbl *ct = &Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;

  USART_Init_With_Irq(&USART_Init_struct[minor],&USART_Pins[minor]);

  return rtems_termios_set_initial_baud(tty, 115200);
}

static int last_close(int major, int minor, void *arg)
{
  return 0;
}

static int read_polled(int minor)
{
  return 0;
  /*return UARTCharGetNonBlocking(UART1_BASE);*/
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
