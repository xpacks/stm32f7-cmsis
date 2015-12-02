#
# Copyright (c) 2015 Liviu Ionescu
# This file is part of the xPacks project (https://xpacks.github.io).
#
# Cross compile the CMSIS Core files with GCC.
#
# Input: (must be set by the caller)
#   PARENT=project root folder
#   TEST_NAME=test name
#

include arm-none-eabi.mk

SUBFAMILY=stm32f745xx
DEFINES=-DSTM32F745xx

all:			$(SUBFAMILY)

$(SUBFAMILY):	vectors_$(SUBFAMILY).o main.o

vectors_$(SUBFAMILY).o:	vectors_$(SUBFAMILY).c

main.o:			main.c

vectors_$(SUBFAMILY).c:
	cp "$(PARENT)/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/$@" "$@"



.PHONY:			all $(SUBFAMILY)

	