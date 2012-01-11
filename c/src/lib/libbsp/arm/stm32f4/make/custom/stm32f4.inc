#
#  Config file for STM32F4
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mstructure-size-boundary=8 -march=armv7-m -mthumb \
	-fno-schedule-insns2 -fno-common

CFLAGS_OPTIMIZE_V = -O0 -g3 -ggdb -g

define bsp-post-link
	$(default-bsp-post-link)
	$(OBJCOPY) -O binary $(basename $@).exe $(basename $@).ralf
endef
