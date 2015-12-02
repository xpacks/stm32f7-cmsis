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
 * $Date:        15. October 2015
 * $Revision:    V1.1
 *
 * Project:      SAI Driver definitions for ST STM32F7xx
 * -------------------------------------------------------------------------- */

#ifndef __SAI_STM32F7XX_H
#define __SAI_STM32F7XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_SAI.h"
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
  #if ((defined(RTE_Drivers_SAI1) || \
        defined(RTE_Drivers_SAI2))   \
       && (RTE_SAI1 == 0)            \
       && (RTE_SAI2 == 0))
    #error "SAI not configured in RTE_Device.h!"
  #endif

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

// SAI1 configuration definitions
#if (RTE_SAI1 == 1)
  #define MX_SAI1

  #if (RTE_SAI1_A_DMA == 1)
    #define MX_SAI1_A_DMA_Instance    DMAx_STREAMy(RTE_SAI1_A_DMA_NUMBER, RTE_SAI1_A_DMA_STREAM)
    #define MX_SAI1_A_DMA_IRQn        DMAx_STREAMy_IRQn(RTE_SAI1_A_DMA_NUMBER, RTE_SAI1_A_DMA_STREAM)
    #define MX_SAI1_A_DMA_Channel     DMA_CHANNEL_x(RTE_SAI1_A_DMA_CHANNEL)
    #define MX_SAI1_A_DMA_Priority    DMA_PRIORITY(RTE_SAI1_A_DMA_PRIORITY)

    #define SAI1_A_DMA_Handler        DMAx_STREAMy_IRQ(RTE_SAI1_A_DMA_NUMBER, RTE_SAI1_A_DMA_STREAM)
  #endif

  #if (RTE_SAI1_B_DMA == 1)
    #if ((RTE_SAI1_B_DMA_STREAM == 5) && (RTE_SAI1_B_DMA_CHANNEL == 1))
      #error "SAI1_B DMA Configuration error: Stream 5 requires Channel 0."
    #endif
    #if ((RTE_SAI1_B_DMA_STREAM == 7) && (RTE_SAI1_B_DMA_CHANNEL == 1))
      #error "SAI1_B DMA Configuration error: Stream 7 requires Channel 0."
    #endif
    #if ((RTE_SAI1_B_DMA_STREAM == 4) && (RTE_SAI1_B_DMA_CHANNEL == 0))
      #error "SAI1_B DMA Configuration error: Stream 4 requires Channel 1."
    #endif
    #define MX_SAI1_B_DMA_Instance    DMAx_STREAMy(RTE_SAI1_B_DMA_NUMBER, RTE_SAI1_B_DMA_STREAM)
    #define MX_SAI1_B_DMA_IRQn        DMAx_STREAMy_IRQn(RTE_SAI1_B_DMA_NUMBER, RTE_SAI1_B_DMA_STREAM)
    #define MX_SAI1_B_DMA_Channel     DMA_CHANNEL_x(RTE_SAI1_B_DMA_CHANNEL)
    #define MX_SAI1_B_DMA_Priority    DMA_PRIORITY(RTE_SAI1_B_DMA_PRIORITY)

    #define SAI1_B_DMA_Handler        DMAx_STREAMy_IRQ(RTE_SAI1_B_DMA_NUMBER, RTE_SAI1_B_DMA_STREAM)
  #endif

  #if (RTE_SAI1_SD_A_PIN == 1)
    #define MX_SAI1_SD_A_Pin            1U
    #define MX_SAI1_SD_A_GPIOx          RTE_SAI1_SD_A_PORT
    #define MX_SAI1_SD_A_GPIO_Pin      (1U << RTE_SAI1_SD_A_BIT)
    #define MX_SAI1_SD_A_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI1_SD_A_GPIO_AF        GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_FS_A_PIN == 1)
    #define MX_SAI1_FS_A_Pin            1U
    #define MX_SAI1_FS_A_GPIOx          RTE_SAI1_FS_A_PORT
    #define MX_SAI1_FS_A_GPIO_Pin      (1U << RTE_SAI1_FS_A_BIT)
    #define MX_SAI1_FS_A_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI1_FS_A_GPIO_AF        GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_SCK_A_PIN == 1)
    #define MX_SAI1_SCK_A_Pin           1U
    #define MX_SAI1_SCK_A_GPIOx         RTE_SAI1_SCK_A_PORT
    #define MX_SAI1_SCK_A_GPIO_Pin     (1U << RTE_SAI1_SCK_A_BIT)
    #define MX_SAI1_SCK_A_GPIO_PuPd     GPIO_NOPULL
    #define MX_SAI1_SCK_A_GPIO_AF       GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_MCLK_A_PIN == 1)
    #define MX_SAI1_MCLK_A_Pin          1U
    #define MX_SAI1_MCLK_A_GPIOx        RTE_SAI1_MCLK_A_PORT
    #define MX_SAI1_MCLK_A_GPIO_Pin    (1U << RTE_SAI1_MCLK_A_BIT)
    #define MX_SAI1_MCLK_A_GPIO_PuPd    GPIO_NOPULL
    #define MX_SAI1_MCLK_A_GPIO_AF      GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_SD_B_PIN == 1)
    #define MX_SAI1_SD_B_Pin            1U
    #define MX_SAI1_SD_B_GPIOx          RTE_SAI1_SD_B_PORT
    #define MX_SAI1_SD_B_GPIO_Pin      (1U << RTE_SAI1_SD_B_BIT)
    #define MX_SAI1_SD_B_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI1_SD_B_GPIO_AF        GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_FS_B_PIN == 1)
    #define MX_SAI1_FS_B_Pin            1U
    #define MX_SAI1_FS_B_GPIOx          RTE_SAI1_FS_B_PORT
    #define MX_SAI1_FS_B_GPIO_Pin      (1U << RTE_SAI1_FS_B_BIT)
    #define MX_SAI1_FS_B_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI1_FS_B_GPIO_AF        GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_SCK_B_PIN == 1)
    #define MX_SAI1_SCK_B_Pin           1U
    #define MX_SAI1_SCK_B_GPIOx         RTE_SAI1_SCK_B_PORT
    #define MX_SAI1_SCK_B_GPIO_Pin     (1U << RTE_SAI1_SCK_B_BIT)
    #define MX_SAI1_SCK_B_GPIO_PuPd     GPIO_NOPULL
    #define MX_SAI1_SCK_B_GPIO_AF       GPIO_AF6_SAI1
  #endif

  #if (RTE_SAI1_MCLK_B_PIN == 1)
    #define MX_SAI1_MCLK_B_Pin          1U
    #define MX_SAI1_MCLK_B_GPIOx        RTE_SAI1_MCLK_B_PORT
    #define MX_SAI1_MCLK_B_GPIO_Pin    (1U << RTE_SAI1_MCLK_B_BIT)
    #define MX_SAI1_MCLK_B_GPIO_PuPd    GPIO_NOPULL
    #define MX_SAI1_MCLK_B_GPIO_AF      GPIO_AF6_SAI1
  #endif

