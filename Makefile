#******************************************************************************
#
# Makefile - Rules for building the code
#
# Copyright (c) 2012-2020 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 2.2.0.295 of the EK-TM4C123GXL Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM

#
# The base directory for TivaWare.
#
ROOT=./

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

# Definitions for the static analysis
CPPCHECK = cppcheck --enable=all --inconclusive --error-exitcode=1 --force
CLANG_TIDY = clang-tidy
ANALYSIS_SRC = led_pwm.c

#
# Where to find header files that do not live in the source directory.
#
IPATH=./

#
# The default rule, which causes the blinky example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/led_pwm.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the blinky example.
#
${COMPILER}/led_pwm.axf: ${COMPILER}/led_pwm.o
${COMPILER}/led_pwm.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/led_pwm.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/led_pwm.axf: led_pwm.ld
SCATTERgcc_led_pwm=led_pwm.ld
ENTRY_led_pwm=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif

# Static Analysis
static-analysis:
	$(CPPCHECK) $(ANALYSIS_SRC) --suppress=missingIncludeSystem --suppress=checkersReport
	$(CLANG_TIDY) $(ANALYSIS_SRC) -- -I$(ROOT)