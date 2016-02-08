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
 * $Date:        14. December 2015
 * $Revision:    V1.2
 *
 * Driver:       Driver_SAI1, Driver_SAI2
 * Configured:   via RTE_Device.h configuration file
 * Project:      SAI Driver for ST STM32F7xx
 * -----------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting               Value     SAI Interface
 *   ---------------------               -----     -------------
 *   Connect to hardware via Driver_SAI# = 1       use SAI1
 *   Connect to hardware via Driver_SAI# = 2       use SAI2
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.2
 *      - Corrected receive DMA configuration
 *      - Corrected synchronization configuration
 *      - Corrected FRC register configuration
 *  Version 1.1
 *    Corrected PowerControl function for:
 *      - Unconditional Power Off
 *      - Conditional Power full (driver must be initialized)
 *  Version 1.0
 *    - Initial release
 */

/*
 * NOTE:
 *  The SAI input clock must be properly configured in the user application
 */

#include "SAI_STM32F7xx.h"

#define ARM_SAI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,2)
// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = { ARM_SAI_API_VERSION, ARM_SAI_DRV_VERSION };


#ifdef MX_SAI1
#ifndef SAI1_RX_BLOCK
#define SAI1_RX_BLOCK SAI_BLOCK_A
#endif
#ifndef SAI1_TX_BLOCK
#define SAI1_TX_BLOCK SAI_BLOCK_B
#endif

#if ((SAI1_RX_BLOCK == SAI_BLOCK_A) && (SAI1_TX_BLOCK == SAI_BLOCK_A))
  #error "SAI1 BlockA can not be used for receive and also transmit."
#endif
#if ((SAI1_RX_BLOCK == SAI_BLOCK_B) && (SAI1_TX_BLOCK == SAI_BLOCK_B))
  #error "SAI1 BlockB can not be used for receive and also transmit."
#endif
#endif

#ifndef SAI1_SYNCOUT
#define SAI1_SYNCOUT   SAI_SYNCOUT_NO_SYNC
#endif

#ifndef SAI1_BLOCK_A_SYNC_SRC
#define SAI1_BLOCK_A_SYNC_SRC SAI_SYNC_SRC_INTERNAL_SUB_BLOCK
#endif

#ifndef SAI1_BLOCK_B_SYNC_SRC
#define SAI1_BLOCK_B_SYNC_SRC SAI_SYNC_SRC_INTERNAL_SUB_BLOCK
#endif

#ifdef MX_SAI2
#ifndef SAI2_RX_BLOCK
#define SAI2_RX_BLOCK SAI_BLOCK_A
#endif
#ifndef SAI2_TX_BLOCK
#define SAI2_TX_BLOCK SAI_BLOCK_B
#endif

#if ((SAI2_RX_BLOCK == SAI_BLOCK_A) && (SAI2_TX_BLOCK == SAI_BLOCK_A))
  #error "SAI2 BlockA can not be used for receive and also transmit."
#endif
#if ((SAI2_RX_BLOCK == SAI_BLOCK_B) && (SAI2_TX_BLOCK == SAI_BLOCK_B))
  #error "SAI2 BlockB can not be used for receive and also transmit."
#endif
#endif

#ifndef SAI2_SYNCOUT
#define SAI2_SYNCOUT   SAI_SYNCOUT_NO_SYNC
#endif

#ifndef SAI2_BLOCK_A_SYNC_SRC
#define SAI2_BLOCK_A_SYNC_SRC SAI_SYNC_SRC_INTERNAL_SUB_BLOCK
#endif

#ifndef SAI2_BLOCK_B_SYNC_SRC
#define SAI2_BLOCK_B_SYNC_SRC SAI_SYNC_SRC_INTERNAL_SUB_BLOCK
#endif

// SAI1
#ifdef MX_SAI1

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
#endif

// SAI1 Run-Time Information
static SAI_INFO        SAI1_Info = { 0U };
static SAI_STREAM_INFO SAI1_Stream_Info_A = { 0U };
static SAI_STREAM_INFO SAI1_Stream_Info_B = { 0U };

#ifdef MX_SAI1_SD_A_Pin
  static const SAI_PIN SAI1_sd_a   = {MX_SAI1_SD_A_GPIOx,   MX_SAI1_SD_A_GPIO_Pin,   MX_SAI1_SD_A_GPIO_AF};
#endif
#ifdef MX_SAI1_FS_A_Pin
  static const SAI_PIN SAI1_fs_a   = {MX_SAI1_FS_A_GPIOx,   MX_SAI1_FS_A_GPIO_Pin,   MX_SAI1_FS_A_GPIO_AF};
#endif
#ifdef MX_SAI1_SCK_A_Pin
  static const SAI_PIN SAI1_sck_a  = {MX_SAI1_SCK_A_GPIOx,  MX_SAI1_SCK_A_GPIO_Pin,  MX_SAI1_SCK_A_GPIO_AF};
#endif
#ifdef MX_SAI1_MCLK_A_Pin
  static const SAI_PIN SAI1_mclk_a = {MX_SAI1_MCLK_A_GPIOx, MX_SAI1_MCLK_A_GPIO_Pin, MX_SAI1_MCLK_A_GPIO_AF};
#endif
#ifdef MX_SAI1_SD_B_Pin
  static const SAI_PIN SAI1_sd_b   = {MX_SAI1_SD_B_GPIOx,   MX_SAI1_SD_B_GPIO_Pin,   MX_SAI1_SD_B_GPIO_AF};
#endif
#ifdef MX_SAI1_FS_B_Pin
  static const SAI_PIN SAI1_fs_b   = {MX_SAI1_FS_B_GPIOx,   MX_SAI1_FS_B_GPIO_Pin,   MX_SAI1_FS_B_GPIO_AF};
#endif
#ifdef MX_SAI1_SCK_B_Pin
  static const SAI_PIN SAI1_sck_b  = {MX_SAI1_SCK_B_GPIOx,  MX_SAI1_SCK_B_GPIO_Pin,  MX_SAI1_SCK_B_GPIO_AF};
#endif
#ifdef MX_SAI1_MCLK_B_Pin
  static const SAI_PIN SAI1_mclk_b = {MX_SAI1_MCLK_B_GPIOx, MX_SAI1_MCLK_B_GPIO_Pin, MX_SAI1_MCLK_B_GPIO_AF};
#endif

// SAI1 A DMA
#ifdef MX_SAI1_A_DMA_Instance
  void SAI1_A_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static DMA_HandleTypeDef hdma_sai1_a = { 0U };
#else
  extern DMA_HandleTypeDef hdma_sai1_a;
#endif
  static SAI_DMA SAI1_A_DMA = {
    &hdma_sai1_a,
    SAI1_A_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SAI1_A_DMA_Instance,
    MX_SAI1_A_DMA_Channel,
    MX_SAI1_A_DMA_Priority,
    MX_SAI1_A_DMA_IRQn
#endif
  };
#endif

// SAI1 B DMA
#ifdef MX_SAI1_B_DMA_Instance
  void SAI1_B_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static DMA_HandleTypeDef hdma_sai1_b = { 0U };
#else
  extern DMA_HandleTypeDef hdma_sai1_b;
#endif
  static SAI_DMA SAI1_B_DMA = {
    &hdma_sai1_b,
    SAI1_B_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SAI1_B_DMA_Instance,
    MX_SAI1_B_DMA_Channel,
    MX_SAI1_B_DMA_Priority,
    MX_SAI1_B_DMA_IRQn
#endif
  };
#endif

// SAI1 A Stream structure
static const SAI_STREAM sai1_stream_a = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hsai_BlockA1,                        // SAI HAL Handle
#endif
  SAI1_Block_A,                         // SAI block peripheral pointer
  {                                     // Pins
#ifdef MX_SAI1_SD_A_Pin
    &SAI1_sd_a,
#else
    NULL,
#endif
#ifdef MX_SAI1_FS_A_Pin
    &SAI1_fs_a,
#else
    NULL,
#endif
#ifdef MX_SAI1_SCK_A_Pin
    &SAI1_sck_a,
#else
    NULL,
#endif
#ifdef MX_SAI1_MCLK_A_Pin
    &SAI1_mclk_a,
#else
    NULL,
#endif
  },
#ifdef MX_SAI1_A_DMA_Instance
  &SAI1_A_DMA,
#else
  NULL,
#endif
  SAI1_BLOCK_A_SYNC_SRC,
  &SAI1_Stream_Info_A
};

// SAI1 B Stream structure
static const SAI_STREAM sai1_stream_b = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hsai_BlockB1,                        // SAI HAL Handle
#endif
  SAI1_Block_B,                         // SAI block peripheral pointer
  {                                     // Pins
#ifdef MX_SAI1_SD_B_Pin
    &SAI1_sd_b,
#else
    NULL,
#endif
#ifdef MX_SAI1_FS_B_Pin
    &SAI1_fs_b,
#else
    NULL,
#endif
#ifdef MX_SAI1_SCK_B_Pin
    &SAI1_sck_b,
#else
    NULL,
#endif
#ifdef MX_SAI1_MCLK_B_Pin
    &SAI1_mclk_b,
#else
    NULL,
#endif
  },
#ifdef MX_SAI1_B_DMA_Instance
  &SAI1_B_DMA,
#else
  NULL,
#endif
  SAI1_BLOCK_B_SYNC_SRC,
  &SAI1_Stream_Info_B
};

