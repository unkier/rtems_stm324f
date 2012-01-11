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



USART_TypeDef* COM_USART[COMn] = {EVAL_COM1,EVAL_COM2}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT,EVAL_COM2_RX_GPIO_PORT};

const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK,EVAL_COM2_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK,EVAL_COM2_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};
 
const uint16_t COM_TX_PIN_SOURCE[COMn] = {EVAL_COM1_TX_SOURCE, EVAL_COM2_TX_SOURCE};

const uint16_t COM_RX_PIN_SOURCE[COMn] = {EVAL_COM1_RX_SOURCE, EVAL_COM1_RX_SOURCE};
 
const uint16_t COM_TX_AF[COMn] = {EVAL_COM1_TX_AF, EVAL_COM2_TX_AF};
 
const uint16_t COM_RX_AF[COMn] = {EVAL_COM1_RX_AF, EVAL_COM2_RX_AF};


USART_InitTypeDef USART_Init_struct [] = 
{
  {
    115200,
    USART_WordLength_8b,
    USART_StopBits_1,
    USART_Mode_Rx | USART_Mode_Tx,
    USART_HardwareFlowControl_None
  },
  {
    115200,
    USART_WordLength_8b,
    USART_StopBits_1,
    USART_Mode_Rx | USART_Mode_Tx,
    USART_HardwareFlowControl_None
  }

};


void COMPreInit(COM_TypeDef COM)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

  if (COM == COM1)
  {
    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_PIN_SOURCE[COM], COM_TX_AF[COM]);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_RX_AF[COM]);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  
}

static void initialize(int minor)
{
  COMPreInit(minor);
}

static int first_open(int major, int minor, void *arg)
{
  
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;

  console_tbl *ct = &Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;

  /* USART configuration */
  USART_Init(COM_USART[minor], &USART_Init_struct[minor]);
    
  /* Enable USART */
  USART_Cmd(COM_USART[minor], ENABLE);

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

  while(USART_GetFlagStatus(COM_USART[minor], USART_FLAG_TXE) == RESET)
  {
  }

  USART_SendData(COM_USART[minor],c);
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

static void my_driver_interrupt_handler(rtems_vector_number vector,void *arg)
{
  
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