#endif

// SAI2 configuration definitions
#if (RTE_SAI2 == 1)
  #define MX_SAI2

  #if (RTE_SAI2_A_DMA == 1)
    #define MX_SAI2_A_DMA_Instance    DMAx_STREAMy(RTE_SAI2_A_DMA_NUMBER, RTE_SAI2_A_DMA_STREAM)
    #define MX_SAI2_A_DMA_IRQn        DMAx_STREAMy_IRQn(RTE_SAI2_A_DMA_NUMBER, RTE_SAI2_A_DMA_STREAM)
    #define MX_SAI2_A_DMA_Channel     DMA_CHANNEL_x(RTE_SAI2_A_DMA_CHANNEL)
    #define MX_SAI2_A_DMA_Priority    DMA_PRIORITY(RTE_SAI2_A_DMA_PRIORITY)

    #define SAI2_A_DMA_Handler        DMAx_STREAMy_IRQ(RTE_SAI2_A_DMA_NUMBER, RTE_SAI2_A_DMA_STREAM)
  #endif

  #if (RTE_SAI2_B_DMA == 1)
    #if ((RTE_SAI2_B_DMA_STREAM == 7) && (RTE_SAI2_B_DMA_CHANNEL == 3))
      #error "SAI2_B DMA Configuration error: Stream 7 requires Channel 0."
    #endif
    #if ((RTE_SAI2_B_DMA_STREAM == 6) && (RTE_SAI2_B_DMA_CHANNEL == 0))
      #error "SAI2_B DMA Configuration error: Stream 6 requires Channel 3."
    #endif
    #define MX_SAI2_B_DMA_Instance    DMAx_STREAMy(RTE_SAI2_B_DMA_NUMBER, RTE_SAI2_B_DMA_STREAM)
    #define MX_SAI2_B_DMA_IRQn        DMAx_STREAMy_IRQn(RTE_SAI2_B_DMA_NUMBER, RTE_SAI2_B_DMA_STREAM)
    #define MX_SAI2_B_DMA_Channel     DMA_CHANNEL_x(RTE_SAI2_B_DMA_CHANNEL)
    #define MX_SAI2_B_DMA_Priority    DMA_PRIORITY(RTE_SAI2_B_DMA_PRIORITY)

    #define SAI2_B_DMA_Handler        DMAx_STREAMy_IRQ(RTE_SAI2_B_DMA_NUMBER, RTE_SAI2_B_DMA_STREAM)
  #endif

  #if (RTE_SAI2_SD_A_PIN == 1)
    #define MX_SAI2_SD_A_Pin            1U
    #define MX_SAI2_SD_A_GPIOx          RTE_SAI2_SD_A_PORT
    #define MX_SAI2_SD_A_GPIO_Pin      (1U << RTE_SAI2_SD_A_BIT)
    #define MX_SAI2_SD_A_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI2_SD_A_GPIO_AF        GPIO_AF10_SAI2
  #endif

  #if (RTE_SAI2_FS_A_PIN == 1)
    #define MX_SAI2_FS_A_Pin            1U
    #define MX_SAI2_FS_A_GPIOx          RTE_SAI2_FS_A_PORT
    #define MX_SAI2_FS_A_GPIO_Pin      (1U << RTE_SAI2_FS_A_BIT)
    #define MX_SAI2_FS_A_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI2_FS_A_GPIO_AF        GPIO_AF10_SAI2
  #endif

  #if (RTE_SAI2_SCK_A_PIN == 1)
    #define MX_SAI2_SCK_A_Pin           1U
    #define MX_SAI2_SCK_A_GPIOx         RTE_SAI2_SCK_A_PORT
    #define MX_SAI2_SCK_A_GPIO_Pin     (1U << RTE_SAI2_SCK_A_BIT)
    #define MX_SAI2_SCK_A_GPIO_PuPd     GPIO_NOPULL
    #define MX_SAI2_SCK_A_GPIO_AF       GPIO_AF10_SAI2
  #endif

  #if (RTE_SAI2_MCLK_A_PIN == 1)
    #define MX_SAI2_MCLK_A_Pin          1U
    #define MX_SAI2_MCLK_A_GPIOx        RTE_SAI2_MCLK_A_PORT
    #define MX_SAI2_MCLK_A_GPIO_Pin    (1U << RTE_SAI2_MCLK_A_BIT)
    #define MX_SAI2_MCLK_A_GPIO_PuPd    GPIO_NOPULL
    #define MX_SAI2_MCLK_A_GPIO_AF      GPIO_AF10_SAI2
  #endif

  #if (RTE_SAI2_SD_B_PIN == 1)
    #define MX_SAI2_SD_B_Pin            1U
    #define MX_SAI2_SD_B_GPIOx          RTE_SAI2_SD_B_PORT
    #define MX_SAI2_SD_B_GPIO_Pin      (1U << RTE_SAI2_SD_B_BIT)
    #define MX_SAI2_SD_B_GPIO_PuPd      GPIO_NOPULL
    #define MX_SAI2_SD_B_GPIO_AF        GPIO_AF10_SAI2
  #endif

  #if (RTE_SAI2_FS_B_PIN == 1)
    #define MX_SAI2_FS_B_Pin            1U
    #define MX_SAI2_FS_B_GPIOx          RTE_SAI2_FS_B_PORT
    #define MX_SAI2_FS_B_GPIO_Pin      (1U << RTE_SAI2_FS_B_BIT)
    #define MX_SAI2_FS_B_GPIO_PuPd      GPIO_NOPULL
    #if ((RTE_SAI2_FS_B_PORT_ID == 1) || (RTE_SAI2_FS_B_PORT_ID == 2))
      #define MX_SAI2_FS_B_GPIO_AF      GPIO_AF8_SAI2
    #else
      #define MX_SAI2_FS_B_GPIO_AF      GPIO_AF10_SAI2
    #endif
  #endif

  #if (RTE_SAI2_SCK_B_PIN == 1)
    #define MX_SAI2_SCK_B_Pin           1U
    #define MX_SAI2_SCK_B_GPIOx         RTE_SAI2_SCK_B_PORT
    #define MX_SAI2_SCK_B_GPIO_Pin     (1U << RTE_SAI2_SCK_B_BIT)
    #define MX_SAI2_SCK_B_GPIO_PuPd     GPIO_NOPULL
    #if (RTE_SAI2_SCK_B_PORT_ID == 1)
      #define MX_SAI2_SCK_B_GPIO_AF      GPIO_AF8_SAI2
    #else
      #define MX_SAI2_SCK_B_GPIO_AF      GPIO_AF10_SAI2
    #endif
  #endif

  #if (RTE_SAI2_MCLK_B_PIN == 1)
    #define MX_SAI2_MCLK_B_Pin          1U
    #define MX_SAI2_MCLK_B_GPIOx        RTE_SAI2_MCLK_B_PORT
    #define MX_SAI2_MCLK_B_GPIO_Pin    (1U << RTE_SAI2_MCLK_B_BIT)
    #define MX_SAI2_MCLK_B_GPIO_PuPd    GPIO_NOPULL
    #define MX_SAI2_MCLK_B_GPIO_AF      GPIO_AF10_SAI2
  #endif 
