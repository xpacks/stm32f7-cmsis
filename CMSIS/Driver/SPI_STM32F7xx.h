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
 * $Date:        30. June 2015
 * $Revision:    V1.0
 *
 * Project:      SPI Driver definitions for ST STM32F7xx
 * -------------------------------------------------------------------------- */

#ifndef __SPI_STM32F4XX_H
#define __SPI_STM32F4XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_SPI.h"
#include "stm32f7xx_hal.h"

#include "RTE_Components.h"

#ifndef   RTE_DEVICE_FRAMEWORK_CLASSIC
#include "MX_Device.h"

// MX macros

#else
#include "RTE_Device.h"

// RTE macros
#define _DMA_CHANNEL_x(x)               DMA_CHANNEL_##x
#define  DMA_CHANNEL_x(x)              _DMA_CHANNEL_x(x)

#define  DMA_PRIORITY(x)              ((x == 0) ? DMA_PRIORITY_LOW    : \
                                       (x == 1) ? DMA_PRIORITY_MEDIUM : \
                                       (x == 2) ? DMA_PRIORITY_HIGH   : \
                                                  DMA_PRIORITY_VERY_HIGH)

#define _DMAx_STREAMy(x, y)             DMA##x##_Stream##y
#define  DMAx_STREAMy(x, y)            _DMAx_STREAMy(x, y)

#define _DMAx_STREAMy_IRQ(x, y)         DMA##x##_Stream##y##_IRQHandler
#define  DMAx_STREAMy_IRQ(x, y)        _DMAx_STREAMy_IRQ(x, y)

#define _DMAx_STREAMy_IRQn(x, y)        DMA##x##_Stream##y##_IRQn
#define  DMAx_STREAMy_IRQn(x, y)       _DMAx_STREAMy_IRQn(x, y)

// SPI1 configuration definitions
#if (RTE_SPI1 == 1)
  #define MX_SPI1

  #if (RTE_SPI1_RX_DMA == 1)
    #define MX_SPI1_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_STREAM)
    #define MX_SPI1_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_STREAM)
    #define MX_SPI1_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI1_RX_DMA_CHANNEL)
    #define MX_SPI1_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI1_RX_DMA_PRIORITY)

    #define SPI1_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI1_TX_DMA == 1)
    #define MX_SPI1_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_STREAM)
    #define MX_SPI1_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_STREAM)
    #define MX_SPI1_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI1_TX_DMA_CHANNEL)
    #define MX_SPI1_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI1_TX_DMA_PRIORITY)

    #define SPI1_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_STREAM)
  #endif

  #define MX_SPI1_MISO_Pin            1U
  #define MX_SPI1_MISO_GPIOx          RTE_SPI1_MISO_PORT
  #define MX_SPI1_MISO_GPIO_Pin      (1U << RTE_SPI1_MISO_BIT)
  #define MX_SPI1_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI1_MISO_GPIO_AF        GPIO_AF5_SPI1

  #define MX_SPI1_MOSI_Pin            1U
  #define MX_SPI1_MOSI_GPIOx          RTE_SPI1_MOSI_PORT
  #define MX_SPI1_MOSI_GPIO_Pin      (1U << RTE_SPI1_MOSI_BIT)
  #define MX_SPI1_MOSI_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI1_MOSI_GPIO_AF        GPIO_AF5_SPI1

  #define MX_SPI1_SCK_Pin             1U
  #define MX_SPI1_SCK_GPIOx           RTE_SPI1_SCL_PORT
  #define MX_SPI1_SCK_GPIO_Pin       (1U << RTE_SPI1_SCL_BIT)
  #define MX_SPI1_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI1_SCK_GPIO_AF         GPIO_AF5_SPI1

  #if (RTE_SPI1_NSS_PIN == 1)
    #define MX_SPI1_NSS_Pin           1U
    #define MX_SPI1_NSS_GPIOx         RTE_SPI1_NSS_PORT
    #define MX_SPI1_NSS_GPIO_Pin     (1U << RTE_SPI1_NSS_BIT)
    #define MX_SPI1_NSS_GPIO_AF       GPIO_AF5_SPI1
  #endif
