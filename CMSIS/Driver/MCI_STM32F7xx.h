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
 * $Date:        24. August 2015
 * $Revision:    V1.1
 *
 * Project:      MCI Driver Definitions for ST STM32F7xx
 * -------------------------------------------------------------------------- */

#ifndef __MCI_STM32F7XX_H
#define __MCI_STM32F7XX_H

#include "Driver_MCI.h"
#include "stm32f7xx_hal.h"
#if   defined(USE_STM32756G_EVAL)
#include "stm32756g_eval_io.h"
#endif

#include "RTE_Components.h"
#if   defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
#include "RTE_Device.h"
#elif defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
#include "MX_Device.h"
#else
#error "::Device:STM32Cube Framework: not selected in RTE"
#endif

#include <string.h>

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  #if (defined(RTE_Drivers_MCI0) && (RTE_SDMMC1 == 0))
    #error "SDMMC not configured in RTE_Device.h!"
  #endif

  #if ((RTE_SDMMC1_RX_DMA == 0) || (RTE_SDMMC1_TX_DMA == 0))
    #error "SDMMC1 requires Rx and Tx DMA! Enable Rx and Tx DMA in RTE_Device.h!"
  #endif

  #if (RTE_SDMMC1_RX_DMA_STREAM == RTE_SDMMC1_TX_DMA_STREAM)
    #error "SDMMC1 requires different Rx and Tx DMA Steams! Please check Rx and Tx DMA Stream configuration in RTE_Device.h!"
  #endif

/* Macro definitions */
#define _GPIO_PIN_x(x)                   GPIO_PIN_##x
#define  GPIO_PIN_x(x)                  _GPIO_PIN_x(x)

#define  DMA_PRIORITY(x)                ((x == 0) ? DMA_PRIORITY_LOW       : \
                                        ((x == 1) ? DMA_PRIORITY_MEDIUM    : \
                                        ((x == 2) ? DMA_PRIORITY_HIGH      : \
                                        ((x == 3) ? DMA_PRIORITY_VERY_HIGH : 0xFFFFFFFFU))))

#define _DMA_CHANNEL_x(x)                DMA_CHANNEL_##x
#define  DMA_CHANNEL_x(x)               _DMA_CHANNEL_x(x)

#define _DMAx_STREAMy(x, y)              DMA##x##_Stream##y
#define  DMAx_STREAMy(x, y)             _DMAx_STREAMy(x, y)

#define _DMAx_STREAMy_IRQ(x, y)          DMA##x##_Stream##y##_IRQHandler
#define  DMAx_STREAMy_IRQ(x, y)         _DMAx_STREAMy_IRQ(x, y)

#define _DMAx_STREAMy_IRQn(x, y)         DMA##x##_Stream##y##_IRQn
#define  DMAx_STREAMy_IRQn(x, y)        _DMAx_STREAMy_IRQn(x, y)

/* DMA SDIO_TX */
#define MX_SDMMC1_TX_DMA_Instance       DMAx_STREAMy(RTE_SDMMC1_TX_DMA_NUMBER, RTE_SDMMC1_TX_DMA_STREAM)
#define MX_SDMMC1_TX_DMA_Channel        DMA_CHANNEL_x(RTE_SDMMC1_TX_DMA_CHANNEL)
#define MX_SDMMC1_TX_DMA_Priority       DMA_PRIORITY(RTE_SDMMC1_TX_DMA_PRIORITY)
#define SDMMC1_TX_DMA_Handler           DMAx_STREAMy_IRQ(RTE_SDMMC1_TX_DMA_NUMBER, RTE_SDMMC1_TX_DMA_STREAM)
#define SDMMC1_TX_DMA_IRQn              DMAx_STREAMy_IRQn(RTE_SDMMC1_TX_DMA_NUMBER, RTE_SDMMC1_TX_DMA_STREAM)