#endif

#endif /* RTE_DEVICE_FRAMEWORK_CLASSIC */

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  #if ((defined(RTE_Drivers_SAI1) || \
        defined(RTE_Drivers_SAI2))   \
        && (!defined (MX_SAI1))      \
        && (!defined (MX_SAI2)))
    #error "SAI not configured in STM32CubeMX!"
  #endif

#endif /* RTE_DEVICE_FRAMEWORK_CUBE_MX */

#if ((defined(MX_SAI1) && defined(MX_SAI1_A_DMA_Instance)) || \
     (defined(MX_SAI1) && defined(MX_SAI1_B_DMA_Instance)) || \
     (defined(MX_SAI2) && defined(MX_SAI2_A_DMA_Instance)) || \
     (defined(MX_SAI2) && defined(MX_SAI2_B_DMA_Instance)))
#define __SAI_DMA                       (1U)
#endif

// SAI Blocks
#define SAI_BLOCK_A                     (0U)
#define SAI_BLOCK_B                     (1U)

// SAI Synchronization outputs
#define SAI_SYNCOUT_NO_SYNC             (0U)
#define SAI_SYNCOUT_BLOCK_A             (1U)
#define SAI_SYNCOUT_BLOCK_B             (2U)

// SAI Synchronization with
#define SAI_SYNC_SRC_INTERNAL_SUB_BLOCK (1U)
#define SAI_SYNC_SRC_EXTERNAL_SAI       (2U)