// SAI1 Resources
static const SAI_RESOURCES SAI1_Resources = {
  {  // Capabilities
    1,   ///< supports asynchronous Transmit/Receive
    1,   ///< supports synchronous Transmit/Receive
    1,   ///< supports user defined Protocol
    1,   ///< supports I2S Protocol
    1,   ///< supports MSB/LSB justified Protocol
    1,   ///< supports PCM short/long frame Protocol
    1,   ///< supports AC'97 Protocol
    1,   ///< supports Mono mode
    1,   ///< supports Companding
#if ((MX_SAI1_MCLK_A_Pin == 1U) || (RTE_SAI1_MCLK_B_PIN == 1U))
    1,   ///< supports MCLK (Master Clock) pin
#else
    0,   ///< supports MCLK (Master Clock) pin
#endif
    0,   ///< supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR
  },

  SAI1,
#if (SAI1_RX_BLOCK == SAI_BLOCK_A)
  &sai1_stream_a,
#else
  &sai1_stream_b,
#endif
#if (SAI1_TX_BLOCK == SAI_BLOCK_A)
  &sai1_stream_a,
#else
  &sai1_stream_b,
#endif

  RCC_PERIPHCLK_SAI1,
  SAI1_IRQn,
  SAI1_SYNCOUT,
  &SAI1_Info
};
#endif /* MX_SAI1 */

// SAI2
#ifdef MX_SAI2

#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
SAI_HandleTypeDef hsai_BlockA2;
SAI_HandleTypeDef hsai_BlockB2;
#endif

// SAI2 Run-Time Information
static SAI_INFO        SAI2_Info = { 0U };
static SAI_STREAM_INFO SAI2_Stream_Info_A = { 0U };
static SAI_STREAM_INFO SAI2_Stream_Info_B = { 0U };

#ifdef MX_SAI2_SD_A_Pin
  static const SAI_PIN SAI2_sd_a   = {MX_SAI2_SD_A_GPIOx,   MX_SAI2_SD_A_GPIO_Pin,   MX_SAI2_SD_A_GPIO_AF};
#endif
#ifdef MX_SAI2_FS_A_Pin
  static const SAI_PIN SAI2_fs_a   = {MX_SAI2_FS_A_GPIOx,   MX_SAI2_FS_A_GPIO_Pin,   MX_SAI2_FS_A_GPIO_AF};
#endif
#ifdef MX_SAI2_SCK_A_Pin
  static const SAI_PIN SAI2_sck_a  = {MX_SAI2_SCK_A_GPIOx,  MX_SAI2_SCK_A_GPIO_Pin,  MX_SAI2_SCK_A_GPIO_AF};
#endif
#ifdef MX_SAI2_MCLK_A_Pin
  static const SAI_PIN SAI2_mclk_a = {MX_SAI2_MCLK_A_GPIOx, MX_SAI2_MCLK_A_GPIO_Pin, MX_SAI2_MCLK_A_GPIO_AF};
#endif
#ifdef MX_SAI2_SD_B_Pin
  static const SAI_PIN SAI2_sd_b   = {MX_SAI2_SD_B_GPIOx,   MX_SAI2_SD_B_GPIO_Pin,   MX_SAI2_SD_B_GPIO_AF};
#endif
#ifdef MX_SAI2_FS_B_Pin
  static const SAI_PIN SAI2_fs_b   = {MX_SAI2_FS_B_GPIOx,   MX_SAI2_FS_B_GPIO_Pin,   MX_SAI2_FS_B_GPIO_AF};
#endif
#ifdef MX_SAI2_SCK_B_Pin
  static const SAI_PIN SAI2_sck_b  = {MX_SAI2_SCK_B_GPIOx,  MX_SAI2_SCK_B_GPIO_Pin,  MX_SAI2_SCK_B_GPIO_AF};
#endif
#ifdef MX_SAI2_MCLK_B_Pin
  static const SAI_PIN SAI2_mclk_b = {MX_SAI2_MCLK_B_GPIOx, MX_SAI2_MCLK_B_GPIO_Pin, MX_SAI2_MCLK_B_GPIO_AF};
#endif

// SAI2 A DMA
#ifdef MX_SAI2_A_DMA_Instance
  void SAI2_A_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static DMA_HandleTypeDef hdma_sai2_a = { 0U };
#else
  extern DMA_HandleTypeDef hdma_sai2_a;
#endif
  static SAI_DMA SAI2_A_DMA = {
    &hdma_sai2_a,
    SAI2_A_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SAI2_A_DMA_Instance,
    MX_SAI2_A_DMA_Channel,
    MX_SAI2_A_DMA_Priority,
    MX_SAI2_A_DMA_IRQn
#endif
  };
#endif

// SAI2 B DMA
#ifdef MX_SAI2_B_DMA_Instance
  void SAI2_B_DMA_Complete (DMA_HandleTypeDef *hdma);

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  static DMA_HandleTypeDef hdma_sai2_b = { 0U };
#else
  extern DMA_HandleTypeDef hdma_sai2_b;
#endif
  static SAI_DMA SAI2_B_DMA = {
    &hdma_sai2_b,
    SAI2_B_DMA_Complete,
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
    MX_SAI2_B_DMA_Instance,
    MX_SAI2_B_DMA_Channel,
    MX_SAI2_B_DMA_Priority,
    MX_SAI2_B_DMA_IRQn
#endif
  };
#endif

// SAI2 A Stream structure
static const SAI_STREAM sai2_stream_a = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hsai_BlockA2,                        // SAI HAL Handle
#endif
  SAI2_Block_A,                         // SAI block peripheral pointer
  {                                     // Pins
#ifdef MX_SAI2_SD_A_Pin
    &SAI2_sd_a,
#else
    NULL,
#endif
#ifdef MX_SAI2_FS_A_Pin
    &SAI2_fs_a,
#else
    NULL,
#endif
#ifdef MX_SAI2_SCK_A_Pin
    &SAI2_sck_a,
#else
    NULL,
#endif
#ifdef MX_SAI2_MCLK_A_Pin
    &SAI2_mclk_a,
#else
    NULL,
#endif
  },
#ifdef MX_SAI2_A_DMA_Instance
  &SAI2_A_DMA,
#else
  NULL,
#endif
  SAI2_BLOCK_A_SYNC_SRC,
  &SAI2_Stream_Info_A
};

// SAI2 B Stream structure
static const SAI_STREAM sai2_stream_b = {
#ifdef RTE_DEVICE_FRAMEWORK_CUBE_MX
  &hsai_BlockB2,                        // SAI HAL Handle
#endif
  SAI2_Block_B,                         // SAI block peripheral pointer
  {                                     // Pins
#ifdef MX_SAI2_SD_B_Pin
    &SAI2_sd_b,
#else
    NULL,
#endif
#ifdef MX_SAI2_FS_B_Pin
    &SAI2_fs_b,
#else
    NULL,
#endif
#ifdef MX_SAI2_SCK_B_Pin
    &SAI2_sck_b,
#else
    NULL,
#endif
#ifdef MX_SAI2_MCLK_B_Pin
    &SAI2_mclk_b,
#else
    NULL,
#endif
  },
#ifdef MX_SAI2_B_DMA_Instance
  &SAI2_B_DMA,
#else
  NULL,
#endif
  SAI2_BLOCK_B_SYNC_SRC,
  &SAI2_Stream_Info_B
};

// SAI2 Resources
static const SAI_RESOURCES SAI2_Resources = {
  {  // Capabilities
    1,   ///< supports asynchronous Transmit/Receive
    1,   ///< supports synchronous Transmit/Receive
    1,   ///< supports user defined Protocol
    1,   ///< supports I2S Protocol
    1,   ///< supports MSB/LSB justified Protocol
    1,   ///< supports PCM short/long frame Protocol
    1,   ///< supports AC'97 Protocol
    1,   ///< supports Mono mode
    1,   ///< supports Companding
#if ((MX_SAI2_MCLK_A_Pin == 1U) || (RTE_SAI2_MCLK_B_PIN == 1U))
    1,   ///< supports MCLK (Master Clock) pin
#else
    0,   ///< supports MCLK (Master Clock) pin
#endif
    0,   ///< supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR
  },

  SAI2,
#if (SAI2_RX_BLOCK == SAI_BLOCK_A)
  &sai2_stream_a,
#else
  &sai2_stream_b,
#endif
#if (SAI2_TX_BLOCK == SAI_BLOCK_A)
  &sai2_stream_a,
#else
  &sai2_stream_b,
#endif

  RCC_PERIPHCLK_SAI2,
  SAI2_IRQn,
  SAI2_SYNCOUT,
  &SAI2_Info
};
#endif /* MX_SAI2 */


#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
/**
  \fn          void Enable_GPIO_Clock (GPIO_TypeDef *port)
  \brief       Enable GPIO clock
*/
static void Enable_GPIO_Clock (GPIO_TypeDef *GPIOx) {
#ifdef GPIOA
  if (GPIOx == GPIOA) __GPIOA_CLK_ENABLE();
#endif 
#ifdef GPIOB
  if (GPIOx == GPIOB) __GPIOB_CLK_ENABLE();
#endif
#ifdef GPIOC
  if (GPIOx == GPIOC) __GPIOC_CLK_ENABLE();
#endif
#ifdef GPIOD
  if (GPIOx == GPIOD) __GPIOD_CLK_ENABLE();
#endif
#ifdef GPIOE
  if (GPIOx == GPIOE) __GPIOE_CLK_ENABLE();
#endif
#ifdef GPIOF
  if (GPIOx == GPIOF) __GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
  if (GPIOx == GPIOG) __GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
  if (GPIOx == GPIOH) __GPIOH_CLK_ENABLE();
#endif
#ifdef GPIOI
  if (GPIOx == GPIOI) __GPIOI_CLK_ENABLE();
#endif
#ifdef GPIOJ
  if (GPIOx == GPIOJ) __GPIOJ_CLK_ENABLE();
#endif
#ifdef GPIOK
  if (GPIOx == GPIOK) __GPIOK_CLK_ENABLE();
#endif
}
#endif


