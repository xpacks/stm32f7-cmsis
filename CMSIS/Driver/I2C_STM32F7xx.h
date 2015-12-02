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
 * $Date:        15. July 2015
 * $Revision:    V1.0
 *
 * Project:      I2C Driver definitions for ST STM32F7xx
 * -------------------------------------------------------------------------- */

#ifndef __I2C_STM32F7XX_H
#define __I2C_STM32F7XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_I2C.h"
#include "stm32f7xx_hal.h"

#include "RTE_Components.h"
#if   defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
#include "RTE_Device.h"
#elif defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
#include "MX_Device.h"
#else
#error "::Device:STM32Cube Framework: not selected in RTE"
#endif

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  #if ((defined(RTE_Drivers_I2C1) || \
        defined(RTE_Drivers_I2C2) || \
        defined(RTE_Drivers_I2C3) || \
        defined(RTE_Drivers_I2C4))   \
       && (RTE_I2C1 == 0)            \
       && (RTE_I2C2 == 0)            \
       && (RTE_I2C3 == 0)            \
       && (RTE_I2C4 == 0))
    #error "I2C not configured in RTE_Device.h!"
  #endif

/* RTE macros */
#define _DMA_CHANNEL_x(x)               DMA_CHANNEL_##x
#define  DMA_CHANNEL_x(x)              _DMA_CHANNEL_x(x)

#define  DMA_PRIORITY(x)               ((x == 0) ? DMA_PRIORITY_LOW    : \
                                       ((x == 1) ? DMA_PRIORITY_MEDIUM : \
                                       ((x == 2) ? DMA_PRIORITY_HIGH   : \
                                                   DMA_PRIORITY_VERY_HIGH)))

#define _DMAx_STREAMy(x, y)             DMA##x##_Stream##y
#define  DMAx_STREAMy(x, y)            _DMAx_STREAMy(x, y)

#define _DMAx_STREAMy_IRQ(x, y)         DMA##x##_Stream##y##_IRQHandler
#define  DMAx_STREAMy_IRQ(x, y)        _DMAx_STREAMy_IRQ(x, y)

#define _DMAx_STREAMy_IRQn(x, y)        DMA##x##_Stream##y##_IRQn
#define  DMAx_STREAMy_IRQn(x, y)       _DMAx_STREAMy_IRQn(x, y)

/* I2C1 configuration definitions */
#if defined (RTE_I2C1) && (RTE_I2C1 == 1)

  #define MX_I2C1

  #if (RTE_I2C1_RX_DMA == 1)
    #define MX_I2C1_RX_DMA_Instance DMAx_STREAMy(RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_STREAM)
    #define MX_I2C1_RX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_STREAM)
    #define MX_I2C1_RX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C1_RX_DMA_CHANNEL)
    #define MX_I2C1_RX_DMA_Priority DMA_PRIORITY(RTE_I2C1_RX_DMA_PRIORITY)

    #define I2C1_RX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_STREAM)
  #endif
  #if (RTE_I2C1_TX_DMA == 1)
    #define MX_I2C1_TX_DMA_Instance DMAx_STREAMy(RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_STREAM)
    #define MX_I2C1_TX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_STREAM)
    #define MX_I2C1_TX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C1_TX_DMA_CHANNEL)
    #define MX_I2C1_TX_DMA_Priority DMA_PRIORITY(RTE_I2C1_TX_DMA_PRIORITY)

    #define I2C1_TX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_STREAM)
  #endif

  #define MX_I2C1_SCL_GPIOx         RTE_I2C1_SCL_PORT
  #define MX_I2C1_SCL_GPIO_Pin      (1U << RTE_I2C1_SCL_BIT)
  #define MX_I2C1_SCL_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C1_SCL_GPIO_AF       GPIO_AF4_I2C1

  #define MX_I2C1_SDA_GPIOx         RTE_I2C1_SDA_PORT
  #define MX_I2C1_SDA_GPIO_Pin      (1U << RTE_I2C1_SDA_BIT)
  #define MX_I2C1_SDA_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C1_SDA_GPIO_AF       GPIO_AF4_I2C1
#endif

