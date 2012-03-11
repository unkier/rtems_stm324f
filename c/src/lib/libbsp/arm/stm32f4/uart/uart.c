#include <bsp/uart.h>

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