/**
  \fn          ARM_DRIVER_VERSION SAI_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION SAI_GetVersion (void) {
  return (DriverVersion);
}

/**
  \fn          ARM_SAI_CAPABILITIES SAI_GetCapabilities (SAI_RESOURCES *sai)
  \brief       Get driver capabilities.
  \param[in]   sai Pointer to SAI resources
  \return      \ref ARM_SAI_CAPABILITIES
*/
static ARM_SAI_CAPABILITIES SAI_GetCapabilities (SAI_RESOURCES *sai) {
  return (sai->capabilities);
}

/**
  \fn          int32_t SAI_Initialize (ARM_SAI_SignalEvent_t  cb_event,
                                       SAI_RESOURCES         *sai)
  \brief       Initialize SAI Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \param[in]   sai       Pointer to SAI resources
  \return      \ref execution_status
*/
static int32_t SAI_Initialize (ARM_SAI_SignalEvent_t  cb_event,
                               SAI_RESOURCES         *sai) {
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  GPIO_InitTypeDef GPIO_InitStruct;
#endif

  if (sai->info->flags & SAI_FLAG_INITIALIZED) { return ARM_DRIVER_OK; }

  // Initialize SAI Run-Time Resources
  sai->info->cb_event = cb_event;
  sai->info->status.tx_busy      = 0U;
  sai->info->status.rx_busy      = 0U;
  sai->info->status.tx_underflow = 0U;
  sai->info->status.rx_overflow  = 0U;
  sai->info->status.frame_error  = 0U;

  // Clear stream information
  memset(sai->rx->info , 0, sizeof(SAI_STREAM_INFO));
  memset(sai->tx->info , 0, sizeof(SAI_STREAM_INFO));

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  // Configure RX SD pin
  if (sai->rx->io.sd != NULL) {
    Enable_GPIO_Clock (sai->rx->io.sd->port);
    GPIO_InitStruct.Pin       = sai->rx->io.sd->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->rx->io.sd->af;
    HAL_GPIO_Init(sai->rx->io.sd->port, &GPIO_InitStruct);
  }

  // Configure RX FS pin
  if (sai->rx->io.fs != NULL) {
    Enable_GPIO_Clock (sai->rx->io.fs->port);
    GPIO_InitStruct.Pin       = sai->rx->io.fs->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->rx->io.fs->af;
    HAL_GPIO_Init(sai->rx->io.fs->port, &GPIO_InitStruct);
  }

  // Configure RX SCK pin
  if (sai->rx->io.sck != NULL) {
    Enable_GPIO_Clock (sai->rx->io.sck->port);
    GPIO_InitStruct.Pin       = sai->rx->io.sck->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->rx->io.sck->af;
    HAL_GPIO_Init(sai->rx->io.sck->port, &GPIO_InitStruct);
  }

    // Configure RX MCLK pin
  if (sai->rx->io.mclk != NULL) {
    Enable_GPIO_Clock (sai->rx->io.mclk->port);
    GPIO_InitStruct.Pin       = sai->rx->io.mclk->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->rx->io.mclk->af;
    HAL_GPIO_Init(sai->rx->io.mclk->port, &GPIO_InitStruct);
  }

  // Configure TX SD pin
  if (sai->tx->io.sd != NULL) {
    Enable_GPIO_Clock (sai->tx->io.sd->port);
    GPIO_InitStruct.Pin       = sai->tx->io.sd->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->tx->io.sd->af;
    HAL_GPIO_Init(sai->tx->io.sd->port, &GPIO_InitStruct);
  }

  // Configure TX FS pin
  if (sai->tx->io.fs != NULL) {
    Enable_GPIO_Clock (sai->tx->io.fs->port);
    GPIO_InitStruct.Pin       = sai->tx->io.fs->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->tx->io.fs->af;
    HAL_GPIO_Init(sai->tx->io.fs->port, &GPIO_InitStruct);
  }

  // Configure TX SCK pin
  if (sai->tx->io.sck != NULL) {
    Enable_GPIO_Clock (sai->tx->io.sck->port);
    GPIO_InitStruct.Pin       = sai->tx->io.sck->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->tx->io.sck->af;
    HAL_GPIO_Init(sai->tx->io.sck->port, &GPIO_InitStruct);
  }

    // Configure TX MCLK pin
  if (sai->tx->io.mclk != NULL) {
    Enable_GPIO_Clock (sai->tx->io.mclk->port);
    GPIO_InitStruct.Pin       = sai->tx->io.mclk->pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = sai->tx->io.mclk->af;
    HAL_GPIO_Init(sai->tx->io.mclk->port, &GPIO_InitStruct);
  }

#ifdef __SAI_DMA
  if ((sai->rx->dma != NULL) || (sai->tx->dma != NULL)) {

    if (sai->rx->dma != NULL) {
      // Initialize SAI RX DMA Resources
      sai->rx->dma->hdma->Instance             = sai->rx->dma->stream;
      sai->rx->dma->hdma->Init.Channel         = sai->rx->dma->channel;
      sai->rx->dma->hdma->Init.Direction       = DMA_PERIPH_TO_MEMORY;
      sai->rx->dma->hdma->Init.PeriphInc       = DMA_PINC_DISABLE;
      sai->rx->dma->hdma->Init.MemInc          = DMA_MINC_ENABLE;
      sai->rx->dma->hdma->Init.Mode            = DMA_NORMAL;
      sai->rx->dma->hdma->Init.Priority        = sai->rx->dma->priority;
      sai->rx->dma->hdma->Init.FIFOMode        = DMA_FIFOMODE_DISABLE;
      sai->rx->dma->hdma->Init.FIFOThreshold   = DMA_FIFO_THRESHOLD_FULL;
      sai->rx->dma->hdma->Init.MemBurst        = DMA_MBURST_SINGLE;
      sai->rx->dma->hdma->Init.PeriphBurst     = DMA_PBURST_SINGLE;
      sai->rx->dma->hdma->Parent               = NULL;
      sai->rx->dma->hdma->XferCpltCallback     = sai->rx->dma->cb_complete;
      sai->rx->dma->hdma->XferHalfCpltCallback = NULL;
      sai->rx->dma->hdma->XferM1CpltCallback   = NULL;
      sai->rx->dma->hdma->XferErrorCallback    = NULL;
    }

    if (sai->tx->dma != NULL) {
      // Initialize SAI TX DMA Resources
      sai->tx->dma->hdma->Instance             = sai->tx->dma->stream;
      sai->tx->dma->hdma->Init.Channel         = sai->tx->dma->channel;
      sai->tx->dma->hdma->Init.Direction       = DMA_MEMORY_TO_PERIPH;
      sai->tx->dma->hdma->Init.PeriphInc       = DMA_PINC_DISABLE;
      sai->tx->dma->hdma->Init.MemInc          = DMA_MINC_ENABLE;
      sai->tx->dma->hdma->Init.Mode            = DMA_NORMAL;
      sai->tx->dma->hdma->Init.Priority        = sai->tx->dma->priority;
      sai->tx->dma->hdma->Init.FIFOMode        = DMA_FIFOMODE_DISABLE;
      sai->tx->dma->hdma->Init.FIFOThreshold   = DMA_FIFO_THRESHOLD_FULL;
      sai->tx->dma->hdma->Init.MemBurst        = DMA_MBURST_SINGLE;
      sai->tx->dma->hdma->Init.PeriphBurst     = DMA_PBURST_SINGLE;
      sai->tx->dma->hdma->Parent               = NULL;
      sai->tx->dma->hdma->XferCpltCallback     = sai->tx->dma->cb_complete;
      sai->tx->dma->hdma->XferHalfCpltCallback = NULL;
      sai->tx->dma->hdma->XferM1CpltCallback   = NULL;
      sai->tx->dma->hdma->XferErrorCallback    = NULL;
    }

    __DMA2_CLK_ENABLE();
  }
#endif
#else
  sai->rx->h->Instance = sai->rx->reg;
  sai->tx->h->Instance = sai->tx->reg;

  if (sai->rx->dma != NULL) {
    sai->rx->dma->hdma->XferCpltCallback = sai->rx->dma->cb_complete;
  }
  if (sai->tx->dma != NULL) {
    sai->tx->dma->hdma->XferCpltCallback = sai->tx->dma->cb_complete;
  }
#endif

  sai->info->flags = SAI_FLAG_INITIALIZED;

  return ARM_DRIVER_OK;

}

