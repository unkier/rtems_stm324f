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

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/linker-symbols.h>
#include <stdint.h>
#include <bsp/system_stm32f4xx.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  /*SystemInit();*/
  SystemCoreClockUpdate();
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  /* At this point we can use objects outside the .start section */
}