#endif

// SPI2 configuration definitions
#if (RTE_SPI2 == 1)
  #define MX_SPI2

  #if (RTE_SPI2_RX_DMA == 1)
    #define MX_SPI2_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_STREAM)
    #define MX_SPI2_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_STREAM)
    #define MX_SPI2_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI2_RX_DMA_CHANNEL)
    #define MX_SPI2_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI2_RX_DMA_PRIORITY)

    #define SPI2_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI2_TX_DMA == 1)
    #define MX_SPI2_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_STREAM)
    #define MX_SPI2_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_STREAM)
    #define MX_SPI2_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI2_TX_DMA_CHANNEL)
    #define MX_SPI2_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI2_TX_DMA_PRIORITY)

    #define SPI2_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_STREAM)
  #endif

  #define MX_SPI2_MISO_Pin            1U
  #define MX_SPI2_MISO_GPIOx          RTE_SPI2_MISO_PORT
  #define MX_SPI2_MISO_GPIO_Pin      (1U << RTE_SPI2_MISO_BIT)
  #define MX_SPI2_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI2_MISO_GPIO_AF        GPIO_AF5_SPI2

  #define MX_SPI2_MOSI_Pin            1U
  #define MX_SPI2_MOSI_GPIOx          RTE_SPI2_MOSI_PORT
  #define MX_SPI2_MOSI_GPIO_Pin      (1U << RTE_SPI2_MOSI_BIT)
  #define MX_SPI2_MOSI_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI2_MOSI_GPIO_AF        GPIO_AF5_SPI2

  #define MX_SPI2_SCK_Pin             1U
  #define MX_SPI2_SCK_GPIOx           RTE_SPI2_SCL_PORT
  #define MX_SPI2_SCK_GPIO_Pin       (1U << RTE_SPI2_SCL_BIT)
  #define MX_SPI2_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI2_SCK_GPIO_AF         GPIO_AF5_SPI2

  #if (RTE_SPI2_NSS_PIN == 1)
    #define MX_SPI2_NSS_Pin           1U
    #define MX_SPI2_NSS_GPIOx         RTE_SPI2_NSS_PORT
    #define MX_SPI2_NSS_GPIO_Pin     (1U << RTE_SPI2_NSS_BIT)
    #if ((RTE_SPI2_NSS_PORT == GPIOB) && (RTE_SPI2_NSS_BIT == 4))
      #define MX_SPI2_NSS_GPIO_AF     GPIO_AF7_SPI2
    #else
      #define MX_SPI2_NSS_GPIO_AF     GPIO_AF5_SPI2
    #endif
  #endif
#endif

