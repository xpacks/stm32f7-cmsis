/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        4. November 2016
 * $Revision:    V1.2
 *
 * Driver:       Driver_CAN1/2/3
 * Configured:   via RTE_Device.h configuration file
 * Project:      CAN Driver Header for ST STM32F7xx
 * -------------------------------------------------------------------------- */

#ifndef __CAN_STM32F7XX_H
#define __CAN_STM32F7XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_CAN.h"
#include "stm32f7xx_hal.h"

#include "RTE_Components.h"
#if    (defined(RTE_DEVICE_FRAMEWORK_CLASSIC))
#include "RTE_Device.h"
#elif  (defined(RTE_DEVICE_FRAMEWORK_CUBE_MX))
#include "MX_Device.h"
#else
#error "::Device:STM32Cube Framework: not selected in RTE"
#endif

#if    (defined(RTE_DEVICE_FRAMEWORK_CLASSIC))

#if   ((defined(RTE_Drivers_CAN1) ||  defined(RTE_Drivers_CAN2) ||  defined(RTE_Drivers_CAN3)) && \
      ((RTE_CAN1 == 0)            && (RTE_CAN2 == 0)            && (RTE_CAN3 == 0)))
#error "No CAN configured in RTE_Device.h!"
#endif

#if    (RTE_CAN1 != 0)
#define MX_CAN1                         1

/* Pin CAN1_RX */
#define MX_CAN1_RX_Pin                  1
#define MX_CAN1_RX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN1_RX_GPIOx                RTE_CAN1_RX_PORT
#define MX_CAN1_RX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN1_RX_GPIO_Pin             RTE_CAN1_RX_BIT
#define MX_CAN1_RX_GPIO_AF              GPIO_AF9_CAN1
#define MX_CAN1_RX_GPIO_Mode            GPIO_MODE_AF_PP

/* Pin CAN1_TX */
#define MX_CAN1_TX_Pin                  1
#define MX_CAN1_TX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN1_TX_GPIOx                RTE_CAN1_TX_PORT
#define MX_CAN1_TX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN1_TX_GPIO_Pin             RTE_CAN1_TX_BIT
#define MX_CAN1_TX_GPIO_AF              GPIO_AF9_CAN1
#define MX_CAN1_TX_GPIO_Mode            GPIO_MODE_AF_PP
#endif

#if    (RTE_CAN2 != 0)
#define MX_CAN2                         1

/* Pin CAN2_RX */
#define MX_CAN2_RX_Pin                  1
#define MX_CAN2_RX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN2_RX_GPIOx                RTE_CAN2_RX_PORT
#define MX_CAN2_RX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN2_RX_GPIO_Pin             RTE_CAN2_RX_BIT
#define MX_CAN2_RX_GPIO_AF              GPIO_AF9_CAN2
#define MX_CAN2_RX_GPIO_Mode            GPIO_MODE_AF_PP

/* Pin CAN2_TX */
#define MX_CAN2_TX_Pin                  1
#define MX_CAN2_TX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN2_TX_GPIOx                RTE_CAN2_TX_PORT
#define MX_CAN2_TX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN2_TX_GPIO_Pin             RTE_CAN2_TX_BIT
#define MX_CAN2_TX_GPIO_AF              GPIO_AF9_CAN2
#define MX_CAN2_TX_GPIO_Mode            GPIO_MODE_AF_PP
#endif

#if    (RTE_CAN3 != 0)
#define MX_CAN3                         1

/* Pin CAN3_RX */
#define MX_CAN3_RX_Pin                  1
#define MX_CAN3_RX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN3_RX_GPIOx                RTE_CAN3_RX_PORT
#define MX_CAN3_RX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN3_RX_GPIO_Pin             RTE_CAN3_RX_BIT
#define MX_CAN3_RX_GPIO_AF              GPIO_AF11_CAN3
#define MX_CAN3_RX_GPIO_Mode            GPIO_MODE_AF_PP

/* Pin CAN3_TX */
#define MX_CAN3_TX_Pin                  1
#define MX_CAN3_TX_GPIO_Speed           GPIO_SPEED_HIGH
#define MX_CAN3_TX_GPIOx                RTE_CAN3_TX_PORT
#define MX_CAN3_TX_GPIO_PuPd            GPIO_NOPULL
#define MX_CAN3_TX_GPIO_Pin             RTE_CAN3_TX_BIT
#define MX_CAN3_TX_GPIO_AF              GPIO_AF11_CAN3
#define MX_CAN3_TX_GPIO_Mode            GPIO_MODE_AF_PP
#endif

#elif  (defined(RTE_DEVICE_FRAMEWORK_CUBE_MX))

#if   ((defined(RTE_Drivers_CAN1) ||   defined(RTE_Drivers_CAN2) ||   defined(RTE_Drivers_CAN3)) && \
      (!defined(MX_CAN1))         && (!defined(MX_CAN2))         && (!defined(MX_CAN3)))
#error "No CAN configured in STM32CubeMX!"
#endif

#endif

#if    !defined(MX_CAN1)
#define MX_CAN1                         (0U)
#endif
#if    !defined(MX_CAN2)
#define MX_CAN2                         (0U)
#endif
#if    !defined(MX_CAN3)
#define MX_CAN3                         (0U)
#endif

#if    (MX_CAN3 == 1U)
#define CAN_CTRL_NUM                    (3U)
#elif  (MX_CAN2 == 1U)
#define CAN_CTRL_NUM                    (2U)
#else
#define CAN_CTRL_NUM                    (1U)
#endif

#endif // __CAN_STM32F7XX_H