/**
  \fn          int32_t SAI_Uninitialize (SAI_RESOURCES *sai)
  \brief       De-initialize SAI Interface.
  \param[in]   sai  Pointer to SAI resources
  \return      \ref execution_status
*/
static int32_t SAI_Uninitialize (SAI_RESOURCES *sai) {
#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
  // Unconfigure SAI pins
  if (sai->rx->io.sd   != NULL) { HAL_GPIO_DeInit(sai->rx->io.sd->port,   sai->rx->io.sd->pin);   }
  if (sai->rx->io.fs   != NULL) { HAL_GPIO_DeInit(sai->rx->io.fs->port,   sai->rx->io.fs->pin);   }
  if (sai->rx->io.sck  != NULL) { HAL_GPIO_DeInit(sai->rx->io.sck->port,  sai->rx->io.sck->pin);  }
  if (sai->rx->io.mclk != NULL) { HAL_GPIO_DeInit(sai->rx->io.mclk->port, sai->rx->io.mclk->pin); }
  if (sai->tx->io.sd   != NULL) { HAL_GPIO_DeInit(sai->tx->io.sd->port,   sai->tx->io.sd->pin);   }
  if (sai->tx->io.fs   != NULL) { HAL_GPIO_DeInit(sai->tx->io.fs->port,   sai->tx->io.fs->pin);   }
  if (sai->tx->io.sck  != NULL) { HAL_GPIO_DeInit(sai->tx->io.sck->port,  sai->tx->io.sck->pin);  }
  if (sai->tx->io.mclk != NULL) { HAL_GPIO_DeInit(sai->tx->io.mclk->port, sai->tx->io.mclk->pin); }

  if (sai->rx->dma     != NULL) { sai->rx->dma->hdma->Instance = NULL; }
  if (sai->tx->dma     != NULL) { sai->tx->dma->hdma->Instance = NULL; }
#else
  sai->rx->h->Instance  = NULL;
  sai->tx->h->Instance  = NULL;
#endif

  // Clear SAI flags
  sai->info->flags = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SAI_PowerControl (ARM_POWER_STATE  state,
                                         SAI_RESOURCES   *sai)
  \brief       Control SAI Interface Power.
  \param[in]   state  Power state
  \param[in]   sai    Pointer to SAI resources
  \return      \ref execution_status
*/
static int32_t SAI_PowerControl (ARM_POWER_STATE  state,
                                 SAI_RESOURCES   *sai) {
  switch (state) {
    case ARM_POWER_OFF:
      // SAI peripheral reset
      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_FORCE_RESET(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_FORCE_RESET(); }

      __NOP(); __NOP(); __NOP(); __NOP();

      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_RELEASE_RESET(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_RELEASE_RESET(); }

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      HAL_NVIC_DisableIRQ (sai->irq_num);
#ifdef __SAI_DMA
      if (sai->rx->dma != NULL) {
        if (sai->rx->dma->hdma->Instance != NULL) {
          // Disable DMA IRQ in NVIC
          HAL_NVIC_DisableIRQ (sai->rx->dma->irq_num);
          // Deinitialize DMA
          HAL_DMA_DeInit (sai->rx->dma->hdma);
        }
      }

      if (sai->tx->dma != NULL) {
        if (sai->tx->dma->hdma->Instance != NULL) {
          // Disable DMA IRQ in NVIC
          HAL_NVIC_DisableIRQ (sai->tx->dma->irq_num);
          // Deinitialize DMA
          HAL_DMA_DeInit (sai->tx->dma->hdma);
        }
      }
#endif
#else
      if (sai->rx->h->Instance != NULL) { HAL_SAI_MspDeInit (sai->rx->h); }
      if (sai->tx->h->Instance != NULL) { HAL_SAI_MspDeInit (sai->tx->h); }
#endif

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      // Disable SAI clock
      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_CLK_DISABLE(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_CLK_DISABLE(); }
#endif

      // Clear status flags
      sai->info->status.tx_busy      = 0U;
      sai->info->status.rx_busy      = 0U;
      sai->info->status.tx_underflow = 0U;
      sai->info->status.rx_overflow  = 0U;
      sai->info->status.frame_error  = 0U;

      // Clear powered flag
      sai->info->flags = SAI_FLAG_INITIALIZED;
      break;

    case ARM_POWER_FULL:
      // Check if SAI already powered
      if (sai->info->flags & SAI_FLAG_POWERED) { return ARM_DRIVER_OK; }

      // Clear status flags
      sai->info->status.tx_busy      = 0U;
      sai->info->status.rx_busy      = 0U;
      sai->info->status.tx_underflow = 0U;
      sai->info->status.rx_overflow  = 0U;
      sai->info->status.frame_error  = 0U;

      // Clear stream information
      memset(sai->rx->info , 0, sizeof(SAI_STREAM_INFO));
      memset(sai->tx->info , 0, sizeof(SAI_STREAM_INFO));

      // Ready for operation - set powered flag
      sai->info->flags |= SAI_FLAG_POWERED;

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
      // Enable SAI clock
      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_CLK_ENABLE(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_CLK_ENABLE(); }

      // Clear and Enable SAI IRQ
      NVIC_ClearPendingIRQ(sai->irq_num);
      NVIC_EnableIRQ(sai->irq_num);

#ifdef __SAI_DMA
      if (sai->rx->dma != NULL) {
        // Clear and Enable DMA IRQ in NVIC
        NVIC_ClearPendingIRQ(sai->rx->dma->irq_num);
        NVIC_EnableIRQ(sai->rx->dma->irq_num);
      }

      if (sai->tx->dma != NULL) {
        // Clear and Enable DMA IRQ in NVIC
        NVIC_ClearPendingIRQ(sai->tx->dma->irq_num);
        NVIC_EnableIRQ(sai->tx->dma->irq_num);
      }
#endif
#else
      HAL_SAI_MspInit (sai->rx->h);
      HAL_SAI_MspInit (sai->tx->h);
#endif

      // SAI peripheral reset
      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_FORCE_RESET(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_FORCE_RESET(); }

      __NOP(); __NOP(); __NOP(); __NOP();

      if      (sai->reg == SAI1) { __HAL_RCC_SAI1_RELEASE_RESET(); }
      else if (sai->reg == SAI2) { __HAL_RCC_SAI2_RELEASE_RESET(); }

      // Synchronization output
      sai->reg->GCR = sai->sync_out << 4;

      // Synchronization input
      if (sai->reg == SAI1) { sai->reg->GCR |= 0x01U; }
      if (sai->reg == SAI2) { sai->reg->GCR |= 0x00U; }

      // Activate all slots (default)
      sai->rx->reg->SLOTR = 0xFFFF0000U;
      sai->tx->reg->SLOTR = 0xFFFF0000U;

      break;

    case ARM_POWER_LOW:
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;

}

/**
  \fn          int32_t SAI_Send (const void *data, uint32_t num, SAI_RESOURCES *sai)
  \brief       Start sending data to SAI transmitter.
  \param[in]   data  Pointer to buffer with data to send to SAI transmitter
  \param[in]   num   Number of data items to send
  \param[in]   sai   Pointer to SAI resources
  \return      \ref execution_status
*/
static int32_t SAI_Send (const void *data, uint32_t num, SAI_RESOURCES *sai) {

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((sai->info->flags & SAI_FLAG_CONFIGURED) == 0U) {
    // SAI is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  if (sai->info->status.tx_busy) {
    // Send is not completed yet
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set Send busy flag
  sai->info->status.tx_busy = 1U;

  // Clear TX underflow flag
  sai->info->status.tx_underflow = 0U;

  // Save transmit buffer info
  sai->tx->info->buf = (uint8_t *)data;
  sai->tx->info->cnt = 0U;
  sai->tx->info->num = num;

  // DMA mode
#ifdef __SAI_DMA
  if (sai->tx->dma != NULL) {
    if (sai->tx->info->data_bits > 16U) {
      sai->tx->dma->hdma->Init.PeriphDataAlignment   = DMA_PDATAALIGN_WORD;
      sai->tx->dma->hdma->Init.MemDataAlignment      = DMA_PDATAALIGN_WORD;
    } else {
      if (sai->tx->info->data_bits > 8U) {
        sai->tx->dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        sai->tx->dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
      } else {
        sai->tx->dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        sai->tx->dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
      }
    }

    // Initialize and start SAI TX DMA Stream
    if (HAL_DMA_Init     (sai->tx->dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (sai->tx->dma->hdma, (uint32_t)sai->tx->info->buf, (uint32_t)(&sai->tx->reg->DR), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // DMA enable
    sai->tx->reg->CR1 |= SAI_xCR1_DMAEN;
  } else
#endif
  {
    if ((sai->tx->reg->CR1 & SAI_xCR1_SAIEN) != 0) {
      // FIFO request interrupt enable
      sai->tx->reg->IMR |= SAI_xIMR_FREQIE;
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SAI_Receive (void *data, uint32_t num, SAI_RESOURCES *sai)
  \brief       Start receiving data from SAI receiver.
  \param[out]  data  Pointer to buffer for data to receive from SAI receiver
  \param[in]   num   Number of data items to receive
  \param[in]   sai   Pointer to SAI resources
  \return      \ref execution_status
*/
static int32_t SAI_Receive (void *data, uint32_t num, SAI_RESOURCES *sai) {

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((sai->info->flags & SAI_FLAG_CONFIGURED) == 0U) {
    // SAI is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  if (sai->info->status.rx_busy) {
    // Receive is not completed yet
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set receive active flag
  sai->info->status.rx_busy = 1U;

  // Clear RX overflow flag
  sai->info->status.rx_overflow = 0U;

  // Save transmit buffer info
  sai->rx->info->buf = (uint8_t *)data;
  sai->rx->info->cnt = 0U;
  sai->rx->info->num = num;

  // DMA mode
#ifdef __SAI_DMA
  if (sai->rx->dma != NULL) {
    if (sai->rx->info->data_bits > 16U) {
      sai->rx->dma->hdma->Init.PeriphDataAlignment   = DMA_PDATAALIGN_WORD;
      sai->rx->dma->hdma->Init.MemDataAlignment      = DMA_PDATAALIGN_WORD;
    } else {
      if (sai->rx->info->data_bits > 8U) {
        sai->rx->dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        sai->rx->dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_HALFWORD;
      } else {
        sai->rx->dma->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        sai->rx->dma->hdma->Init.MemDataAlignment    = DMA_PDATAALIGN_BYTE;
      }
    }

    // Initialize and start SAI RX DMA Stream
    if (HAL_DMA_Init     (sai->rx->dma->hdma) != HAL_OK) { return ARM_DRIVER_ERROR; }
    if (HAL_DMA_Start_IT (sai->rx->dma->hdma, (uint32_t)(&sai->rx->reg->DR), (uint32_t)sai->rx->info->buf, num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    // DMA enable
    sai->rx->reg->CR1 |= SAI_xCR1_DMAEN;
  } else
#endif
  {
    if ((sai->rx->reg->CR1 & SAI_xCR1_SAIEN) != 0U) {
      // FIFO request interrupt enable
      sai->rx->reg->IMR |= SAI_xIMR_FREQIE;
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t SAI_GetTxCount (SAI_RESOURCES *sai)
  \brief       Get transmitted data count.
  \param[in]   sai  Pointer to SAI resources
  \return      number of data items transmitted
*/
static uint32_t SAI_GetTxCount (SAI_RESOURCES *sai) {
#ifdef __SAI_DMA
  if (sai->tx->dma != NULL) {
    return (sai->tx->info->num - __HAL_DMA_GET_COUNTER(sai->tx->dma->hdma));
  } else
#endif
  {
    return sai->tx->info->cnt;
  }
}

/**
  \fn          uint32_t ARM_SAI_GetRxCount (SAI_RESOURCES *sai)
  \brief       Get received data count.
  \param[in]   sai  Pointer to SAI resources
  \return      number of data items received
*/
static uint32_t SAI_GetRxCount (SAI_RESOURCES *sai) {
#ifdef __SAI_DMA
  if (sai->rx->dma != NULL) {
    return (sai->rx->info->num - __HAL_DMA_GET_COUNTER(sai->rx->dma->hdma));
  } else
#endif
  {
    return sai->rx->info->cnt;
  }
}

/**
  \fn          int32_t SAI_Control (uint32_t control, uint32_t arg1, uint32_t arg2, SAI_RESOURCES *sai)
  \brief       Control SAI Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \param[in]   sai      Pointer to SAI resources
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/
static int32_t SAI_Control (uint32_t control, uint32_t arg1, uint32_t arg2, SAI_RESOURCES *sai) {
  uint8_t  master, mclk_pin, stream_en;
  uint32_t val, freq;
  uint32_t cr1, cr2, slotr, frcr, imr;
  uint32_t frame_len, frame_sync_width, frame_sync_pol;
  uint32_t bit_order, clock_pol, frame_sync_early, slot_count, slot_sz, slot_offset; 
  SAI_STREAM * stream;

  if ((sai->info->flags & SAI_FLAG_POWERED) == 0U) {
    // SAI not powered
    return ARM_DRIVER_ERROR;
  }

  // Clear local register variables
  cr1 = cr2 = slotr = frcr = 0U;

  switch (control & ARM_SAI_CONTROL_Msk) {
    case ARM_SAI_CONFIGURE_TX:
      stream = sai->tx;
      break;

    case ARM_SAI_CONFIGURE_RX:
      stream = sai->rx;
      break;

    case ARM_SAI_CONTROL_TX:
      stream = sai->tx;
      // Check mute bit
      if ((arg1 & 2U) != 0U) { stream->reg->CR2 |=  SAI_xCR2_MUTE; }
      else                   { stream->reg->CR2 &= ~SAI_xCR2_MUTE; }

      if ((arg1 & 1U) == 0U) {
        // Disable FREQ interrupt
        stream->reg->IMR &= ~SAI_xIMR_FREQIE;

        // Disable transmitter
        stream->reg->CR1 &= ~SAI_xCR1_SAIEN;
        while ((stream->reg->CR1 & SAI_xCR1_SAIEN) != 0);
      } else {
        // Enable transmitter
        stream->reg->CR1 |=  SAI_xCR1_SAIEN;

#ifdef __SAI_DMA
        if (stream->dma == NULL) {
#endif
          if (sai->info->status.tx_busy != 0U) {
            // Enable FREQ interrupt
            stream->reg->IMR |= SAI_xIMR_FREQIE;
          }
#ifdef __SAI_DMA
        }
#endif

        if ((stream->reg->CR1 & SAI_xCR1_MODE) == 0U) {
          // Master transmitter mode: send dummy value to start generating clocks
          stream->reg->DR = 0U;
        }

        // Enable transmit underflow interrupt
        stream->reg->IMR |= SAI_xIMR_OVRUDRIE;
      }

      return ARM_DRIVER_OK;

    case ARM_SAI_CONTROL_RX:
      stream = sai->rx;
      if ((arg1 & 1U) == 0U) {
        // Disable FREQ interrupt
        stream->reg->IMR &= ~SAI_xIMR_FREQIE;

        // Disable receiver
        while ((stream->reg->CR1 & SAI_xCR1_SAIEN) != 0) {
          stream->reg->CR1 &= ~SAI_xCR1_SAIEN;
        }
      } else {

        // Enable receiver
        stream->reg->CR1 |=  SAI_xCR1_SAIEN;

#ifdef __SAI_DMA
        if (stream->dma == NULL) {
#endif
          if (sai->info->status.rx_busy != 0U) {
            // Enable FREQ interrupt
            stream->reg->IMR |= SAI_xIMR_FREQIE;
          }
#ifdef __SAI_DMA
        }
#endif

        // Enable receive overflow interrupt
        stream->reg->IMR |= SAI_xIMR_OVRUDRIE;
      }
      return ARM_DRIVER_OK;

    case ARM_SAI_MASK_SLOTS_TX:
      stream = sai->tx;
      if (stream->info->protocol != ARM_SAI_PROTOCOL_USER) { return ARM_DRIVER_ERROR; }

      arg1 =~ arg1;

      // Max slot number is 16
      if ((arg1 & 0xFFFF0000) != 0U) { return ARM_DRIVER_ERROR; }

      if (stream->reg->CR1 & SAI_xCR1_SAIEN) {
        stream_en = 1U;

        imr = (stream->reg->IMR & SAI_xIMR_FREQIE);
        stream->reg->IMR &= ~ SAI_xIMR_FREQIE;

        // Disable SAI block
        stream->reg->CR1 &= ~SAI_xCR1_SAIEN;
        while ((stream->reg->CR1 & SAI_xCR1_SAIEN) != 0);
      } else {
        stream_en = 0U;
      }

      stream->reg->SLOTR = (stream->reg->SLOTR & 0x0FFFF) | (arg1 << 16);

      if (stream_en == 1U) {
        stream->reg->CR1 |= SAI_xCR1_SAIEN;
        stream->reg->IMR |= imr;
      }

      return ARM_DRIVER_OK;
    case ARM_SAI_MASK_SLOTS_RX:
      stream = sai->rx;
      if (stream->info->protocol != ARM_SAI_PROTOCOL_USER) { return ARM_DRIVER_ERROR; }

      arg1 =~ arg1;

      // Max slot number is 16
      if ((arg1 & 0xFFFF0000) != 0U) { return ARM_DRIVER_ERROR; }

      if (stream->reg->CR1 & SAI_xCR1_SAIEN) {
        stream_en = 1U;

        imr = (stream->reg->IMR & SAI_xIMR_FREQIE);
        stream->reg->IMR &= ~ SAI_xIMR_FREQIE;

        // Disable SAI block
        stream->reg->CR1 &= ~SAI_xCR1_SAIEN;
        while ((stream->reg->CR1 & SAI_xCR1_SAIEN) != 0);
      } else {
        stream_en = 0U;
      }

      stream->reg->SLOTR = (stream->reg->SLOTR & 0x0FFFF) | (arg1 << 16);

      if (stream_en == 1U) {
        stream->reg->CR1 |= SAI_xCR1_SAIEN;
        stream->reg->IMR |= imr;
      }

      return ARM_DRIVER_OK;

    case ARM_SAI_ABORT_SEND:
      stream = sai->tx;
      // Disable FIFO request interrupt
      stream->reg->IMR &= ~SAI_xIMR_FREQIE;

      // Disable DMA channel
#ifdef __SAI_DMA
      // Disable DMA
      stream->reg->CR1 &= ~(SAI_xCR1_DMAEN);
      while ((stream->reg->CR1 & SAI_xCR1_DMAEN) != 0);

      // If DMA mode - disable DMA channel
      if (stream->dma != NULL) {
        HAL_DMA_Abort (stream->dma->hdma);
      }
#endif

      // Flush TX FIFO
      stream->reg->CR2 |= SAI_xCR2_FFLUSH;

      // Reset counters
      stream->info->cnt = 0U;
      stream->info->num = 0U;

      // Clear busy flag
      sai->info->status.tx_busy = 0U;

      return ARM_DRIVER_OK;

    case ARM_SAI_ABORT_RECEIVE:
      stream = sai->rx;
      // Disable FIFO request interrupt
      stream->reg->IMR &= ~SAI_xIMR_FREQIE;

      // Disable DMA channel
#ifdef __SAI_DMA
      // Disable DMA
      stream->reg->CR1 &= ~(SAI_xCR1_DMAEN);
      while ((stream->reg->CR1 & SAI_xCR1_DMAEN) != 0);

      // If DMA mode - disable DMA channel
      if (stream->dma != NULL) {
        HAL_DMA_Abort (stream->dma->hdma);
      }
#endif

      // Flush RX FIFO
      stream->reg->CR2 |= SAI_xCR2_FFLUSH;

      // Reset counters
      stream->info->cnt = 0U;
      stream->info->num = 0U;

      // Clear busy flag
      sai->info->status.rx_busy = 0U;
      return ARM_DRIVER_OK;

    default: return ARM_DRIVER_ERROR;
  }

  // Mode
  switch (control & ARM_SAI_MODE_Msk) {
    case ARM_SAI_MODE_MASTER:
      master = 1U;
      if ((control & ARM_SAI_CONTROL_Msk) == ARM_SAI_CONFIGURE_RX) {
        // Master receiver
        cr1 |= SAI_xCR1_MODE_0;
      }
      break;
    case ARM_SAI_MODE_SLAVE:
      master = 0U;
      if ((control & ARM_SAI_CONTROL_Msk) == ARM_SAI_CONFIGURE_TX) {
        // Slave transmitter
        cr1 |= SAI_xCR1_MODE_1;
      } else {
        // Slave receiver
        cr1 |= SAI_xCR1_MODE;
      }
      cr1 |= SAI_xCR1_NODIV;
      break;
    default: return ARM_DRIVER_ERROR;
  }

  // Synchronization
  switch (control & ARM_SAI_SYNCHRONIZATION_Msk) {
    case ARM_SAI_ASYNCHRONOUS:
      // Stream in asynchronous mode
      break;
    case ARM_SAI_SYNCHRONOUS:
      if (stream->sync_source == SAI_SYNC_SRC_INTERNAL_SUB_BLOCK) {
        // Stream synchronous with the other internal sub-block
        cr1 |= SAI_xCR1_SYNCEN_0;
      } else {
        // Stream synchronous with an external SAI embedded peripheral
        cr1 |= SAI_xCR1_SYNCEN_1;
      }
      break;
    default: return ARM_SAI_ERROR_SYNCHRONIZATION;
  }

  // Get Data size
  stream->info->data_bits = ((control & ARM_SAI_DATA_SIZE_Msk) >> ARM_SAI_DATA_SIZE_Pos) + 1U;
  switch (stream->info->data_bits) {
    case 8:  cr1 |= (2U << 5); break;
    case 10: cr1 |= (3U << 5); break;
    case 16: cr1 |= (4U << 5); break;
    case 20: cr1 |= (5U << 5); break;
    case 24: cr1 |= (6U << 5); break;
    case 32: cr1 |= (7U << 5); break;
    default: return ARM_SAI_ERROR_DATA_SIZE;
  }

  // Get Bit Order
  bit_order = control & ARM_SAI_BIT_ORDER_Msk;

  // Clock polarity
  clock_pol = control & ARM_SAI_CLOCK_POLARITY_Msk;

  // Get Frame length
  frame_len = ((arg1 & ARM_SAI_FRAME_LENGTH_Msk) >> ARM_SAI_FRAME_LENGTH_Pos) + 1U;
  // Check if default value
  if (frame_len == 1U) { frame_len = 0U; }

  // Get Slot size
  switch (arg1 & ARM_SAI_SLOT_SIZE_Msk) {
    case ARM_SAI_SLOT_SIZE_DEFAULT: slot_sz = stream->info->data_bits; break;
    case ARM_SAI_SLOT_SIZE_16:      slot_sz = 16;                      break;
    case ARM_SAI_SLOT_SIZE_32:      slot_sz = 32;                      break;
    default: return ARM_SAI_ERROR_SLOT_SIZE;
  }

  if (slot_sz < stream->info->data_bits) {return ARM_SAI_ERROR_SLOT_SIZE; }

  // Get Agument1 values
  frame_sync_width = ((arg1 & ARM_SAI_FRAME_SYNC_WIDTH_Msk) >> ARM_SAI_FRAME_SYNC_WIDTH_Pos) + 1U;
  frame_sync_pol   =   arg1 & ARM_SAI_FRAME_SYNC_POLARITY_Msk;
  frame_sync_early = ((arg1 & ARM_SAI_FRAME_SYNC_EARLY) == 0) ? false : true;
  slot_count       = ((arg1 & ARM_SAI_SLOT_COUNT_Msk) >> ARM_SAI_SLOT_COUNT_Pos) + 1;
  slot_offset      =  (arg1 & ARM_SAI_SLOT_OFFSET_Msk) >> ARM_SAI_SLOT_OFFSET_Pos;

  // Protocol
  stream->info->protocol = control & ARM_SAI_PROTOCOL_Msk;

  switch (stream->info->protocol) {
    // User Protocol
    case ARM_SAI_PROTOCOL_USER:
      if (frame_len < 8U)     { return ARM_SAI_ERROR_FRAME_LENGHT; }
      if (frame_len > 1024U)  { return ARM_SAI_ERROR_FRAME_LENGHT; }
      break;

    // I2S
    case ARM_SAI_PROTOCOL_I2S:
      if (clock_pol != ARM_SAI_CLOCK_POLARITY_0) {
        return ARM_SAI_ERROR_CLOCK_POLARITY;
      }
      if (frame_len == 0U) {
        frame_len = stream->info->data_bits * 2U;
      } else {
        if (frame_len != (stream->info->data_bits * 2U)) { return ARM_SAI_ERROR_FRAME_LENGHT; }
      }

      bit_order        = ARM_SAI_MSB_FIRST;
      frame_sync_width = frame_len / 2U;
      frame_sync_pol   = ARM_SAI_FRAME_SYNC_POLARITY_LOW;
      frame_sync_early = true;
      slot_count       = 2U;
      slot_sz          = stream->info->data_bits;
      slot_offset      = 0U;
      frcr            |= SAI_xFRCR_FSDEF;
      break;

    // MSB Justified
    case ARM_SAI_PROTOCOL_MSB_JUSTIFIED:
      if (clock_pol != ARM_SAI_CLOCK_POLARITY_0) {
        return ARM_SAI_ERROR_CLOCK_POLARITY;
      }
      if (frame_len == 0U) {
        frame_len = slot_sz * 2U;
      } else {
        if (frame_len != (slot_sz * 2)) {
          return ARM_SAI_ERROR_FRAME_LENGHT;
        }
      }

      bit_order        = ARM_SAI_MSB_FIRST;
      frame_sync_width = frame_len / 2U;
      frame_sync_pol   = ARM_SAI_FRAME_SYNC_POLARITY_HIGH;
      frame_sync_early = false;
      slot_count       = 2U;
      slot_offset      = 0U;
      frcr            |= SAI_xFRCR_FSDEF;
      break;

    // LSB Justified
    case ARM_SAI_PROTOCOL_LSB_JUSTIFIED:
      if (clock_pol != ARM_SAI_CLOCK_POLARITY_0) {
        return ARM_SAI_ERROR_CLOCK_POLARITY;
      }
      if (frame_len == 0U) {
        frame_len = slot_sz * 2U;
      } else {
        if (frame_len != (slot_sz * 2)) {
          return ARM_SAI_ERROR_FRAME_LENGHT;
        }
      }

      bit_order        = ARM_SAI_MSB_FIRST;
      frame_sync_width = frame_len / 2U;
      frame_sync_pol   = ARM_SAI_FRAME_SYNC_POLARITY_HIGH;
      frame_sync_early = false;
      slot_count       = 2U;
      slot_offset      = slot_sz - stream->info->data_bits;
      frcr            |= SAI_xFRCR_FSDEF;
      break;

    // PCM Short
    case ARM_SAI_PROTOCOL_PCM_SHORT:
      if (clock_pol != ARM_SAI_CLOCK_POLARITY_0) {
        return ARM_SAI_ERROR_CLOCK_POLARITY;
      }

      if (frame_len == 0U) {
        frame_len = slot_sz;
      } else {
        if (frame_len != slot_sz) {
          return ARM_SAI_ERROR_FRAME_LENGHT;
        }
      }

      bit_order        = ARM_SAI_MSB_FIRST;
      frame_sync_width = 1U;
      frame_sync_pol   = ARM_SAI_FRAME_SYNC_POLARITY_HIGH;
      frame_sync_early = true;
      slot_count       = 1U;
      slot_offset      = 0U;
      break;

    // PCM Long
    case ARM_SAI_PROTOCOL_PCM_LONG:
      if (stream->info->data_bits < 16U) {
        return ARM_SAI_ERROR_DATA_SIZE;
      }

      if (clock_pol != ARM_SAI_CLOCK_POLARITY_0) {
        return ARM_SAI_ERROR_CLOCK_POLARITY;
      }

      if (frame_len == 0U) {
        frame_len = slot_sz;
      } else {
        if (frame_len != slot_sz) {
          return ARM_SAI_ERROR_FRAME_LENGHT;
        }
      }

      bit_order        = ARM_SAI_MSB_FIRST;
      frame_sync_width = 13U;
      frame_sync_pol   = ARM_SAI_FRAME_SYNC_POLARITY_HIGH;
      frame_sync_early = false;
      slot_count       = 1U;
      slot_offset      = 0U;
      break;

    // AC97
    case ARM_SAI_PROTOCOL_AC97:

      // AC97 mode
      cr1 |= SAI_xCR1_PRTCFG_1;
      break;
    default: return ARM_SAI_ERROR_PROTOCOL;
  }

  // Frame Length
  frcr |= frame_len - 1U;

  // Bit Order
  if (bit_order == ARM_SAI_LSB_FIRST) { cr1 |= SAI_xCR1_LSBFIRST; }

  // Mono mode
  if (control & ARM_SAI_MONO_MODE)    { cr1 |= SAI_xCR1_MONO; }

  // Companding
  switch (control & ARM_SAI_COMPANDING_Msk) {
    case ARM_SAI_COMPANDING_NONE:                          break;
    case ARM_SAI_COMPANDING_U_LAW: cr2 |= SAI_xCR2_COMP_1; break;
    case ARM_SAI_COMPANDING_A_LAW: cr2 |= SAI_xCR2_COMP;   break;
    default: return ARM_SAI_ERROR_COMPANDING;
  }

  // Clock polarity
  if (clock_pol == ARM_SAI_CLOCK_POLARITY_0) { cr1 |= SAI_xCR1_CKSTR; }

  // Master clock pin
  switch (control & ARM_SAI_MCLK_PIN_Msk) {
    case ARM_SAI_MCLK_PIN_INACTIVE: 
      mclk_pin = 0U;
      break;
    case ARM_SAI_MCLK_PIN_OUTPUT:
      if ((master == 1U) && (stream->io.mclk != NULL)) {
        mclk_pin = 1U;
        break;
      }
    case ARM_SAI_MCLK_PIN_INPUT:
    default: return ARM_SAI_ERROR_MCLK_PIN;
  }

  // Frame Sync Width
  frcr |= (frame_sync_width - 1) << 8;

  // Frame sync polarity
  if (frame_sync_pol == ARM_SAI_FRAME_SYNC_POLARITY_HIGH) {
    frcr |= SAI_xFRCR_FSPO;
  }

  // Frame sync early
  if (frame_sync_early == true) { frcr |= SAI_xFRCR_FSOFF; }

  // Slot Count
  slotr |= (slot_count - 1) << 8;

  // Slot size
  if ((arg1 & ARM_SAI_SLOT_SIZE_Msk) != ARM_SAI_SLOT_SIZE_DEFAULT) {
    if (slot_sz == 16) { slotr |= SAI_xSLOTR_SLOTSZ_0; }
    if (slot_sz == 32) { slotr |= SAI_xSLOTR_SLOTSZ_1; }
  }

  // Slot offset
  slotr |= slot_offset;

  // Audio Frequency
  if (master == 1U) {
    // WS and SCK are generated only by master

    freq  = HAL_RCCEx_GetPeriphCLKFreq (sai->periph_clk);

    freq = (((freq * 10) / ((arg2 & ARM_SAI_AUDIO_FREQ_Msk) * 512)));
    val  = freq / 10;

    // Round result to the nearest integer
    if((freq % 10) > 8) { val += 1; }

    cr1 |= (val & 0x0F) << 20;

    if (mclk_pin == 1U) {
      // MCLK Prescaler
      val = (((arg2 & ARM_SAI_MCLK_PRESCALER_Msk) >> ARM_SAI_MCLK_PRESCALER_Pos) + 1U);
      if (val != 256U) {return ARM_SAI_ERROR_MCLK_PRESCALER;}
    }
  }

  // Configure registers
  cr2   |= (stream->reg->CR2 & SAI_xCR2_MUTE);
  slotr |= (stream->reg->SLOTR & SAI_xSLOTR_SLOTEN);

  if (stream->reg->CR1 & SAI_xCR1_SAIEN) {
    cr1 |= SAI_xCR1_SAIEN;
    imr  = (stream->reg->IMR & SAI_xIMR_FREQIE) ;

    // Disable FREQ interrupt
    stream->reg->IMR &= ~SAI_xIMR_FREQIE;

    // Disable SAI block
    stream->reg->CR1 &= ~SAI_xCR1_SAIEN;
    while ((stream->reg->CR1 & SAI_xCR1_SAIEN) != 0U);
  }

  stream->reg->CR2   = cr2;
  stream->reg->FRCR  = frcr;
  stream->reg->SLOTR = slotr;
  stream->reg->CR1   = (cr1 & ~(SAI_xCR1_SAIEN));

  if (cr1 & SAI_xCR1_SAIEN) {
    stream->reg->CR1 |= SAI_xCR1_SAIEN;
    while ((stream->reg->CR1 & SAI_xCR1_SAIEN) == 0U);
    stream->reg->IMR |= imr;

    if ((stream->reg->CR1 & SAI_xCR1_MODE) == 0U) {
       // Master transmitter mode: send dummy value to start generating clocks
      stream->reg->DR = 0U;
    }
  }

  sai->info->flags |= SAI_FLAG_CONFIGURED;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_SAI_STATUS SAI_GetStatus (SAI_RESOURCES *sai)
  \brief       Get SAI status.
  \param[in]   sai  Pointer to SAI resources
  \return      SAI status \ref ARM_SAI_STATUS
*/
static  ARM_SAI_STATUS SAI_GetStatus (SAI_RESOURCES *sai) {
  ARM_SAI_STATUS status;

  status.frame_error  = sai->info->status.frame_error;
  status.rx_busy      = sai->info->status.rx_busy;
  status.rx_overflow  = sai->info->status.rx_overflow;
  status.tx_busy      = sai->info->status.tx_busy;
  status.tx_underflow = sai->info->status.tx_underflow;

  return status;
}

/* SAI IRQ Handler */
void SAI_IRQHandler (SAI_RESOURCES *sai) {
  uint32_t     sr, data, event;
  SAI_STREAM  *stream;

  event = 0U;

  // Receive
  stream = sai->rx;
  if (stream->reg->CR1 & SAI_xCR1_SAIEN) {
    // SAI transmit block is enabled

    // Read status register
    sr = stream->reg->SR;

    // FIFO request
    if (((sr & SAI_xSR_FREQ) != 0U) & (sai->info->status.rx_busy == 1U)) {
      while (stream->info->cnt != stream->info->num) {
        // Check is FIFO is empty
        if (((stream->reg->SR & SAI_xSR_FLVL) >> 16) == 0U) { break; }

        data = stream->reg->DR;

        // Put data into buffer
        *(stream->info->buf++) = (uint8_t)data;
        if (stream->info->data_bits > 8U) {
          *(stream->info->buf++) = (uint8_t)(data >> 8);
          if (stream->info->data_bits > 16U) {
            *(stream->info->buf++) = (uint8_t)(data >> 16);
            *(stream->info->buf++) = (uint8_t)(data >> 24);
          }
        }
        stream->info->cnt++;
      }

      if (stream->info->cnt == stream->info->num) {
        // Disable FIFO request interrupt
        stream->reg->IMR &= ~SAI_xIMR_FREQIE;

        // Clear busy flag
        sai->info->status.rx_busy = 0U;

        // Set event
        event |= ARM_SAI_EVENT_RECEIVE_COMPLETE;
      }
    }

    // Receive Overrun
    if (sr & SAI_xSR_OVRUDR) {
      // Set overflow flag
      sai->info->status.rx_overflow = 1U;

      // Clear interrupt flag
      stream->reg->CLRFR = SAI_xCLRFR_COVRUDR;

      // Set event
      event |= ARM_SAI_EVENT_RX_OVERFLOW;
    }

    // Late frame / anticipated frame synchronization detection
    if (sr & (SAI_xSR_AFSDET | SAI_xSR_LFSDET)) {

      // Only in slave mode
      if (stream->reg->CR1 & SAI_xCR1_MODE_1) {
        // Set error flag
        sai->info->status.frame_error = 1U;

        // Clear interrupt flag
        stream->reg->CLRFR = (SAI_xCLRFR_CLFSDET | SAI_xCLRFR_CAFSDET);

        // Set event
        event |= ARM_SAI_EVENT_FRAME_ERROR;
      }
    }
  }

  // Transmit
  stream = sai->tx;
  if (stream->reg->CR1 & SAI_xCR1_SAIEN) {
    // SAI transmit block is enabled

    // Read status register
    sr = stream->reg->SR;

    // FIFO request
    if (((sr & SAI_xSR_FREQ) != 0U) & (sai->info->status.tx_busy == 1U)) {
      while (stream->info->cnt != stream->info->num) {
        // Check is FIFO is full
        if (((stream->reg->SR & SAI_xSR_FLVL) >> 16) == 5U) { break; }

        data = *(stream->info->buf++);
        if (stream->info->data_bits > 8U) {
          data |= *(stream->info->buf++) << 8;
          if (stream->info->data_bits > 16U) {
            data |= *(stream->info->buf++) << 16;
            data |= *(stream->info->buf++) << 24;
          }
        }
        // Write data to FIFO
        stream->reg->DR = data;
        stream->info->cnt++;
      }

      if (stream->info->cnt == stream->info->num) {
        // Disable FIFO request interrupt
        stream->reg->IMR &= ~SAI_xIMR_FREQIE;

        // Clear busy flag
        sai->info->status.tx_busy = 0U;

        // Set event
        event |= ARM_SAI_EVENT_SEND_COMPLETE;
      }
    }

    // Transmit Under-run
    if (sr & SAI_xSR_OVRUDR) {
      // Set underflow flag
      sai->info->status.tx_underflow = 1U;

      // Clear interrupt flag
      stream->reg->CLRFR = SAI_xCLRFR_COVRUDR;

      // Set event
      event |= ARM_SAI_EVENT_TX_UNDERFLOW;
    }

    // Late frame / anticipated frame synchronization detection
    if (sr & (SAI_xSR_AFSDET | SAI_xSR_LFSDET)) {

      // Only in slave mode
      if (stream->reg->CR1 & SAI_xCR1_MODE_1) {
        // Set error flag
        sai->info->status.frame_error = 1U;

        // Clear interrupt flag
        stream->reg->CLRFR = (SAI_xCLRFR_CLFSDET | SAI_xCLRFR_CAFSDET);

        // Set event
        event |= ARM_SAI_EVENT_FRAME_ERROR;
      }
    }
  }




  if ((event != 0U) && (sai->info->cb_event != NULL)) {
    sai->info->cb_event(event);
  }
}

#if ((defined(MX_SAI1_A_DMA_Instance) && (SAI1_RX_BLOCK == SAI_BLOCK_A)) || \
     (defined(MX_SAI1_B_DMA_Instance) && (SAI1_RX_BLOCK == SAI_BLOCK_B)) || \
     (defined(MX_SAI2_A_DMA_Instance) && (SAI2_RX_BLOCK == SAI_BLOCK_A)) || \
     (defined(MX_SAI2_B_DMA_Instance) && (SAI2_RX_BLOCK == SAI_BLOCK_B)))
/* SAI RX DMA Handler */
void SAI_RX_DMA_Complete (SAI_RESOURCES *sai) {

  if ((__HAL_DMA_GET_COUNTER(sai->rx->dma->hdma) != 0) && (sai->rx->info->num != 0)) {
    // RX DMA Complete caused by transfer abort
    return;
  }

  sai->rx->info->cnt = sai->rx->info->num;

  sai->info->status.rx_busy = 0U;
  if (sai->info->cb_event != NULL) {
    sai->info->cb_event(ARM_SAI_EVENT_RECEIVE_COMPLETE);
  }
}
#endif


#if ((defined(MX_SAI1_A_DMA_Instance) && (SAI1_TX_BLOCK == SAI_BLOCK_A)) || \
     (defined(MX_SAI1_B_DMA_Instance) && (SAI1_TX_BLOCK == SAI_BLOCK_B)) || \
     (defined(MX_SAI2_A_DMA_Instance) && (SAI2_TX_BLOCK == SAI_BLOCK_A)) || \
     (defined(MX_SAI2_B_DMA_Instance) && (SAI2_TX_BLOCK == SAI_BLOCK_B)))
/* SAI TX DMA Handler */
void SAI_TX_DMA_Complete (SAI_RESOURCES *sai) {

  if ((__HAL_DMA_GET_COUNTER(sai->tx->dma->hdma) != 0) && (sai->tx->info->num != 0)) {
    // TX DMA Complete caused by transfer abort
    return;
  }

  sai->tx->info->cnt = sai->tx->info->num;

  sai->info->status.tx_busy = 0U;
  if (sai->info->cb_event != NULL) {
    sai->info->cb_event(ARM_SAI_EVENT_SEND_COMPLETE);
  }
}
#endif


// SAI1
#ifdef MX_SAI1
static ARM_SAI_CAPABILITIES SAI1_GetCapabilities     (void)                                           { return SAI_GetCapabilities (&SAI1_Resources); }
static int32_t              SAI1_Initialize          (ARM_SAI_SignalEvent_t cb_event)                 { return SAI_Initialize (cb_event, &SAI1_Resources); }
static int32_t              SAI1_Uninitialize        (void)                                           { return SAI_Uninitialize (&SAI1_Resources); }
static int32_t              SAI1_PowerControl        (ARM_POWER_STATE state)                          { return SAI_PowerControl (state, &SAI1_Resources); }
static int32_t              SAI1_Send                (const void *data, uint32_t num)                 { return SAI_Send (data, num, &SAI1_Resources); }
static int32_t              SAI1_Receive             (      void *data, uint32_t num)                 { return SAI_Receive (data, num, &SAI1_Resources); }
static uint32_t             SAI1_GetTxCount          (void)                                           { return SAI_GetTxCount (&SAI1_Resources); }
static uint32_t             SAI1_GetRxCount          (void)                                           { return SAI_GetRxCount (&SAI1_Resources); }
static int32_t              SAI1_Control             (uint32_t control, uint32_t arg1, uint32_t arg2) { return SAI_Control (control, arg1, arg2, &SAI1_Resources); }
static ARM_SAI_STATUS       SAI1_GetStatus           (void)                                           { return SAI_GetStatus (&SAI1_Resources); }
       void                 SAI1_IRQHandler          (void)                                           {        SAI_IRQHandler (&SAI1_Resources); }

#ifdef MX_SAI1_A_DMA_Instance
void SAI1_A_DMA_Complete (DMA_HandleTypeDef *hdma) {
#if (SAI1_RX_BLOCK == SAI_BLOCK_A)
  SAI_RX_DMA_Complete (&SAI1_Resources);
#endif
#if (SAI1_TX_BLOCK == SAI_BLOCK_A)
  SAI_TX_DMA_Complete (&SAI1_Resources);
#endif
}

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SAI1_A_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SAI1_A_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sai1_a);
}
#endif
#endif

#ifdef MX_SAI1_B_DMA_Instance
void SAI1_B_DMA_Complete (DMA_HandleTypeDef *hdma) {
#if (SAI1_RX_BLOCK == SAI_BLOCK_B)
  SAI_RX_DMA_Complete (&SAI1_Resources);
#endif
#if (SAI1_TX_BLOCK == SAI_BLOCK_B)
  SAI_TX_DMA_Complete (&SAI1_Resources);
#endif
}

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SAI1_B_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SAI1_B_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sai1_b);
}
#endif
#endif


// SAI1 Driver Control Block
ARM_DRIVER_SAI Driver_SAI1 = {
    SAI_GetVersion,
    SAI1_GetCapabilities,
    SAI1_Initialize,
    SAI1_Uninitialize,
    SAI1_PowerControl,
    SAI1_Send,
    SAI1_Receive,
    SAI1_GetTxCount,
    SAI1_GetRxCount,
    SAI1_Control,
    SAI1_GetStatus
};
#endif /* MX_SAI1 */

// SAI2
#ifdef MX_SAI2
static ARM_SAI_CAPABILITIES SAI2_GetCapabilities     (void)                                           { return SAI_GetCapabilities (&SAI2_Resources); }
static int32_t              SAI2_Initialize          (ARM_SAI_SignalEvent_t cb_event)                 { return SAI_Initialize (cb_event, &SAI2_Resources); }
static int32_t              SAI2_Uninitialize        (void)                                           { return SAI_Uninitialize (&SAI2_Resources); }
static int32_t              SAI2_PowerControl        (ARM_POWER_STATE state)                          { return SAI_PowerControl (state, &SAI2_Resources); }
static int32_t              SAI2_Send                (const void *data, uint32_t num)                 { return SAI_Send (data, num, &SAI2_Resources); }
static int32_t              SAI2_Receive             (      void *data, uint32_t num)                 { return SAI_Receive (data, num, &SAI2_Resources); }
static uint32_t             SAI2_GetTxCount          (void)                                           { return SAI_GetTxCount (&SAI2_Resources); }
static uint32_t             SAI2_GetRxCount          (void)                                           { return SAI_GetRxCount (&SAI2_Resources); }
static int32_t              SAI2_Control             (uint32_t control, uint32_t arg1, uint32_t arg2) { return SAI_Control (control, arg1, arg2, &SAI2_Resources); }
static ARM_SAI_STATUS       SAI2_GetStatus           (void)                                           { return SAI_GetStatus (&SAI2_Resources); }
       void                 SAI2_IRQHandler          (void)                                           {        SAI_IRQHandler (&SAI2_Resources); }

#ifdef MX_SAI2_A_DMA_Instance
void SAI2_A_DMA_Complete (DMA_HandleTypeDef *hdma) {
#if (SAI2_RX_BLOCK == SAI_BLOCK_A)
  SAI_RX_DMA_Complete (&SAI2_Resources);
#endif
#if (SAI2_TX_BLOCK == SAI_BLOCK_A)
  SAI_TX_DMA_Complete (&SAI2_Resources);
#endif
}

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SAI2_A_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SAI2_A_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sai2_a);
}
#endif
#endif

#ifdef MX_SAI2_B_DMA_Instance
void SAI2_B_DMA_Complete (DMA_HandleTypeDef *hdma) {
#if (SAI2_RX_BLOCK == SAI_BLOCK_B)
  SAI_RX_DMA_Complete (&SAI2_Resources);
#endif
#if (SAI2_TX_BLOCK == SAI_BLOCK_B)
  SAI_TX_DMA_Complete (&SAI2_Resources);
#endif
}

#ifdef RTE_DEVICE_FRAMEWORK_CLASSIC
void SAI2_B_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_SAI2_B_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sai2_b);
}
#endif
#endif


// SAI2 Driver Control Block
ARM_DRIVER_SAI Driver_SAI2 = {
    SAI_GetVersion,
    SAI2_GetCapabilities,
    SAI2_Initialize,
    SAI2_Uninitialize,
    SAI2_PowerControl,
    SAI2_Send,
    SAI2_Receive,
    SAI2_GetTxCount,
    SAI2_GetRxCount,
    SAI2_Control,
    SAI2_GetStatus
};
#endif /* MX_SAI2 */