// SPI3 configuration definitions
#if (RTE_SPI3 == 1)
  #define MX_SPI3

  #if (RTE_SPI3_RX_DMA == 1)
    #define MX_SPI3_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_STREAM)
    #define MX_SPI3_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_STREAM)
    #define MX_SPI3_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI3_RX_DMA_CHANNEL)
    #define MX_SPI3_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI3_RX_DMA_PRIORITY)

    #define SPI3_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI3_TX_DMA == 1)
    #define MX_SPI3_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_STREAM)
    #define MX_SPI3_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_STREAM)
    #define MX_SPI3_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI3_TX_DMA_CHANNEL)
    #define MX_SPI3_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI3_TX_DMA_PRIORITY)

    #define SPI3_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_STREAM)
  #endif

  #define MX_SPI3_MISO_Pin            1U
  #define MX_SPI3_MISO_GPIOx          RTE_SPI3_MISO_PORT
  #define MX_SPI3_MISO_GPIO_Pin      (1U << RTE_SPI3_MISO_BIT)
  #define MX_SPI3_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI3_MISO_GPIO_AF        GPIO_AF6_SPI3

  #define MX_SPI3_MOSI_Pin            1U
  #define MX_SPI3_MOSI_GPIOx          RTE_SPI3_MOSI_PORT
  #define MX_SPI3_MOSI_GPIO_Pin      (1U << RTE_SPI3_MOSI_BIT)
  #define MX_SPI3_MOSI_GPIO_PuPd      GPIO_NOPULL
  #if   ((RTE_SPI3_MOSI_PORT == GPIOB) && (RTE_SPI3_MOSI_BIT == 2))
    #define MX_SPI3_MOSI_GPIO_AF      GPIO_AF7_SPI3
  #elif ((RTE_SPI3_MOSI_PORT == GPIOD) && (RTE_SPI3_MOSI_BIT == 6))
    #define MX_SPI3_MOSI_GPIO_AF      GPIO_AF5_SPI3
  #else
    #define MX_SPI3_MOSI_GPIO_AF      GPIO_AF6_SPI3
  #endif

  #define MX_SPI3_SCK_Pin             1U
  #define MX_SPI3_SCK_GPIOx           RTE_SPI3_SCL_PORT
  #define MX_SPI3_SCK_GPIO_Pin       (1U << RTE_SPI3_SCL_BIT)
  #define MX_SPI3_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI3_SCK_GPIO_AF         GPIO_AF6_SPI3

  #if (RTE_SPI3_NSS_PIN == 1)
    #define MX_SPI3_NSS_Pin           1U
    #define MX_SPI3_NSS_GPIOx         RTE_SPI3_NSS_PORT
    #define MX_SPI3_NSS_GPIO_Pin     (1U << RTE_SPI3_NSS_BIT)
    #define MX_SPI3_NSS_GPIO_AF       GPIO_AF6_SPI3
  #endif
#endif

// SPI4 configuration definitions
#if (RTE_SPI4 == 1)

  #ifndef SPI4
    #error "SPI4 not available for selected device!"
  #endif
  
  #define MX_SPI4

  #if (RTE_SPI4_RX_DMA == 1)
    #if ((RTE_SPI4_RX_DMA_STREAM == 0) && (RTE_SPI4_RX_DMA_CHANNEL == 5))
      #error "SPI4 RX DMA Configuration error: Stream 0 requires Channel 4."
    #endif
    #if ((RTE_SPI4_RX_DMA_STREAM == 3) && (RTE_SPI4_RX_DMA_CHANNEL == 4))
      #error "SPI4 RX DMA Configuration error: Stream 3 requires Channel 5."
    #endif
    #define MX_SPI4_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI4_RX_DMA_NUMBER, RTE_SPI4_RX_DMA_STREAM)
    #define MX_SPI4_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI4_RX_DMA_NUMBER, RTE_SPI4_RX_DMA_STREAM)
    #define MX_SPI4_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI4_RX_DMA_CHANNEL)
    #define MX_SPI4_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI4_RX_DMA_PRIORITY)

    #define SPI4_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI4_RX_DMA_NUMBER, RTE_SPI4_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI4_TX_DMA == 1)
    #if ((RTE_SPI4_TX_DMA_STREAM == 1) && (RTE_SPI4_TX_DMA_CHANNEL == 5))
      #error "SPI4 TX DMA Configuration error: Stream 1 requires Channel 4."
    #endif
    #if ((RTE_SPI4_TX_DMA_STREAM == 4) && (RTE_SPI4_TX_DMA_CHANNEL == 4))
      #error "SPI4 TX DMA Configuration error: Stream 4 requires Channel 5."
    #endif
    #define MX_SPI4_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI4_TX_DMA_NUMBER, RTE_SPI4_TX_DMA_STREAM)
    #define MX_SPI4_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI4_TX_DMA_NUMBER, RTE_SPI4_TX_DMA_STREAM)
    #define MX_SPI4_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI4_TX_DMA_CHANNEL)
    #define MX_SPI4_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI4_TX_DMA_PRIORITY)

    #define SPI4_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI4_TX_DMA_NUMBER, RTE_SPI4_TX_DMA_STREAM)
  #endif

  #define MX_SPI4_MISO_Pin            1U
  #define MX_SPI4_MISO_GPIOx          RTE_SPI4_MISO_PORT
  #define MX_SPI4_MISO_GPIO_Pin      (1U << RTE_SPI4_MISO_BIT)
  #define MX_SPI4_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI4_MISO_GPIO_AF        GPIO_AF5_SPI4

  #define MX_SPI4_MOSI_Pin            1U
  #define MX_SPI4_MOSI_GPIOx          RTE_SPI4_MOSI_PORT
  #define MX_SPI4_MOSI_GPIO_Pin      (1U << RTE_SPI4_MOSI_BIT)
  #define MX_SPI4_MOSI_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI4_MOSI_GPIO_AF        GPIO_AF5_SPI4

  #define MX_SPI4_SCK_Pin             1U
  #define MX_SPI4_SCK_GPIOx           RTE_SPI4_SCL_PORT
  #define MX_SPI4_SCK_GPIO_Pin       (1U << RTE_SPI4_SCL_BIT)
  #define MX_SPI4_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI4_SCK_GPIO_AF         GPIO_AF5_SPI4

  #if (RTE_SPI4_NSS_PIN == 1)
    #define MX_SPI4_NSS_Pin           1U
    #define MX_SPI4_NSS_GPIOx         RTE_SPI4_NSS_PORT
    #define MX_SPI4_NSS_GPIO_Pin     (1U << RTE_SPI4_NSS_BIT)
    #define MX_SPI4_NSS_GPIO_AF       GPIO_AF5_SPI4
  #endif