/* DMA SDIO_RX */
#define MX_SDMMC1_RX_DMA_Instance       DMAx_STREAMy(RTE_SDMMC1_RX_DMA_NUMBER, RTE_SDMMC1_RX_DMA_STREAM)
#define MX_SDMMC1_RX_DMA_Channel        DMA_CHANNEL_x(RTE_SDMMC1_RX_DMA_CHANNEL)
#define MX_SDMMC1_RX_DMA_Priority       DMA_PRIORITY(RTE_SDMMC1_RX_DMA_PRIORITY)
#define SDMMC1_RX_DMA_Handler           DMAx_STREAMy_IRQ(RTE_SDMMC1_RX_DMA_NUMBER, RTE_SDMMC1_RX_DMA_STREAM)
#define SDMMC1_RX_DMA_IRQn              DMAx_STREAMy_IRQn(RTE_SDMMC1_RX_DMA_NUMBER, RTE_SDMMC1_RX_DMA_STREAM)

  #if (RTE_SDMMC1_BUS_WIDTH_4)
    #define MX_SDMMC1_D0_Pin            1
    #define MX_SDMMC1_D1_Pin            1
    #define MX_SDMMC1_D2_Pin            1
    #define MX_SDMMC1_D3_Pin            1
  #endif
  #if (RTE_SDMMC1_BUS_WIDTH_8)
    #define MX_SDMMC1_D4_Pin            1
    #define MX_SDMMC1_D5_Pin            1
    #define MX_SDMMC1_D6_Pin            1
    #define MX_SDMMC1_D7_Pin            1
  #endif

  #if (RTE_SDMMC1_CD_PIN_EN)
    #define MX_MemoryCard_CD_Pin        1
    #define MX_MemoryCard_CD_GPIOx      RTE_SDMMC1_CD_PORT
    #define MX_MemoryCard_CD_GPIO_Pin   GPIO_PIN_x(RTE_SDMMC1_CD_PIN)
    #define MX_MemoryCard_CD_GPIO_PuPd  RTE_SDMMC1_CD_PULL
    #define MemoryCard_CD_Pin_Active    ((RTE_SDMMC1_CD_ACTIVE == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET)
  #endif

  #if (RTE_SDMMC1_WP_EN)
    #define MX_MemoryCard_WP_Pin        1
    #define MX_MemoryCard_WP_GPIOx      RTE_SDMMC1_WP_PORT
    #define MX_MemoryCard_WP_GPIO_Pin   GPIO_PIN_x(RTE_SDMMC1_WP_PIN)
    #define MX_MemoryCard_WP_GPIO_PuPd  RTE_SDMMC1_WP_PULL
    #define MemoryCard_WP_Pin_Active    ((RTE_SDMMC1_WP_ACTIVE == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET)
  #endif

#else /* MX_Device.h */

#define EXPAND_SYMBOL(pin, ext)         MX_##pin##_##ext
#define MX_SYM(pin, ext)                EXPAND_SYMBOL(pin, ext)

  #if defined(MX_MemoryCard_CD)
    #define MX_MemoryCard_CD_Pin        1
    #define MX_MemoryCard_CD_GPIOx      MX_SYM(MX_MemoryCard_CD, GPIOx)
    #define MX_MemoryCard_CD_GPIO_Pin   MX_SYM(MX_MemoryCard_CD, GPIO_Pin)
    #define MX_MemoryCard_CD_GPIO_PuPd  MX_SYM(MX_MemoryCard_CD, GPIO_PuPd)
    #define MX_MemoryCard_CD_GPIO_Mode  MX_SYM(MX_MemoryCard_CD, GPIO_Mode)
  #endif

  #if defined(MX_MemoryCard_WP)
    #define MX_MemoryCard_WP_Pin        1
    #define MX_MemoryCard_WP_GPIOx      MX_SYM(MX_MemoryCard_WP, GPIOx)
    #define MX_MemoryCard_WP_GPIO_Pin   MX_SYM(MX_MemoryCard_WP, GPIO_Pin)
    #define MX_MemoryCard_WP_GPIO_PuPd  MX_SYM(MX_MemoryCard_WP, GPIO_PuPd)
    #define MX_MemoryCard_WP_GPIO_Mode  MX_SYM(MX_MemoryCard_WP, GPIO_Mode)
  #endif

#endif /* RTE_DEVICE_FRAMEWORK_CLASSIC */

/* Define 4-bit data bus width */
#if defined(MX_SDMMC1_D0_Pin) && defined(MX_SDMMC1_D1_Pin) && defined(MX_SDMMC1_D2_Pin) && defined(MX_SDMMC1_D3_Pin)
  #define MCI_BUS_WIDTH_4   1U