// SAI flags
#define SAI_FLAG_INITIALIZED            (     1U)
#define SAI_FLAG_POWERED                (1U << 1)
#define SAI_FLAG_CONFIGURED             (1U << 2)

// DMA Callback functions
typedef void (*DMA_Callback_t) (DMA_HandleTypeDef *hdma);

// SAI Stream Information (Run-Time)
typedef struct _SAI_STREAM_INFO {
  
  uint32_t                num;          // Total number of data to be transmited/received
  uint32_t                cnt;          // Number of data transmited/receive
  uint8_t                *buf;          // Pointer to data buffer
  uint8_t                 data_bits;    // Number of data bits
  uint32_t                protocol;     // SAI Protocol
} SAI_STREAM_INFO;

typedef struct _SAI_STATUS {
  uint8_t tx_busy;                      // Transmitter busy flag
  uint8_t rx_busy;                      // Receiver busy flag
  uint8_t tx_underflow;                 // Transmit data underflow detected (cleared on start of next send operation)
  uint8_t rx_overflow;                  // Receive data overflow detected (cleared on start of next receive operation)
  uint8_t frame_error;                  // Sync Frame error detected (cleared on start of next send/receive operation)
} SAI_STATUS;

// SAI DMA
typedef const struct _SAI_DMA {
  DMA_HandleTypeDef    *hdma;           // DMA handle
  DMA_Callback_t        cb_complete;    // DMA complete callback
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  DMA_Stream_TypeDef   *stream;         // Stream register interface
  uint32_t              channel;        // DMA channel
  uint32_t              priority;       // DMA channel priority
  IRQn_Type             irq_num;        // Stream IRQ number
#endif
} SAI_DMA;