#endif

// SPI5 configuration definitions
#if (RTE_SPI5 == 1)

  #ifndef SPI5
    #error "SPI5 not available for selected device!"
  #endif

  #define MX_SPI5

  #if (RTE_SPI5_RX_DMA == 1)
    #if ((RTE_SPI5_RX_DMA_STREAM == 3) && (RTE_SPI5_RX_DMA_CHANNEL == 7))
      #error "SPI5 RX DMA Configuration error: Stream 3 requires Channel 2."
    #endif
    #if ((RTE_SPI5_RX_DMA_STREAM == 5) && (RTE_SPI5_RX_DMA_CHANNEL == 2))
      #error "SPI5 RX DMA Configuration error: Stream 5 requires Channel 7."
    #endif
    #define MX_SPI5_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI5_RX_DMA_NUMBER, RTE_SPI5_RX_DMA_STREAM)
    #define MX_SPI5_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI5_RX_DMA_NUMBER, RTE_SPI5_RX_DMA_STREAM)
    #define MX_SPI5_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI5_RX_DMA_CHANNEL)
    #define MX_SPI5_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI5_RX_DMA_PRIORITY)

    #define SPI5_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI5_RX_DMA_NUMBER, RTE_SPI5_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI5_TX_DMA == 1)
    #if ((RTE_SPI5_TX_DMA_STREAM == 4) && (RTE_SPI5_TX_DMA_CHANNEL == 7))
      #error "SPI5 TX DMA Configuration error: Stream 4 requires Channel 2."
    #endif
    #if ((RTE_SPI5_TX_DMA_STREAM == 6) && (RTE_SPI5_TX_DMA_CHANNEL == 2))
      #error "SPI5 TX DMA Configuration error: Stream 6 requires Channel 7."
    #endif
    #define MX_SPI5_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI5_TX_DMA_NUMBER, RTE_SPI5_TX_DMA_STREAM)
    #define MX_SPI5_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI5_TX_DMA_NUMBER, RTE_SPI5_TX_DMA_STREAM)
    #define MX_SPI5_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI5_TX_DMA_CHANNEL)
    #define MX_SPI5_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI5_TX_DMA_PRIORITY)

    #define SPI5_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI5_TX_DMA_NUMBER, RTE_SPI5_TX_DMA_STREAM)
  #endif

  #define MX_SPI5_MISO_Pin            1U
  #define MX_SPI5_MISO_GPIOx          RTE_SPI5_MISO_PORT
  #define MX_SPI5_MISO_GPIO_Pin      (1U << RTE_SPI5_MISO_BIT)
  #define MX_SPI5_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI5_MISO_GPIO_AF        GPIO_AF5_SPI5

  #define MX_SPI5_MOSI_Pin            1U
  #define MX_SPI5_MOSI_GPIOx          RTE_SPI5_MOSI_PORT
  #define MX_SPI5_MOSI_GPIO_Pin      (1U << RTE_SPI5_MOSI_BIT)
  #define MX_SPI5_MOSI_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI5_MOSI_GPIO_AF        GPIO_AF5_SPI5

  #define MX_SPI5_SCK_Pin             1U
  #define MX_SPI5_SCK_GPIOx           RTE_SPI5_SCL_PORT
  #define MX_SPI5_SCK_GPIO_Pin       (1U << RTE_SPI5_SCL_BIT)
  #define MX_SPI5_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI5_SCK_GPIO_AF         GPIO_AF5_SPI5

  #if (RTE_SPI5_NSS_PIN == 1)
    #define MX_SPI5_NSS_Pin           1U
    #define MX_SPI5_NSS_GPIOx         RTE_SPI5_NSS_PORT
    #define MX_SPI5_NSS_GPIO_Pin     (1U << RTE_SPI5_NSS_BIT)
    #define MX_SPI5_NSS_GPIO_AF       GPIO_AF5_SPI5
  #endif