#else
  #define MCI_BUS_WIDTH_4   0U
#endif

/* Define 8-bit data bus width */
#if defined(MX_SDMMC1_D0_Pin) && defined(MX_SDMMC1_D1_Pin) && defined(MX_SDMMC1_D2_Pin) && defined(MX_SDMMC1_D3_Pin) && \
    defined(MX_SDMMC1_D4_Pin) && defined(MX_SDMMC1_D5_Pin) && defined(MX_SDMMC1_D6_Pin) && defined(MX_SDMMC1_D7_Pin)
  #define MCI_BUS_WIDTH_8   1U
#else
  #define MCI_BUS_WIDTH_8   0U
#endif

/* Define Card Detect pin existence */
#if defined(MX_MemoryCard_CD_Pin)
  #define MCI_CD_PIN        1U
#else
  #define MCI_CD_PIN        0U
#endif

/* Define Write Protect pin existence */
#if defined(MX_MemoryCard_WP_Pin)
  #define MCI_WP_PIN        1U
#else
  #define MCI_WP_PIN        0U
#endif

/* SDIO Adapter Clock definition */
#define SDMMCCLK            48000000U    /* SDMMC adapter clock */

/* Interrupt clear mask */
#define SDMMC_ICR_BIT_Msk     (SDMMC_ICR_CCRCFAILC | \
                               SDMMC_ICR_DCRCFAILC | \
                               SDMMC_ICR_CTIMEOUTC | \
                               SDMMC_ICR_DTIMEOUTC | \
                               SDMMC_ICR_TXUNDERRC | \
                               SDMMC_ICR_RXOVERRC  | \
                               SDMMC_ICR_CMDRENDC  | \
                               SDMMC_ICR_CMDSENTC  | \
                               SDMMC_ICR_DATAENDC  | \
                               SDMMC_ICR_DBCKENDC  | \
                               SDMMC_ICR_SDIOITC)

/* Error interrupt mask */
#define SDMMC_STA_ERR_BIT_Msk (SDMMC_STA_CCRCFAIL | \
                               SDMMC_STA_DCRCFAIL | \
                               SDMMC_STA_CTIMEOUT | \
                               SDMMC_STA_DTIMEOUT)

/* Driver flag definitions */
#define MCI_INIT      ((uint8_t)0x01)   /* MCI initialized           */
#define MCI_POWER     ((uint8_t)0x02)   /* MCI powered on            */
#define MCI_SETUP     ((uint8_t)0x04)   /* MCI configured            */
#define MCI_RESP_LONG ((uint8_t)0x08)   /* Long response expected    */
#define MCI_RESP_CRC  ((uint8_t)0x10)   /* Check response CRC error  */
#define MCI_DATA_XFER ((uint8_t)0x20)   /* Transfer data             */
#define MCI_DATA_READ ((uint8_t)0x40)   /* Read transfer             */
#define MCI_READ_WAIT ((uint8_t)0x80)   /* Read wait operation start */

#define MCI_RESPONSE_EXPECTED_Msk (ARM_MCI_RESPONSE_SHORT      | \
                                   ARM_MCI_RESPONSE_SHORT_BUSY | \
                                   ARM_MCI_RESPONSE_LONG)

/* MCI Transfer Information Definition */
typedef struct _MCI_XFER {
  uint8_t *buf;                         /* Data buffer                        */
  uint32_t cnt;                         /* Data bytes to transfer             */
} MCI_XFER;

/* MCI Driver State Definition */
typedef struct _MCI_INFO {
  ARM_MCI_SignalEvent_t cb_event;       /* Driver event callback function     */
  ARM_MCI_STATUS        status;         /* Driver status                      */
  uint32_t             *response;       /* Pointer to response buffer         */
  MCI_XFER              xfer;           /* Data transfer description          */
  uint8_t volatile      flags;          /* Driver state flags                 */
  uint32_t              dctrl;          /* Data control register value        */
  uint32_t              dlen;           /* Data length register value         */
} MCI_INFO;

#endif /* __MCI_STM32F7XX_H */