// SAI pin
typedef const struct _SAI_PIN {
  GPIO_TypeDef         *port;           // Port
  uint32_t              pin;            // Pin
  uint32_t              af;             // Alternate function
} SAI_PIN;

// SAI Pin Configuration
typedef const struct _SAI_IO {
  SAI_PIN              *sd;             // Pointer to SD pin configuration
  SAI_PIN              *fs;             // Pointer to FS pin configuration
  SAI_PIN              *sck;            // Pointer to SCK pin configuration
  SAI_PIN              *mclk;           // Pointer to MCLK pin configuration
} SAI_IO;


// SAI Information (Run-time)
typedef struct _SAI_INFO {
  ARM_SAI_SignalEvent_t cb_event;       // Event Callback
  SAI_STATUS            status;         // Status flags
  uint8_t               flags;          // SAI driver flags
} SAI_INFO;

// SAI Stream
typedef const struct _SAI_STREAM {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  SAI_HandleTypeDef    *h;              // SAI HAL Handle
#endif
  SAI_Block_TypeDef    *reg;            // SAI block peripheral pointer
  SAI_IO                io;             // SAI pins
  SAI_DMA              *dma;            // DMA
  uint8_t               sync_source;    // SAI_SYNC_SRC_INTERNAL_SUB_BLOCK or SAI_SYNC_SRC_EXTERNAL_SAI
  SAI_STREAM_INFO      *info;           // Pointer to Stream Run-Time information
} SAI_STREAM;

// SAI Resources definition
typedef const struct {
  ARM_SAI_CAPABILITIES  capabilities;        // Capabilities
  SAI_TypeDef          *reg;                 // SAI global peripheral pointer
  SAI_STREAM           *rx;                  // SAI RX Stream
  SAI_STREAM           *tx;                  // SAI TX Stream
  uint32_t              periph_clk;          // RCC Periph Clock Selection
  IRQn_Type             irq_num;             // SAI IRQ Number
  uint8_t               sync_out;            // Synchronization outputs
  SAI_INFO             *info;                // Run-Time Information
} SAI_RESOURCES;

#endif /* __SAI_STM32F7XX_H */