#endif

// SPI6 configuration definitions
#if (RTE_SPI6 == 1)

  #ifndef SPI6
    #error "SPI6 not available for selected device!"
  #endif

  #define MX_SPI6

  #if (RTE_SPI6_RX_DMA == 1)
    #define MX_SPI6_RX_DMA_Instance   DMAx_STREAMy(RTE_SPI6_RX_DMA_NUMBER, RTE_SPI6_RX_DMA_STREAM)
    #define MX_SPI6_RX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI6_RX_DMA_NUMBER, RTE_SPI6_RX_DMA_STREAM)
    #define MX_SPI6_RX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI6_RX_DMA_CHANNEL)
    #define MX_SPI6_RX_DMA_Priority   DMA_PRIORITY(RTE_SPI6_RX_DMA_PRIORITY)

    #define SPI6_RX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI6_RX_DMA_NUMBER, RTE_SPI6_RX_DMA_STREAM)
  #endif
  #if (RTE_SPI6_TX_DMA == 1)
    #define MX_SPI6_TX_DMA_Instance   DMAx_STREAMy(RTE_SPI6_TX_DMA_NUMBER, RTE_SPI6_TX_DMA_STREAM)
    #define MX_SPI6_TX_DMA_IRQn       DMAx_STREAMy_IRQn(RTE_SPI6_TX_DMA_NUMBER, RTE_SPI6_TX_DMA_STREAM)
    #define MX_SPI6_TX_DMA_Channel    DMA_CHANNEL_x(RTE_SPI6_TX_DMA_CHANNEL)
    #define MX_SPI6_TX_DMA_Priority   DMA_PRIORITY(RTE_SPI6_TX_DMA_PRIORITY)

    #define SPI6_TX_DMA_Handler       DMAx_STREAMy_IRQ(RTE_SPI6_TX_DMA_NUMBER, RTE_SPI6_TX_DMA_STREAM)
  #endif

  #define MX_SPI6_MISO_Pin            1U
  #define MX_SPI6_MISO_GPIOx          RTE_SPI6_MISO_PORT
  #define MX_SPI6_MISO_GPIO_Pin      (1U << RTE_SPI6_MISO_BIT)
  #define MX_SPI6_MISO_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI6_MISO_GPIO_AF        GPIO_AF5_SPI6

  #define MX_SPI6_MOSI_Pin            1U
  #define MX_SPI6_MOSI_GPIOx          RTE_SPI6_MOSI_PORT
  #define MX_SPI6_MOSI_GPIO_Pin      (1U << RTE_SPI6_MOSI_BIT)
  #define MX_SPI6_MOSI_GPIO_PuPd      GPIO_NOPULL
  #define MX_SPI6_MOSI_GPIO_AF        GPIO_AF5_SPI6

  #define MX_SPI6_SCK_Pin             1U
  #define MX_SPI6_SCK_GPIOx           RTE_SPI6_SCL_PORT
  #define MX_SPI6_SCK_GPIO_Pin       (1U << RTE_SPI6_SCL_BIT)
  #define MX_SPI6_SCK_GPIO_PuPd       GPIO_NOPULL
  #define MX_SPI6_SCK_GPIO_AF         GPIO_AF5_SPI6

  #if (RTE_SPI6_NSS_PIN == 1)
    #define MX_SPI6_NSS_Pin           1U
    #define MX_SPI6_NSS_GPIOx         RTE_SPI6_NSS_PORT
    #define MX_SPI6_NSS_GPIO_Pin     (1U << RTE_SPI6_NSS_BIT)
    #define MX_SPI6_NSS_GPIO_AF       GPIO_AF5_SPI6
  #endif
