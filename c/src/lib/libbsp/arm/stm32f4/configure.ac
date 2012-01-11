##
#
# @file
#
# @brief Configure script of LibBSP for the LM3S39XX boards.
#

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-arm-lm3s89xx],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([BSP_SMALL_MEMORY],[*],[])
RTEMS_BSPOPTS_HELP([BSP_SMALL_MEMORY],[disable testsuite samples with high memory demands])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)
RTEMS_BSP_LINKCMDS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
