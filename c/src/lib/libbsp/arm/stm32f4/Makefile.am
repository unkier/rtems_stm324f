##
#
# @file
#
# @brief Makefile of LibBSP for the STM32F4 boards.
#

# $Id$

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
	include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/tod.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/uart.h
include_bsp_HEADERS += include/serial_console.h


include_HEADERS += ../../shared/include/tm27.h

include_bsp_HEADERS += peripheral/inc/core_cm4_simd.h
include_bsp_HEADERS += peripheral/inc/core_cmFunc.h
include_bsp_HEADERS += peripheral/inc/arm_math.h
include_bsp_HEADERS += peripheral/inc/misc.h
include_bsp_HEADERS += peripheral/inc/system_stm32f4xx.h
include_bsp_HEADERS += peripheral/inc/stm32f4xx.h
include_bsp_HEADERS += peripheral/inc/core_cm4.h
include_bsp_HEADERS += peripheral/inc/core_cmInstr.h
include_bsp_HEADERS += peripheral/inc/stm32f4xx_conf.h
include_bsp_HEADERS += peripheral/inc/stm32f4xx_rcc.h
include_bsp_HEADERS += peripheral/inc/stm32f4xx_gpio.h
include_bsp_HEADERS += peripheral/inc/stm32f4xx_usart.h



###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
EXTRA_DIST = startup/linkcmds.stm32f4

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppredriverhook.c \
	../../shared/bsppretaskinghook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../../shared/src/stackalloc.c

# Startup
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthook.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-copy-sections.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S


# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../../shared/src/irq-info.c \
	../../shared/src/irq-shell.c \
	../../shared/src/irq-server.c \
	irq/irq.c

# Uart
libbsp_a_SOURCES += uart/uart.c

# Console
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += ../../shared/console_control.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_select.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/serial_console.c

# Clock
libbsp_a_SOURCES += ../../../shared/clockdrv_shell.h \
    clock/clock-config.c

# Network
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
#noinst_PROGRAMS = network.rel
#network_rel_SOURCES = network/network.c
#network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
#network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
#libbsp_a_LIBADD += network.rel

# Peripheral
libbsp_a_SOURCES += peripheral/src/misc.c
libbsp_a_SOURCES += peripheral/src/system_stm32f4xx.c
libbsp_a_SOURCES += peripheral/src/stm32f4xx_usart.c
libbsp_a_SOURCES += peripheral/src/stm32f4xx_gpio.c
libbsp_a_SOURCES += peripheral/src/stm32f4xx_rcc.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c \
	../../../libcpu/arm/shared/cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include
libbsp_a_CPPFLAGS += -I$(srcdir)/peripheral/inc

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