/* I2C2 configuration definitions */
#if defined (RTE_I2C2) && (RTE_I2C2 == 1)

  #define MX_I2C2

  #if (RTE_I2C2_RX_DMA == 1)
    #define MX_I2C2_RX_DMA_Instance DMAx_STREAMy(RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_STREAM)
    #define MX_I2C2_RX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_STREAM)
    #define MX_I2C2_RX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C2_RX_DMA_CHANNEL)
    #define MX_I2C2_RX_DMA_Priority DMA_PRIORITY(RTE_I2C2_RX_DMA_PRIORITY)

    #define I2C2_RX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_STREAM)
  #endif
  #if (RTE_I2C2_TX_DMA == 1)
    #define MX_I2C2_TX_DMA_Instance DMAx_STREAMy(RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_STREAM)
    #define MX_I2C2_TX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_STREAM)
    #define MX_I2C2_TX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C2_TX_DMA_CHANNEL)
    #define MX_I2C2_TX_DMA_Priority DMA_PRIORITY(RTE_I2C2_TX_DMA_PRIORITY)

    #define I2C2_TX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_STREAM)
  #endif

  #define MX_I2C2_SCL_GPIOx         RTE_I2C2_SCL_PORT
  #define MX_I2C2_SCL_GPIO_Pin      (1U << RTE_I2C2_SCL_BIT)
  #define MX_I2C2_SCL_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C2_SCL_GPIO_AF       GPIO_AF4_I2C2

  #define MX_I2C2_SDA_GPIOx         RTE_I2C2_SDA_PORT
  #define MX_I2C2_SDA_GPIO_Pin      (1U << RTE_I2C2_SDA_BIT)
  #define MX_I2C2_SDA_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C2_SDA_GPIO_AF       GPIO_AF4_I2C2
#endif

/* I2C3 configuration definitions */
#if defined (RTE_I2C3) && (RTE_I2C3 == 1)

  #define MX_I2C3

  #if (RTE_I2C3_RX_DMA == 1)
    #define MX_I2C3_RX_DMA_Instance DMAx_STREAMy(RTE_I2C3_RX_DMA_NUMBER, RTE_I2C3_RX_DMA_STREAM)
    #define MX_I2C3_RX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C3_RX_DMA_NUMBER, RTE_I2C3_RX_DMA_STREAM)
    #define MX_I2C3_RX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C3_RX_DMA_CHANNEL)
    #define MX_I2C3_RX_DMA_Priority DMA_PRIORITY(RTE_I2C3_RX_DMA_PRIORITY)

    #define I2C3_RX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C3_RX_DMA_NUMBER, RTE_I2C3_RX_DMA_STREAM)
  #endif
  #if (RTE_I2C3_TX_DMA == 1)
    #define MX_I2C3_TX_DMA_Instance DMAx_STREAMy(RTE_I2C3_TX_DMA_NUMBER, RTE_I2C3_TX_DMA_STREAM)
    #define MX_I2C3_TX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C3_TX_DMA_NUMBER, RTE_I2C3_TX_DMA_STREAM)
    #define MX_I2C3_TX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C3_TX_DMA_CHANNEL)
    #define MX_I2C3_TX_DMA_Priority DMA_PRIORITY(RTE_I2C3_TX_DMA_PRIORITY)

    #define I2C3_TX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C3_TX_DMA_NUMBER, RTE_I2C3_TX_DMA_STREAM)
  #endif

  #define MX_I2C3_SCL_GPIOx         RTE_I2C3_SCL_PORT
  #define MX_I2C3_SCL_GPIO_Pin      (1U << RTE_I2C3_SCL_BIT)
  #define MX_I2C3_SCL_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C3_SCL_GPIO_AF       GPIO_AF4_I2C3

  #define MX_I2C3_SDA_GPIOx         RTE_I2C3_SDA_PORT
  #define MX_I2C3_SDA_GPIO_Pin      (1U << RTE_I2C3_SDA_BIT)
  #define MX_I2C3_SDA_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C3_SDA_GPIO_AF       GPIO_AF4_I2C3
#endif

/* I2C4 configuration definitions */
#if defined (RTE_I2C4) && (RTE_I2C4 == 1)

  #define MX_I2C4

  #if (RTE_I2C4_RX_DMA == 1)
    #define MX_I2C4_RX_DMA_Instance DMAx_STREAMy(RTE_I2C4_RX_DMA_NUMBER, RTE_I2C4_RX_DMA_STREAM)
    #define MX_I2C4_RX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C4_RX_DMA_NUMBER, RTE_I2C4_RX_DMA_STREAM)
    #define MX_I2C4_RX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C4_RX_DMA_CHANNEL)
    #define MX_I2C4_RX_DMA_Priority DMA_PRIORITY(RTE_I2C4_RX_DMA_PRIORITY)

    #define I2C4_RX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C4_RX_DMA_NUMBER, RTE_I2C4_RX_DMA_STREAM)
  #endif
  #if (RTE_I2C4_TX_DMA == 1)
    #define MX_I2C4_TX_DMA_Instance DMAx_STREAMy(RTE_I2C4_TX_DMA_NUMBER, RTE_I2C4_TX_DMA_STREAM)
    #define MX_I2C4_TX_DMA_IRQn     DMAx_STREAMy_IRQn(RTE_I2C4_TX_DMA_NUMBER, RTE_I2C4_TX_DMA_STREAM)
    #define MX_I2C4_TX_DMA_Channel  DMA_CHANNEL_x(RTE_I2C4_TX_DMA_CHANNEL)
    #define MX_I2C4_TX_DMA_Priority DMA_PRIORITY(RTE_I2C4_TX_DMA_PRIORITY)

    #define I2C4_TX_DMA_Handler     DMAx_STREAMy_IRQ(RTE_I2C4_TX_DMA_NUMBER, RTE_I2C4_TX_DMA_STREAM)
  #endif

  #define MX_I2C4_SCL_GPIOx         RTE_I2C4_SCL_PORT
  #define MX_I2C4_SCL_GPIO_Pin      (1U << RTE_I2C4_SCL_BIT)
  #define MX_I2C4_SCL_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C4_SCL_GPIO_AF       GPIO_AF4_I2C4

  #define MX_I2C4_SDA_GPIOx         RTE_I2C4_SDA_PORT
  #define MX_I2C4_SDA_GPIO_Pin      (1U << RTE_I2C4_SDA_BIT)
  #define MX_I2C4_SDA_GPIO_PuPdOD   GPIO_NOPULL
  #define MX_I2C4_SDA_GPIO_AF       GPIO_AF4_I2C4