#endif
#endif


#ifdef MX_SPI1
#if (defined(MX_SPI1_RX_DMA_Instance) || defined(MX_SPI1_TX_DMA_Instance))
#ifndef MX_SPI1_RX_DMA_Instance
  #error "SPI1 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI1_TX_DMA_Instance
  #error "SPI1 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI2
#if (defined(MX_SPI2_RX_DMA_Instance) || defined(MX_SPI2_TX_DMA_Instance))
#ifndef MX_SPI2_RX_DMA_Instance
  #error "SPI2 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI2_TX_DMA_Instance
  #error "SPI2 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI3
#if (defined(MX_SPI3_RX_DMA_Instance) || defined(MX_SPI3_TX_DMA_Instance))
#ifndef MX_SPI3_RX_DMA_Instance
  #error "SPI3 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI3_TX_DMA_Instance
  #error "SPI3 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI4
#if (defined(MX_SPI4_RX_DMA_Instance) || defined(MX_SPI4_TX_DMA_Instance))
#ifndef MX_SPI4_RX_DMA_Instance
  #error "SPI4 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI4_TX_DMA_Instance
  #error "SPI4 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI5
#if (defined(MX_SPI5_RX_DMA_Instance) || defined(MX_SPI5_TX_DMA_Instance))
#ifndef MX_SPI5_RX_DMA_Instance
  #error "SPI5 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI5_TX_DMA_Instance
  #error "SPI5 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI6
#if (defined(MX_SPI6_RX_DMA_Instance) || defined(MX_SPI6_TX_DMA_Instance))
#ifndef MX_SPI6_RX_DMA_Instance
  #error "SPI6 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI6_TX_DMA_Instance
  #error "SPI6 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif


#if ((defined(MX_SPI1) && defined(MX_SPI1_RX_DMA_Instance)) || \
     (defined(MX_SPI2) && defined(MX_SPI2_RX_DMA_Instance)) || \
     (defined(MX_SPI3) && defined(MX_SPI3_RX_DMA_Instance)) || \
     (defined(MX_SPI4) && defined(MX_SPI4_RX_DMA_Instance)) || \
     (defined(MX_SPI5) && defined(MX_SPI5_RX_DMA_Instance)) || \
     (defined(MX_SPI6) && defined(MX_SPI6_RX_DMA_Instance)))
#define __SPI_DMA_RX
#endif
#if ((defined(MX_SPI1) && defined(MX_SPI1_TX_DMA_Instance)) || \
     (defined(MX_SPI2) && defined(MX_SPI2_TX_DMA_Instance)) || \
     (defined(MX_SPI3) && defined(MX_SPI3_TX_DMA_Instance)) || \
     (defined(MX_SPI4) && defined(MX_SPI4_TX_DMA_Instance)) || \
     (defined(MX_SPI5) && defined(MX_SPI5_TX_DMA_Instance)) || \
     (defined(MX_SPI6) && defined(MX_SPI6_TX_DMA_Instance)))