#endif

#endif /* RTE_DEVICE_FRAMEWORK_CLASSIC */

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  #if ((defined(RTE_Drivers_I2C1) || \
        defined(RTE_Drivers_I2C2) || \
        defined(RTE_Drivers_I2C3) || \
        defined(RTE_Drivers_I2C4))   \
        && (!defined (MX_I2C1))      \
        && (!defined (MX_I2C2))      \
        && (!defined (MX_I2C3))      \
        && (!defined (MX_I2C4)))
    #error "I2C not configured in STM32CubeMX!"
  #endif

#endif /* RTE_DEVICE_FRAMEWORK_CUBE_MX */

/* Bus Clear clock period definition */
#define I2C_BUS_CLEAR_CLOCK_PERIOD   2        // I2C bus clock period in ms

/* Current driver status flag definition */
#define I2C_INIT            ((uint8_t)0x01)   // I2C initialized
#define I2C_POWER           ((uint8_t)0x02)   // I2C powered on
#define I2C_SETUP           ((uint8_t)0x04)   // I2C configured

/* Transfer status flags definitions */
#define XFER_CTRL_RESTART   ((uint8_t)0x01)   // Transfer pending
#define XFER_CTRL_STOP      ((uint8_t)0x02)   // Generate repeated start and readdress
#define XFER_CTRL_ADDR_NACK ((uint8_t)0x04)   // Slave address not acknowledged
#define XFER_CTRL_ADDR_DONE ((uint8_t)0x08)   // Addressing done

/* DMA Event definitions */
#define DMA_COMPLETED             0U
#define DMA_ERROR                 1U

/* DMA Callback functions */
typedef void (*DMA_Callback_t) (DMA_HandleTypeDef *hdma);

/* DMA Information definitions */
typedef struct _I2C_DMA {
  DMA_HandleTypeDef    *h;
  DMA_Callback_t        cb_complete;
  DMA_Callback_t        cb_error;
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  DMA_Stream_TypeDef   *stream;               // Stream register interface
  IRQn_Type             irq_num;              // Stream IRQ number
  uint32_t              channel;              // Channel number
  uint32_t              priority;             // Stream priority
#endif
} const I2C_DMA;


/* I2C Input/Output Configuration */
typedef const struct _I2C_IO {
  GPIO_TypeDef         *scl_port;             // SCL IO Port
  GPIO_TypeDef         *sda_port;             // SDA IO Port
  uint16_t              scl_pin;              // SCL IO Pin
  uint16_t              sda_pin;              // SDA IO Pin
  uint32_t              scl_pull;             // SCL IO Pin Pull Definition
  uint32_t              sda_pull;             // SDA IO Pin Pull Definition
  uint32_t              scl_af;
  uint32_t              sda_af;
} I2C_IO;


/* I2C Transfer Information (Run-Time) */
typedef struct _I2C_TRANSFER_INFO {
  uint32_t              num;                  // Number of data to transfer
  uint32_t              cnt;                  // Data transfer counter
  uint8_t              *data;                 // Data pointer
  uint8_t               ctrl;                 // Transfer control flags
} I2C_TRANSFER_INFO;


/* I2C Information (Run-Time) */
typedef struct _I2C_INFO {
  ARM_I2C_SignalEvent_t cb_event;             // Event Callback
  ARM_I2C_STATUS        status;               // Status flags
  I2C_TRANSFER_INFO     xfer;                 // Transfer information
  uint8_t               flags;                // Current I2C state flags
} I2C_INFO;


/* I2C Resources definition */
typedef struct {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  I2C_HandleTypeDef    *h;                    // I2C handle
#endif
  I2C_TypeDef          *reg;                  // I2C peripheral register interface
  I2C_DMA              *dma_rx;               // I2C DMA Configuration
  I2C_DMA              *dma_tx;               // I2C DMA Configuration
  I2C_IO                io;                   // I2C Input/Output pins
  IRQn_Type             ev_irq_num;           // I2C Event IRQ Number
  IRQn_Type             er_irq_num;           // I2C Error IRQ Number
  I2C_INFO             *info;                 // Run-Time information
} const I2C_RESOURCES;

#endif /* __I2C_STM32F7XX_H */