#define __SPI_DMA_TX
#endif
#if (defined(__SPI_DMA_RX) && defined(__SPI_DMA_TX))
#define __SPI_DMA
#endif


// Current driver status flag definition
#define SPI_INITIALIZED           ((uint8_t)(1U))           // SPI initialized
#define SPI_POWERED               ((uint8_t)(1U << 1))     // SPI powered on
#define SPI_CONFIGURED            ((uint8_t)(1U << 2))     // SPI configured
#define SPI_DATA_LOST             ((uint8_t)(1U << 3))     // SPI data lost occurred
#define SPI_MODE_FAULT            ((uint8_t)(1U << 4))     // SPI mode fault occurred


// DMA Callback functions
typedef void (*DMA_Callback_t) (DMA_HandleTypeDef *hdma);


// SPI DMA
typedef struct _SPI_DMA {
  DMA_HandleTypeDef    *hdma;           // DMA handle
  DMA_Callback_t        cb_complete;    // DMA complete callback
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  DMA_Stream_TypeDef   *stream;         // Stream register interface
  uint32_t              channel;        // DMA channel
  uint32_t              priority;       // DMA channel priority
  IRQn_Type             irq_num;        // Stream IRQ number
#endif
} SPI_DMA;

// SPI pin
typedef const struct _SPI_PIN {
  GPIO_TypeDef         *port;           // Port
  uint32_t              pin;            // Pin
  uint32_t              af;             // Alternate function
} SPI_PIN;

// SPI Input/Output Configuration
typedef const struct _SPI_IO {
  SPI_PIN              *mosi;           // Pointer to MOSI pin configuration
  SPI_PIN              *miso;           // Pointer to MISO pin configuration
  SPI_PIN              *sck;            // Pointer to SCK pin configuration
  SPI_PIN              *nss;            // Pointer to NSS pin configuration
} SPI_IO;

typedef struct _SPI_STATUS {
  uint8_t busy;                         // Transmitter/Receiver busy flag
  uint8_t data_lost;                    // Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
  uint8_t mode_fault;                   // Mode fault detected; optional (cleared on start of transfer operation)
} SPI_STATUS;

// SPI Information (Run-time)
typedef struct _SPI_INFO {
  ARM_SPI_SignalEvent_t cb_event;       // Event Callback
  SPI_STATUS            status;         // Status flags
  uint8_t               state;          // Current SPI state
  uint32_t              mode;           // Current SPI mode
} SPI_INFO;

// SPI Transfer Information (Run-Time)
typedef struct _SPI_TRANSFER_INFO {
  uint32_t              num;            // Total number of transfers
  uint8_t              *rx_buf;         // Pointer to in data buffer
  uint8_t              *tx_buf;         // Pointer to out data buffer
  uint32_t              rx_cnt;         // Number of data received
  uint32_t              tx_cnt;         // Number of data sent
  uint32_t              dump_val;       // Variable for dumping DMA data
  uint16_t              def_val;        // Default transfer value
} SPI_TRANSFER_INFO;


// SPI Resources definition
typedef const struct {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  SPI_HandleTypeDef    *h;                    // SPI Handle
#endif
  SPI_TypeDef          *reg;                  // SPI peripheral pointer
  uint32_t            (*periph_clock)(void);  // Peripheral bus clock
  SPI_IO                io;                   // SPI Input/Output pins
  IRQn_Type             irq_num;              // SPI IRQ Number
  SPI_DMA              *rx_dma;               // Receive stream register interface
  SPI_DMA              *tx_dma;               // Transmit stream register interface
  SPI_INFO             *info;                 // Run-Time Information
  SPI_TRANSFER_INFO    *xfer;                 // SPI transfer information
} SPI_RESOURCES;

#endif /* __SPI_STM32F4XX_H */
