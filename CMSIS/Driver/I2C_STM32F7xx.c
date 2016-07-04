/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2016 ARM Ltd.
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
 * $Date:        24. May 2016
 * $Revision:    V1.5
 *
 * Driver:       Driver_I2C1, Driver_I2C2, Driver_I2C3, Driver_I2C4
 * Configured:   via RTE_Device.h configuration file
 * Project:      I2C Driver for ST STM32F7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   I2C Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_I2C# = 1       use I2C1
 *   Connect to hardware via Driver_I2C# = 2       use I2C2
 *   Connect to hardware via Driver_I2C# = 3       use I2C3
 *   Connect to hardware via Driver_I2C# = 4       use I2C4
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.5
 *    Added port configuration for ports supported by new subfamilies.
 *  Version 1.4
 *    Corrected event signaling and data counter value when stop condition is detected
 *    Corrected register CR2 handling in MasterReceive function (AUTOEND/RELOAD)
 *  Version 1.3
 *    Corrected PowerControl function for:
 *      - Unconditional Power Off
 *      - Conditional Power full (driver must be initialized)
 *  Version 1.2
 *    Own address setting corrected
 *  Version 1.1
 *    Enhanced STM32CubeMx compatibility
 *  Version 1.0
 *    Initial release
 */
 
 /*! \page stm32f7_i2c CMSIS-Driver I2C Setup 

The CMSIS-Driver I2C requires:
  - Setup of I2Cx input clock
  - Setup of I2Cx in I2C mode with optional DMA for Rx and Tx transfers
 
The example below uses correct settings for STM32F746G-Discovery:
  - I2C3 Mode: I2C

For different boards, refer to the hardware schematics to reflect correct setup values.

The STM32CubeMX configuration steps for Pinout, Clock, and System Configuration are 
listed below. Enter the values that are marked \b bold.
   
Pinout tab
----------
  1. Configure mode
    - Peripherals \b I2C3: Mode=<b>I2C</b>

  2. Configure pin PH7 and pin PH8 as I2C3 peripheral alternative pins
    - Click in chip diagram on pin \b PH7 and select \b I2C3_SCL
    - Click in chip diagram on pin \b PH8 and select \b I2C3_SDA

Clock Configuration tab
-----------------------
  1. Configure I2C3 Clock

Configuration tab
-----------------
  1. Under Connectivity open \b I2C3 Configuration:
     - \e optional <b>DMA Settings</b>: setup DMA transfers for Rx and Tx\n
       \b Add - Select \b I2C3_RX: Stream=DMA1 Stream 1, Direction=Peripheral to Memory, Priority=Low
          DMA Request Settings         | Label             | Peripheral | Memory
          :----------------------------|:------------------|:-----------|:-------------
          Mode: Normal                 | Increment Address | OFF        |\b ON
          Use Fifo OFF Threshold: Full | Data Width        |\b Byte     | Byte
          .                            | Burst Size        | Single     | Single
       \b Add - Select \b I2C3_TX: Stream=DMA1 Stream 4, Direction=Memory to Peripheral, Priority=Low
          DMA Request Settings         | Label             | Peripheral | Memory
          :----------------------------|:------------------|:-----------|:-------------
          Mode: Normal                 | Increment Address | OFF        |\b ON
          Use Fifo OFF Threshold: Full | Data Width        |\b Byte     | Byte
          .                            | Burst Size        | Single     | Single

     - <b>GPIO Settings</b>: review settings, no changes required
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PH7      | I2C3_SCL      | Alternate | Pull-up            | High        |.
          PH8      | I2C3_SDA      | Alternate | Pull-up            | High        |.

     - <b>NVIC Settings</b>: enable interrupts
          Interrupt Table                      | Enable | Preemption Priority | Sub Priority
          :------------------------------------|:-------|:--------------------|:--------------
          DMA1 stream1 global interrupt        |   ON   | 0                   | 0
          DMA1 stream4 global interrupt        |   ON   | 0                   | 0
          I2C3 event interrupt                 |\b ON   | 0                   | 0
          I2C3 error interrupt                 |\b ON   | 0                   | 0

     - Parameter Settings: not used
     - User Constants: not used
   
     Click \b OK to close the I2C3 Configuration dialog
*/

/*! \cond */

#include "I2C_STM32F7xx.h"

#define ARM_I2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,5)    /* driver version */


#if defined(MX_I2C1_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c1_rx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c1_rx;
#endif

void I2C1_RX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C1_RX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C1_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c1_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c1_tx;
#endif

void I2C1_TX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C1_TX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C2_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c2_rx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c2_rx;
#endif

void I2C2_RX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C2_RX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C2_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c2_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c2_tx;
#endif

void I2C2_TX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C2_TX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C3_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c3_rx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c3_rx;
#endif

void I2C3_RX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C3_RX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C3_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c3_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c3_tx;
#endif

void I2C3_TX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C3_TX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C4_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c4_rx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c4_rx;
#endif

void I2C4_RX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C4_RX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif

#if defined(MX_I2C4_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_i2c4_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_i2c4_tx;
#endif

void I2C4_TX_DMA_Complete(DMA_HandleTypeDef *hdma);
void I2C4_TX_DMA_Error   (DMA_HandleTypeDef *hdma);
#endif


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_I2C_API_VERSION,
  ARM_I2C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES DriverCapabilities = { 0U };


#if defined(MX_I2C1)
/* Function prototypes */
void I2C1_EV_IRQHandler (void);
void I2C1_ER_IRQHandler (void);

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern I2C_HandleTypeDef hi2c1;
#endif

/* I2C1 Rx DMA */
#if defined(MX_I2C1_RX_DMA_Instance)
static const I2C_DMA I2C1_RX_DMA = {
  &hdma_i2c1_rx,
  &I2C1_RX_DMA_Complete,
  &I2C1_RX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C1_RX_DMA_Instance,
  MX_I2C1_RX_DMA_IRQn,
  MX_I2C1_RX_DMA_Channel,
  MX_I2C1_RX_DMA_Priority
  #endif
};
#endif

/* I2C1 Tx DMA */
#if defined(MX_I2C1_TX_DMA_Instance)
static const I2C_DMA I2C1_TX_DMA = {
  &hdma_i2c1_tx,
  &I2C1_TX_DMA_Complete,
  &I2C1_TX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C1_TX_DMA_Instance,
  MX_I2C1_TX_DMA_IRQn,
  MX_I2C1_TX_DMA_Channel,
  MX_I2C1_TX_DMA_Priority
  #endif
};
#endif

/* I2C1 Information (Run-Time) */
static I2C_INFO I2C1_Info;

/* I2C1 Resources */
static I2C_RESOURCES I2C1_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hi2c1,
#endif
  I2C1,
#if defined(MX_I2C1_RX_DMA_Instance)
  &I2C1_RX_DMA,
#else
  NULL,
#endif
#if defined(MX_I2C1_TX_DMA_Instance)
  &I2C1_TX_DMA,
#else
  NULL,
#endif
  {
    MX_I2C1_SCL_GPIOx,
    MX_I2C1_SDA_GPIOx,
    MX_I2C1_SCL_GPIO_Pin,
    MX_I2C1_SDA_GPIO_Pin,
    MX_I2C1_SCL_GPIO_PuPdOD,
    MX_I2C1_SDA_GPIO_PuPdOD,
    MX_I2C1_SCL_GPIO_AF,
    MX_I2C1_SDA_GPIO_AF
  },
  I2C1_EV_IRQn,
  I2C1_ER_IRQn,
  &I2C1_Info
};

#endif /* MX_I2C1 */

#if defined(MX_I2C2)
/* Function prototypes */
void I2C2_EV_IRQHandler (void);
void I2C2_ER_IRQHandler (void);

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern I2C_HandleTypeDef hi2c2;
#endif

/* I2C2 Rx DMA */
#if defined(MX_I2C2_RX_DMA_Instance)
static const I2C_DMA I2C2_RX_DMA = {
  &hdma_i2c2_rx,
  I2C2_RX_DMA_Complete,
  I2C2_RX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C2_RX_DMA_Instance,
  MX_I2C2_RX_DMA_IRQn,
  MX_I2C2_RX_DMA_Channel,
  MX_I2C2_RX_DMA_Priority
  #endif
};
#endif

/* I2C2 Tx DMA */
#if defined(MX_I2C2_TX_DMA_Instance)
static const I2C_DMA I2C2_TX_DMA = {
  &hdma_i2c2_tx,
  I2C2_TX_DMA_Complete,
  I2C2_TX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C2_TX_DMA_Instance,
  MX_I2C2_TX_DMA_IRQn,
  MX_I2C2_TX_DMA_Channel,
  MX_I2C2_TX_DMA_Priority
  #endif
};
#endif

/* I2C2 Information (Run-Time) */
static I2C_INFO I2C2_Info;

/* I2C2 Resources */
static I2C_RESOURCES I2C2_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hi2c2,
#endif
  I2C2,
#if defined(MX_I2C2_RX_DMA_Instance)
  &I2C2_RX_DMA,
#else
  NULL,
#endif
#if defined(MX_I2C2_TX_DMA_Instance)
  &I2C2_TX_DMA,
#else
  NULL,
#endif
  {
    MX_I2C2_SCL_GPIOx,
    MX_I2C2_SDA_GPIOx,
    MX_I2C2_SCL_GPIO_Pin,
    MX_I2C2_SDA_GPIO_Pin,
    MX_I2C2_SCL_GPIO_PuPdOD,
    MX_I2C2_SDA_GPIO_PuPdOD,
    MX_I2C2_SCL_GPIO_AF,
    MX_I2C2_SDA_GPIO_AF
  },
  I2C2_EV_IRQn,
  I2C2_ER_IRQn,
  &I2C2_Info
};

#endif /* MX_I2C2 */

#if defined(MX_I2C3)
/* Function prototypes */
void I2C3_EV_IRQHandler (void);
void I2C3_ER_IRQHandler (void);

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern I2C_HandleTypeDef hi2c3;
#endif

/* I2C3 Rx DMA */
#if defined(MX_I2C3_RX_DMA_Instance)
static const I2C_DMA I2C3_RX_DMA = {
  &hdma_i2c3_rx,
  I2C3_RX_DMA_Complete,
  I2C3_RX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C3_RX_DMA_Instance,
  MX_I2C3_RX_DMA_IRQn,
  MX_I2C3_RX_DMA_Channel,
  MX_I2C3_RX_DMA_Priority
  #endif
};
#endif

/* I2C3 Tx DMA */
#if defined(MX_I2C3_TX_DMA_Instance)
static const I2C_DMA I2C3_TX_DMA = {
  &hdma_i2c3_tx,
  I2C3_TX_DMA_Complete,
  I2C3_TX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C3_TX_DMA_Instance,
  MX_I2C3_TX_DMA_IRQn,
  MX_I2C3_TX_DMA_Channel,
  MX_I2C3_TX_DMA_Priority
  #endif
};
#endif

/* I2C3 Information (Run-Time) */
static I2C_INFO I2C3_Info;

/* I2C3 Resources */
static I2C_RESOURCES I2C3_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hi2c3,
#endif
  I2C3,
#if defined(MX_I2C3_RX_DMA_Instance)
  &I2C3_RX_DMA,
#else
  NULL,
#endif
#if defined(MX_I2C3_TX_DMA_Instance)
  &I2C3_TX_DMA,
#else
  NULL,
#endif
  {
    MX_I2C3_SCL_GPIOx,
    MX_I2C3_SDA_GPIOx,
    MX_I2C3_SCL_GPIO_Pin,
    MX_I2C3_SDA_GPIO_Pin,
    MX_I2C3_SCL_GPIO_PuPdOD,
    MX_I2C3_SDA_GPIO_PuPdOD,
    MX_I2C3_SCL_GPIO_AF,
    MX_I2C3_SDA_GPIO_AF
  },
  I2C3_EV_IRQn,
  I2C3_ER_IRQn,
  &I2C3_Info
};

#endif /* MX_I2C3 */

#if defined(MX_I2C4)
/* Function prototypes */
void I2C4_EV_IRQHandler (void);
void I2C4_ER_IRQHandler (void);

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern I2C_HandleTypeDef hi2c4;
#endif

/* I2C4 Rx DMA */
#if defined(MX_I2C4_RX_DMA_Instance)
static const I2C_DMA I2C4_RX_DMA = {
  &hdma_i2c4_rx,
  &I2C4_RX_DMA_Complete,
  &I2C4_RX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C4_RX_DMA_Instance,
  MX_I2C4_RX_DMA_IRQn,
  MX_I2C4_RX_DMA_Channel,
  MX_I2C4_RX_DMA_Priority
  #endif
};
#endif

/* I2C4 Tx DMA */
#if defined(MX_I2C4_TX_DMA_Instance)
static const I2C_DMA I2C4_TX_DMA = {
  &hdma_i2c4_tx,
  &I2C4_TX_DMA_Complete,
  &I2C4_TX_DMA_Error,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_I2C4_TX_DMA_Instance,
  MX_I2C4_TX_DMA_IRQn,
  MX_I2C4_TX_DMA_Channel,
  MX_I2C4_TX_DMA_Priority
  #endif
};
#endif

/* I2C4 Information (Run-Time) */
static I2C_INFO I2C4_Info;

/* I2C4 Resources */
static I2C_RESOURCES I2C4_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hi2c4,
#endif
  I2C4,
#if defined(MX_I2C4_RX_DMA_Instance)
  &I2C4_RX_DMA,
#else
  NULL,
#endif
#if defined(MX_I2C4_TX_DMA_Instance)
  &I2C4_TX_DMA,
#else
  NULL,
#endif
  {
    MX_I2C4_SCL_GPIOx,
    MX_I2C4_SDA_GPIOx,
    MX_I2C4_SCL_GPIO_Pin,
    MX_I2C4_SDA_GPIO_Pin,
    MX_I2C4_SCL_GPIO_PuPdOD,
    MX_I2C4_SDA_GPIO_PuPdOD,
    MX_I2C4_SCL_GPIO_AF,
    MX_I2C4_SDA_GPIO_AF
  },
  I2C4_EV_IRQn,
  I2C4_ER_IRQn,
  &I2C4_Info
};

#endif /* MX_I2C4 */


#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/**
  \fn          void Enable_GPIO_Clock (const GPIO_TypeDef *port)
  \brief       Enable GPIO clock
*/
static void Enable_GPIO_Clock (const GPIO_TypeDef *GPIOx) {
  if      (GPIOx == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
  else if (GPIOx == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
  else if (GPIOx == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
  else if (GPIOx == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
  else if (GPIOx == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
#if defined(GPIOF)
  else if (GPIOx == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
#endif
#if defined(GPIOG)
  else if (GPIOx == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
#endif
#if defined(GPIOH)
  else if (GPIOx == GPIOH) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
#endif
#if defined(GPIOI)
  else if (GPIOx == GPIOI) { __HAL_RCC_GPIOI_CLK_ENABLE(); }
#endif
#if defined(GPIOJ)
  else if (GPIOx == GPIOJ) { __HAL_RCC_GPIOJ_CLK_ENABLE(); }
#endif
#if defined(GPIOK)
  else if (GPIOx == GPIOK) { __HAL_RCC_GPIOK_CLK_ENABLE(); }
#endif
}
#endif


/**
  \fn          ARM_DRV_VERSION I2C_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION I2CX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_I2C_CAPABILITIES I2C_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_I2C_CAPABILITIES
*/
static ARM_I2C_CAPABILITIES I2CX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t I2C_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event  Pointer to \ref ARM_I2C_SignalEvent
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static int32_t I2C_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c) {
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  GPIO_InitTypeDef GPIO_InitStruct;
#endif

  if (i2c->info->flags & I2C_INIT) { return ARM_DRIVER_OK; }

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* Setup I2C pin configuration */
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;

    /* Configure SCL Pin */
    Enable_GPIO_Clock (i2c->io.scl_port);

    GPIO_InitStruct.Pin       = i2c->io.scl_pin;
    GPIO_InitStruct.Pull      = i2c->io.scl_pull;
    GPIO_InitStruct.Alternate = i2c->io.scl_af;

    HAL_GPIO_Init (i2c->io.scl_port, &GPIO_InitStruct);

    /* Configure SDA Pin */
    Enable_GPIO_Clock (i2c->io.sda_port);

    GPIO_InitStruct.Pin       = i2c->io.sda_pin;
    GPIO_InitStruct.Pull      = i2c->io.sda_pull;
    GPIO_InitStruct.Alternate = i2c->io.sda_af;

    HAL_GPIO_Init (i2c->io.sda_port, &GPIO_InitStruct);

    if (i2c->dma_rx != NULL) {
      i2c->dma_rx->h->Instance = i2c->dma_rx->stream;

      /* DMA controller clock enable */
      __HAL_RCC_DMA1_CLK_ENABLE();

      /* Configure DMA receive stream */
      i2c->dma_rx->h->Init.Channel             = i2c->dma_rx->channel;
      i2c->dma_rx->h->Init.Direction           = DMA_PERIPH_TO_MEMORY;
      i2c->dma_rx->h->Init.PeriphInc           = DMA_PINC_DISABLE;
      i2c->dma_rx->h->Init.MemInc              = DMA_MINC_ENABLE;
      i2c->dma_rx->h->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      i2c->dma_rx->h->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      i2c->dma_rx->h->Init.Mode                = DMA_NORMAL;
      i2c->dma_rx->h->Init.Priority            = i2c->dma_rx->priority;
      i2c->dma_rx->h->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
      i2c->dma_rx->h->Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
      i2c->dma_rx->h->Init.MemBurst            = DMA_MBURST_SINGLE;
      i2c->dma_rx->h->Init.PeriphBurst         = DMA_PBURST_SINGLE;

      /* Configure stream */
      if (HAL_DMA_Init (i2c->dma_rx->h) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }
    }
    if (i2c->dma_tx != NULL) {
      i2c->dma_tx->h->Instance = i2c->dma_tx->stream;

      /* DMA controller clock enable */
      __HAL_RCC_DMA1_CLK_ENABLE();

      /* Configure DMA transmit stream */
      i2c->dma_tx->h->Init.Channel             = i2c->dma_tx->channel;
      i2c->dma_tx->h->Init.Direction           = DMA_MEMORY_TO_PERIPH;
      i2c->dma_tx->h->Init.PeriphInc           = DMA_PINC_DISABLE;
      i2c->dma_tx->h->Init.MemInc              = DMA_MINC_ENABLE;
      i2c->dma_tx->h->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      i2c->dma_tx->h->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      i2c->dma_tx->h->Init.Mode                = DMA_NORMAL;
      i2c->dma_tx->h->Init.Priority            = i2c->dma_tx->priority;
      i2c->dma_tx->h->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
      i2c->dma_tx->h->Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
      i2c->dma_tx->h->Init.MemBurst            = DMA_MBURST_SINGLE;
      i2c->dma_tx->h->Init.PeriphBurst         = DMA_PBURST_SINGLE;

      /* Configure stream */
      if (HAL_DMA_Init (i2c->dma_tx->h) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }
    }
  #else
    i2c->h->Instance = i2c->reg;
  #endif

  if (i2c->dma_rx != NULL) {
    i2c->dma_rx->h->XferCpltCallback  = i2c->dma_rx->cb_complete;
    i2c->dma_rx->h->XferErrorCallback = i2c->dma_rx->cb_error;
  }
  if (i2c->dma_tx != NULL) {
    i2c->dma_tx->h->XferCpltCallback  = i2c->dma_tx->cb_complete;
    i2c->dma_tx->h->XferErrorCallback = i2c->dma_tx->cb_error;
  }

  /* Reset Run-Time information structure */
  memset (i2c->info, 0x00, sizeof (I2C_INFO));

  i2c->info->cb_event = cb_event;
  i2c->info->flags    = I2C_INIT;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_Uninitialize (I2C_RESOURCES *i2c)
  \brief       De-initialize I2C Interface.
  \param[in]   i2c  Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_Uninitialize (I2C_RESOURCES *i2c) {

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* Unconfigure SCL and SDA Pins */
    HAL_GPIO_DeInit(i2c->io.scl_port, i2c->io.scl_pin);
    HAL_GPIO_DeInit(i2c->io.sda_port, i2c->io.sda_pin);

    if (i2c->dma_rx != NULL) { i2c->dma_rx->h->Instance = NULL; }
    if (i2c->dma_tx != NULL) { i2c->dma_tx->h->Instance = NULL; }
  #else
    i2c->h->Instance = NULL;
  #endif

  i2c->info->flags = 0U;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t ARM_I2C_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *i2c)
  \brief       Control I2C Interface Power.
  \param[in]   state  Power state
  \param[in]   i2c  Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *i2c) {
  uint32_t cr1;

  switch (state) {
    case ARM_POWER_OFF:
      /* Enable I2C clock */
      if      (i2c->reg == I2C1) { __HAL_RCC_I2C1_CLK_ENABLE(); }
      else if (i2c->reg == I2C2) { __HAL_RCC_I2C2_CLK_ENABLE(); }
      else if (i2c->reg == I2C3) { __HAL_RCC_I2C3_CLK_ENABLE(); }
      else /*(i2c->reg == I2C4)*/{ __HAL_RCC_I2C4_CLK_ENABLE(); }

      /* Disable I2C peripheral */
      i2c->reg->CR1 = 0;

      #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
        /* Disable I2C IRQ */
        HAL_NVIC_DisableIRQ(i2c->ev_irq_num);
        HAL_NVIC_DisableIRQ(i2c->er_irq_num);

        /* Abort DMA streams */
        if (i2c->dma_rx != NULL) {
          if (i2c->dma_rx->h->Instance != NULL) {
            HAL_DMA_Abort (i2c->dma_rx->h);
          }
        }
        if (i2c->dma_tx != NULL) {
          if (i2c->dma_tx->h->Instance != NULL) {
            HAL_DMA_Abort (i2c->dma_tx->h);
          }
        }

        /* Disable DMA stream IRQs in NVIC */
        if (i2c->dma_rx != NULL) {
          if (i2c->dma_rx->h->Instance != NULL) {
            HAL_NVIC_DisableIRQ (i2c->dma_rx->irq_num);
          }
        }
        if (i2c->dma_tx != NULL) {
          if (i2c->dma_tx->h->Instance != NULL) {
            HAL_NVIC_DisableIRQ (i2c->dma_tx->irq_num);
          }
        }

        /* Disable peripheral clock */
        if      (i2c->reg == I2C1) { __HAL_RCC_I2C1_CLK_DISABLE(); }
        else if (i2c->reg == I2C2) { __HAL_RCC_I2C2_CLK_DISABLE(); }
        else if (i2c->reg == I2C3) { __HAL_RCC_I2C3_CLK_DISABLE(); }
        else /*(i2c->reg == I2C4)*/{ __HAL_RCC_I2C4_CLK_DISABLE(); }
      #else
        if (i2c->h->Instance != NULL) { HAL_I2C_MspDeInit (i2c->h);}
      #endif

      i2c->info->status.busy             = 0U;
      i2c->info->status.mode             = 0U;
      i2c->info->status.direction        = 0U;
      i2c->info->status.general_call     = 0U;
      i2c->info->status.arbitration_lost = 0U;
      i2c->info->status.bus_error        = 0U;

      i2c->info->flags &= ~I2C_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((i2c->info->flags & I2C_INIT)  == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((i2c->info->flags & I2C_POWER) != 0U) {
        return ARM_DRIVER_OK;
      }

      #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
        /* Enable I2C clock */
        if      (i2c->reg == I2C1) { __HAL_RCC_I2C1_CLK_ENABLE(); }
        else if (i2c->reg == I2C2) { __HAL_RCC_I2C2_CLK_ENABLE(); }
        else if (i2c->reg == I2C3) { __HAL_RCC_I2C3_CLK_ENABLE(); }
        else /*(i2c->reg == I2C4)*/{ __HAL_RCC_I2C4_CLK_ENABLE(); }

        /* Enable DMA IRQs in NVIC */
        if (i2c->dma_rx != NULL) { HAL_NVIC_EnableIRQ (i2c->dma_rx->irq_num); }
        if (i2c->dma_tx != NULL) { HAL_NVIC_EnableIRQ (i2c->dma_tx->irq_num); }

        /* Clear and Enable I2C IRQ */
        HAL_NVIC_ClearPendingIRQ(i2c->ev_irq_num);
        HAL_NVIC_ClearPendingIRQ(i2c->er_irq_num);
        HAL_NVIC_EnableIRQ(i2c->ev_irq_num);
        HAL_NVIC_EnableIRQ(i2c->er_irq_num);
      #else
        HAL_I2C_MspInit (i2c->h);
      #endif

      /* Reset the peripheral */
      if (i2c->reg == I2C1) {
        __HAL_RCC_I2C1_FORCE_RESET();
        __NOP(); __NOP(); __NOP(); __NOP();
        __HAL_RCC_I2C1_RELEASE_RESET();
      }
      else if (i2c->reg == I2C2) {
        __HAL_RCC_I2C2_FORCE_RESET();
        __NOP(); __NOP(); __NOP(); __NOP();
        __HAL_RCC_I2C2_RELEASE_RESET();
      }
      else if (i2c->reg == I2C3) {
        __HAL_RCC_I2C3_FORCE_RESET();
        __NOP(); __NOP(); __NOP(); __NOP();
        __HAL_RCC_I2C3_RELEASE_RESET();
      }
      else /*(i2c->reg == I2C4)*/ {
        __HAL_RCC_I2C4_FORCE_RESET();
        __NOP(); __NOP(); __NOP(); __NOP();
        __HAL_RCC_I2C4_RELEASE_RESET();
      }

      /* Initial peripheral setup */
      cr1 = I2C_CR1_SBC    | /* Slave byte control enabled          */
            I2C_CR1_ERRIE  | /* Error interrupts enabled            */
            I2C_CR1_TCIE   | /* Transfer complete interrupt enabled */
            I2C_CR1_STOPIE | /* STOP detection interrupt enabled    */
            I2C_CR1_NACKIE | /* NACK interrupt enabled              */
            I2C_CR1_ADDRIE ; /* Address match interrupt enabled     */

      /* Enable IRQ/DMA rx/tx requests */
      if (i2c->dma_rx == NULL) {cr1 |= I2C_CR1_RXIE;    }
      else                     {cr1 |= I2C_CR1_RXDMAEN; }
      if (i2c->dma_tx == NULL) {cr1 |= I2C_CR1_TXIE;    }
      else                     {cr1 |= I2C_CR1_TXDMAEN; }

      /* Apply setup and enable peripheral */
      i2c->reg->CR1 = cr1 | I2C_CR1_PE;

      /* Ready for operation */
      i2c->info->flags |= I2C_POWER;
      break;
  }

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_MasterTransmit (uint32_t       addr,
                                           const uint8_t *data,
                                           uint32_t       num,
                                           bool           xfer_pending,
                                           I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[in]   data          Pointer to buffer with data to send to I2C Slave
  \param[in]   num           Number of data bytes to send
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterTransmit (uint32_t       addr,
                                   const uint8_t *data,
                                   uint32_t       num,
                                   bool           xfer_pending,
                                   I2C_RESOURCES *i2c) {
  uint32_t cr2, cnt;
  bool restart;

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((addr & ~(ARM_I2C_ADDRESS_10BIT | ARM_I2C_ADDRESS_GC)) > 0x3FFU) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  restart = (i2c->info->xfer.ctrl & XFER_CTRL_RESTART) != 0U;

  if (!restart) {
    /* New transfer */
    while (i2c->reg->ISR & I2C_ISR_BUSY) {
      ; /* Wait until bus released */
    }
  }

  i2c->info->status.busy             = 1U;
  i2c->info->status.mode             = 1U;
  i2c->info->status.direction        = 0U;
  i2c->info->status.bus_error        = 0U;
  i2c->info->status.arbitration_lost = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0;
  i2c->info->xfer.data = (uint8_t *)data;
  i2c->info->xfer.ctrl = 0U;

  if (xfer_pending) {
    i2c->info->xfer.ctrl = XFER_CTRL_RESTART;
  }

  /* Set slave address and transfer direction */
  if ((addr & ARM_I2C_ADDRESS_10BIT) != 0) {
    cr2 = (addr & 0x3FF) | I2C_CR2_ADD10;
  }
  else {
    cr2 = (addr & 0x7F) << 1;
  }

  /* Set number of bytes to transfer */
  if (num < 256) {
    cnt = num;
    /* Send all data bytes and enable automatic STOP */
    if (!xfer_pending && !restart) {
      cr2 |= I2C_CR2_AUTOEND;
    }
  }
  else {
    cnt = 255;
    /* Send 255 data bytes and enable NBYTES reload */
    cr2 |= I2C_CR2_RELOAD;
  }

  /* Apply transfer setup */
  i2c->reg->CR2 = (cnt << 16) | cr2;

  if (i2c->dma_tx) {
    /* Enable stream */
    if (HAL_DMA_Start_IT (i2c->dma_tx->h, (uint32_t)data, (uint32_t)&(i2c->reg->TXDR), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }

  /* Generate start */
  i2c->reg->CR2 |= I2C_CR2_START;
  /* Enable transfer complete interrupt */
  i2c->reg->CR1 |= I2C_CR1_TCIE;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_MasterReceive (uint32_t       addr,
                                          uint8_t       *data,
                                          uint32_t       num,
                                          bool           xfer_pending,
                                          I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[out]  data          Pointer to buffer for data to receive from I2C Slave
  \param[in]   num           Number of data bytes to receive
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterReceive (uint32_t       addr,
                                  uint8_t       *data,
                                  uint32_t       num,
                                  bool           xfer_pending,
                                  I2C_RESOURCES *i2c) {
  uint32_t cr2, cnt;
  bool restart;

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((addr & ~(ARM_I2C_ADDRESS_10BIT | ARM_I2C_ADDRESS_GC)) > 0x3FFU) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  restart = (i2c->info->xfer.ctrl & XFER_CTRL_RESTART) != 0U;

  if (!restart) {
    /* New transfer */
    while (i2c->reg->ISR & I2C_ISR_BUSY) {
      ; /* Wait until bus released */
    }
  }

  i2c->info->status.busy             = 1U;
  i2c->info->status.mode             = 1U;
  i2c->info->status.direction        = 1U;
  i2c->info->status.bus_error        = 0U;
  i2c->info->status.arbitration_lost = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0;
  i2c->info->xfer.data = (uint8_t *)data;
  i2c->info->xfer.ctrl = 0U;

  if (xfer_pending) {
    i2c->info->xfer.ctrl = XFER_CTRL_RESTART;
  }

  /* Set slave address and transfer direction */
  if ((addr & ARM_I2C_ADDRESS_10BIT) != 0) {
    cr2 = (addr & 0x3FF) | I2C_CR2_ADD10 | I2C_CR2_RD_WRN;
  }
  else {
    cr2 = ((addr & 0x7F) << 1) | I2C_CR2_RD_WRN;
  }

  /* Set number of bytes to transfer */
  if (num < 256) {
    cnt = num;
    /* Send all data bytes and enable automatic STOP */
    if (!xfer_pending && !restart) {
      cr2 |= I2C_CR2_AUTOEND;
    }
  }
  else {
    cnt = 255;
    /* Send 255 data bytes and enable NBYTES reload */
    cr2 |= I2C_CR2_RELOAD;
  }

  /* Apply transfer setup */
  i2c->reg->CR2 = (cnt << 16) | cr2;

  if (i2c->dma_rx) {
    /* Enable stream */
    if (HAL_DMA_Start_IT (i2c->dma_rx->h, (uint32_t)&(i2c->reg->RXDR), (uint32_t)data, num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }

  /* Generate start */
  i2c->reg->CR2 |= I2C_CR2_START;
  /* Enable transfer complete interrupt */
  i2c->reg->CR1 |= I2C_CR1_TCIE;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_SlaveTransmit (const uint8_t *data, uint32_t num, I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Slave.
  \param[in]   data          Pointer to buffer with data to send to I2C Master
  \param[in]   num           Number of data bytes to send
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveTransmit (const uint8_t *data, uint32_t num, I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  i2c->info->status.bus_error    = 0U;
  i2c->info->status.general_call = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = -1;
  i2c->info->xfer.data = (uint8_t *)data;
  i2c->info->xfer.ctrl = 0U;

  /* Set number of bytes to transfer */
  if (num < 256) {
    i2c->reg->CR2 = (num << 16);
  }
  else {
    /* Send 255 data bytes and enable NBYTES reload */
    i2c->reg->CR2 = (255 << 16) | I2C_CR2_RELOAD;
  }

  if (i2c->dma_tx) {
    /* Enable stream */
    if (HAL_DMA_Start_IT (i2c->dma_tx->h, (uint32_t)data, (uint32_t)&(i2c->reg->TXDR), num) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_SlaveReceive (uint8_t *data, uint32_t num, I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Slave.
  \param[out]  data          Pointer to buffer for data to receive from I2C Master
  \param[in]   num           Number of data bytes to receive
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveReceive (uint8_t *data, uint32_t num, I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  i2c->info->status.bus_error    = 0U;
  i2c->info->status.general_call = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = -1;
  i2c->info->xfer.data = data;
  i2c->info->xfer.ctrl = 0U;

  /* Set number of bytes to transfer */
  if (num < 256) {
    i2c->reg->CR2 = (num << 16);
  }
  else {
    /* Send 255 data bytes and enable NBYTES reload */
    i2c->reg->CR2 = (255 << 16) | I2C_CR2_RELOAD;
  }

  if (i2c->dma_rx) {
    /* Enable stream */
    if (HAL_DMA_Start_IT (i2c->dma_rx->h, (uint32_t)&(i2c->reg->RXDR), (uint32_t)data, num) != HAL_OK) {
      return ARM_DRIVER_OK;
    }
  }

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_GetDataCount (void)
  \brief       Get transferred data count.
  \return      number of data bytes transferred; -1 when Slave is not addressed by Master
*/
static int32_t I2C_GetDataCount (I2C_RESOURCES *i2c) {
  return (i2c->info->xfer.cnt);
}


/**
  \fn          int32_t I2C_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c)
  \brief       Control I2C Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   i2c      pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c) {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_PinState state;
  uint32_t i, val;
  uint32_t fpclk, fscl, ferr;
  uint32_t presc, scl;
  uint32_t presc_ok, scl_ok;
  uint32_t err, err_min;

  if ((i2c->info->flags & I2C_POWER) == 0U) {
    /* I2C not powered */
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_I2C_OWN_ADDRESS:
      if (arg == 0) {
        /* Disable slave */
        i2c->reg->OAR1 = 0;
      }
      else {
        if ((arg & ARM_I2C_ADDRESS_GC) != 0) {
          /* Enable general call */
          i2c->reg->CR1 |=  I2C_CR1_GCEN;
        } else {
          /* Disable general call */
          i2c->reg->CR1 &= ~I2C_CR1_GCEN;
        }

        if ((arg & ARM_I2C_ADDRESS_10BIT) != 0) {
          val = (arg & 0x3FF ) | I2C_OAR1_OA1MODE;
        } else {
          val = (arg & 0x7F) << 1;
        }

        i2c->reg->OAR1 = val | I2C_OAR1_OA1EN;
      }
      break;

    case ARM_I2C_BUS_SPEED:
      fpclk = HAL_RCC_GetPCLK1Freq();
      switch (arg) {
        case ARM_I2C_BUS_SPEED_STANDARD: /* Clock = 100kHz */
          fscl = 200000;
          break;

        case ARM_I2C_BUS_SPEED_FAST: /* Clock = 400kHz */
          fscl = 800000;
          break;
        
        case ARM_I2C_BUS_SPEED_FAST_PLUS: /* Clock = 1MHz */
          fscl = 2000000;
          break;

        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      /* Determine prescalers */
      err_min = 0xFFFFFFFF;
      for (presc = 1; presc <= 16; presc++) {
        for (scl = 1; scl <= 256; scl++) {
          ferr = fscl * scl * presc;

          if (ferr <= fpclk) {
            err = fpclk - ferr;
            
            if (err < err_min) {
              err_min = err;
              presc_ok = presc;
              scl_ok   = scl;
            }
          }
        }
      }

      i2c->reg->CR1    &= ~I2C_CR1_PE;
      i2c->reg->TIMINGR = ((presc_ok-1) << 28) |
                          ((scl_ok -1)  <<  8) |
                           (scl_ok -1)         ;
      i2c->reg->CR1    |=  I2C_CR1_PE;
      break;

    case ARM_I2C_BUS_CLEAR:
      /* Configure SCl and SDA pins as GPIO pin */
      GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
      GPIO_InitStruct.Pull  = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;

      GPIO_InitStruct.Pin = i2c->io.scl_pin;
      HAL_GPIO_Init(i2c->io.scl_port, &GPIO_InitStruct);
      GPIO_InitStruct.Pin = i2c->io.sda_pin;
      HAL_GPIO_Init(i2c->io.sda_port, &GPIO_InitStruct);

      /* Pull SCL and SDA high */
      HAL_GPIO_WritePin (i2c->io.scl_port, i2c->io.scl_pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin (i2c->io.sda_port, i2c->io.sda_pin, GPIO_PIN_SET);

      HAL_Delay (I2C_BUS_CLEAR_CLOCK_PERIOD);

      for (i = 0U; i < 9U; i++) {
        if (HAL_GPIO_ReadPin (i2c->io.sda_port, i2c->io.sda_pin) == GPIO_PIN_SET) {
          /* Break if slave released SDA line */
          break;
        }
        /* Clock high */
        HAL_GPIO_WritePin (i2c->io.scl_port, i2c->io.scl_pin, GPIO_PIN_SET);
        HAL_Delay (I2C_BUS_CLEAR_CLOCK_PERIOD/2);

        /* Clock low */
        HAL_GPIO_WritePin (i2c->io.scl_port, i2c->io.scl_pin, GPIO_PIN_RESET);
        HAL_Delay (I2C_BUS_CLEAR_CLOCK_PERIOD/2);
      }

      /* Check SDA state */
      state = HAL_GPIO_ReadPin (i2c->io.sda_port, i2c->io.sda_pin);

      /* Configure SDA and SCL pins as I2C peripheral pins */
      GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;

      GPIO_InitStruct.Pin       = i2c->io.scl_pin;
      GPIO_InitStruct.Pull      = i2c->io.scl_pull;
      GPIO_InitStruct.Alternate = i2c->io.scl_af;

      HAL_GPIO_Init (i2c->io.scl_port, &GPIO_InitStruct);

      GPIO_InitStruct.Pin       = i2c->io.sda_pin;
      GPIO_InitStruct.Pull      = i2c->io.sda_pull;
      GPIO_InitStruct.Alternate = i2c->io.sda_af;

      HAL_GPIO_Init (i2c->io.sda_port, &GPIO_InitStruct);

      return (state == GPIO_PIN_SET) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;

    case ARM_I2C_ABORT_TRANSFER:
      /* Save CR1 register */
      val = i2c->reg->CR1;

      /* Disable DMA requests and peripheral interrupts */
      i2c->reg->CR1 &= ~(I2C_CR1_RXDMAEN |
                         I2C_CR1_TXDMAEN |
                         I2C_CR1_ERRIE   |
                         I2C_CR1_TCIE    |
                         I2C_CR1_STOPIE  |
                         I2C_CR1_NACKIE  |
                         I2C_CR1_ADDRIE  |
                         I2C_CR1_RXIE    |
                         I2C_CR1_TXIE)   ;

      if (i2c->info->status.mode != 0U) {
        /* Master generates stop after the current byte transfer */
        i2c->reg->CR2 |= I2C_CR2_STOP;
      }
      else {
        /* Slave receiver will send NACK */
        i2c->reg->CR2 |= I2C_CR2_NACK;
      }

      if (i2c->dma_rx != NULL) {
        /* Disable DMA Streams */
        if (HAL_DMA_Abort (i2c->dma_rx->h) != HAL_OK) {
          return ARM_DRIVER_ERROR;
        }
      }
      if (i2c->dma_tx != NULL) {
        if (HAL_DMA_Abort (i2c->dma_tx->h) != HAL_OK) {
          return ARM_DRIVER_ERROR;
        }
      }

      i2c->info->xfer.num  = 0U;
      i2c->info->xfer.cnt  = 0U;
      i2c->info->xfer.data = NULL;
      i2c->info->xfer.ctrl = 0U;

      i2c->info->status.busy             = 0U;
      i2c->info->status.mode             = 0U;
      i2c->info->status.direction        = 0U;
      i2c->info->status.general_call     = 0U;
      i2c->info->status.arbitration_lost = 0U;
      i2c->info->status.bus_error        = 0U;

      /* Disable peripheral (I2C lines will be released */
      i2c->reg->CR1 &= ~I2C_CR1_PE;

      /* Clear pending interrupts */
      i2c->reg->ICR = I2C_ICR_OVRCF  |
                      I2C_ICR_ARLOCF |
                      I2C_ICR_BERRCF |
                      I2C_ICR_STOPCF |
                      I2C_ICR_NACKCF |
                      I2C_ICR_ADDRCF ;

      /* Restore settings and enable peripheral */
      i2c->reg->CR1 = val;
      break;

    default: return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}


/**
  \fn          ARM_I2C_STATUS I2C_GetStatus (I2C_RESOURCES *i2c)
  \brief       Get I2C status.
  \param[in]   i2c      pointer to I2C resources
  \return      I2C status \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_GetStatus (I2C_RESOURCES *i2c) {
  return (i2c->info->status);
}


/**
  \fn          void I2C_EV_IRQHandler (I2C_RESOURCES *i2c)
  \brief       I2C Event Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_EV_IRQHandler (I2C_RESOURCES *i2c) {
  I2C_TRANSFER_INFO *tr = &i2c->info->xfer;
  ARM_I2C_STATUS    *st = &i2c->info->status;

  uint32_t event;
  uint32_t isr, icr;
  uint32_t cnt, cr;

  event = 0U;
  icr   = 0U;
  isr   = i2c->reg->ISR;

  if (isr & I2C_ISR_RXNE) {
    /* Receive data register not empty */
    tr->data[tr->cnt++] = i2c->reg->RXDR;
  }
  else if (isr & I2C_ISR_TXIS) {
    /* Transmit data register empty */
    i2c->reg->TXDR = tr->data[tr->cnt++];
  }
  else if ((isr & I2C_ISR_TC) != 0U) {
    /* Transfer Complete */
    st->busy  = 0U;

    if ((tr->ctrl & XFER_CTRL_RESTART) != 0U) {
      /* Wait for pending transfer */
      i2c->reg->CR1 &= ~I2C_CR1_TCIE;

      event = ARM_I2C_EVENT_TRANSFER_DONE;
    }
    else {
      /* Send stop */
      i2c->reg->CR2 |= I2C_CR2_STOP;
    }
  }
  else if (isr & (I2C_ISR_STOPF | I2C_ISR_NACKF | I2C_ISR_ADDR)) {
    if (isr & I2C_ISR_STOPF) {
      /* Stop detection flag */
      icr |= I2C_ICR_STOPCF;

      if (tr->ctrl & XFER_CTRL_ADDR_NACK) {
        /* Slave address not acknowledged */
        event = ARM_I2C_EVENT_TRANSFER_DONE | ARM_I2C_EVENT_ADDRESS_NACK;
      }
      else {
        event = ARM_I2C_EVENT_TRANSFER_DONE;

        if (tr->cnt < tr->num) {
          event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
        }
        if (st->general_call) {
          event |= ARM_I2C_EVENT_GENERAL_CALL;
        }
      }

      tr->data = NULL;
      tr->ctrl = 0U;

      st->busy = 0U;
      st->mode = 0U;
    }
    else if (isr & I2C_ISR_ADDR) {
      /* Address matched (slave mode) */
      icr |= I2C_ICR_ADDRCF;

      st->mode = 0U;

      if ((isr & I2C_ISR_ADDCODE) == 0) {
        /* General call */
        st->general_call = 1U;

        event = ARM_I2C_EVENT_GENERAL_CALL;
      }

      /* Set transfer direction */
      if ((isr & I2C_ISR_DIR) == 0U) {
        st->direction = 1U; /* Slave enters receiver mode */

        event |= ARM_I2C_EVENT_SLAVE_RECEIVE;
      }
      else {
        st->direction = 0U; /* Slave enters transmitter mode */

        event |= ARM_I2C_EVENT_SLAVE_TRANSMIT;
      }

      if ((tr->data == NULL) || (st->general_call != 0U)) {
        if (i2c->info->cb_event != NULL) {
          i2c->info->cb_event (event);
        }
        event = 0U;
      }

      st->busy  = 1U;
      tr->cnt   = 0;
      tr->ctrl |= XFER_CTRL_ADDR_DONE;
    }
    else {
      if (isr & I2C_ISR_NACKF) {
        /* NACK received */
        icr |= I2C_ICR_NACKCF;

        /* Master sends STOP after slave NACK */
        /* Slave already released the lines   */
        if (st->mode != 0U) {
          i2c->reg->CR2 |= I2C_CR2_STOP;

          if (tr->cnt == 0U) {
            tr->ctrl |= XFER_CTRL_ADDR_NACK;
          }
        }
      }
    }
  }
  else {
    if ((isr & I2C_ISR_TCR) != 0) {
      /* Transfer Complete Reload */
      cr = i2c->reg->CR2;

      tr->cnt += (cr >> 16) & 0xFF;

      cr &= ~(I2C_CR2_RELOAD | I2C_CR2_NBYTES);

      cnt = tr->num - tr->cnt;

      if (cnt < 256) {
        if ((tr->ctrl & XFER_CTRL_RESTART) == 0) {
          cr |= I2C_CR2_AUTOEND;
        }
      }
      else {
        cnt = 255;
        cr |= I2C_CR2_RELOAD;
      }

      i2c->reg->CR2 = (cnt << 16) | cr;
    }
  }
  i2c->reg->ICR = icr;

  if ((event != 0U) && (i2c->info->cb_event != NULL)) {
    i2c->info->cb_event (event);
  }
}


/**
  \fn          void I2C_ER_IRQHandler (I2C_RESOURCES *i2c)
  \brief       I2C Error Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_ER_IRQHandler (I2C_RESOURCES *i2c) {
  I2C_TRANSFER_INFO *tr = &i2c->info->xfer;
  ARM_I2C_STATUS    *st = &i2c->info->status;
  uint32_t event, isr, icr;

  event = 0U;
  icr   = 0U;
  isr   = i2c->reg->ISR;

  if (isr & I2C_ISR_ARLO) {
    /* Arbitration lost */
    icr |= I2C_ICR_ARLOCF;

    /* Switch to slave mode */
    st->busy             = 0U;
    st->mode             = 0U;
    st->arbitration_lost = 1U;

    tr->data = NULL;
    tr->ctrl = 0U;

    event = ARM_I2C_EVENT_TRANSFER_DONE | ARM_I2C_EVENT_ARBITRATION_LOST;
  }
  else {
    if (isr & I2C_ISR_BERR) {
      /* Bus error (misplaced start/stop) */
      icr |= I2C_ICR_BERRCF;

      st->bus_error = 1U;

      if (st->mode == 0U) {
        /* Lines are released in slave mode */
        st->busy = 0U;

        tr->data = NULL;
        tr->ctrl = 0U;
      }

      event = ARM_I2C_EVENT_TRANSFER_DONE | ARM_I2C_EVENT_BUS_ERROR;
    }
  }
  /* Clear interrupt flags */
  i2c->reg->ICR = icr;

  if ((event != 0U) && (i2c->info->cb_event != NULL)) {
    i2c->info->cb_event (event);
  }
}


#if (defined(MX_I2C1_TX_DMA_Instance) || \
     defined(MX_I2C2_TX_DMA_Instance) || \
     defined(MX_I2C3_TX_DMA_Instance) || \
     defined(MX_I2C4_TX_DMA_Instance))
/**
  \fn          void I2C_DMA_TxEvent (uint32_t event, I2C_RESOURCES *i2c)
  \brief       I2C DMA Transmit Event handler
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_DMA_TxEvent (uint32_t event, I2C_RESOURCES *i2c) {

  if (event == DMA_COMPLETED) {
    i2c->info->xfer.cnt = i2c->info->xfer.num - __HAL_DMA_GET_COUNTER(i2c->dma_tx->h);
  }
}
#endif


#if (defined(MX_I2C1_RX_DMA_Instance) || \
     defined(MX_I2C2_RX_DMA_Instance) || \
     defined(MX_I2C3_RX_DMA_Instance) || \
     defined(MX_I2C4_RX_DMA_Instance))
/**
  \fn          void I2C_DMA_RxEvent (uint32_t event, I2C_RESOURCES *i2c)
  \brief       I2C DMA Receive Event handler
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_DMA_RxEvent (uint32_t event, I2C_RESOURCES *i2c) {

  if (event == DMA_COMPLETED) {
    i2c->info->xfer.cnt = i2c->info->xfer.num - __HAL_DMA_GET_COUNTER(i2c->dma_rx->h);
  }
}
#endif


#if defined(MX_I2C1)
/* I2C1 Driver wrapper functions */
static int32_t I2C1_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C1_Resources);
}
static int32_t I2C1_Uninitialize (void) {
  return I2C_Uninitialize(&I2C1_Resources);
}
static int32_t I2C1_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C1_Resources);
}
static int32_t I2C1_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C1_Resources);
}
static int32_t I2C1_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C1_Resources);
}
static int32_t I2C1_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C1_Resources);
}
static int32_t I2C1_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C1_Resources);
}
static int32_t I2C1_GetDataCount (void) {
  return I2C_GetDataCount(&I2C1_Resources);
}
static int32_t I2C1_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C1_Resources);
}
static ARM_I2C_STATUS I2C1_GetStatus (void) {
  return I2C_GetStatus(&I2C1_Resources);
}
void I2C1_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C1_Resources);
}
void I2C1_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C1_Resources);
}

#if defined(MX_I2C1_RX_DMA_Instance) && defined(MX_I2C1_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C1_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C1_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}
#endif
void I2C1_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_COMPLETED, &I2C1_Resources);
}
void I2C1_RX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_ERROR, &I2C1_Resources);
}
#endif

#if defined(MX_I2C1_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C1_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C1_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}
#endif
void I2C1_TX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_COMPLETED, &I2C1_Resources);
}
void I2C1_TX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_ERROR, &I2C1_Resources);
}
#endif

/* I2C1 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C1 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C1_Initialize,
  I2C1_Uninitialize,
  I2C1_PowerControl,
  I2C1_MasterTransmit,
  I2C1_MasterReceive,
  I2C1_SlaveTransmit,
  I2C1_SlaveReceive,
  I2C1_GetDataCount,
  I2C1_Control,
  I2C1_GetStatus
};
#endif /* MX_I2C1 */


#if defined(MX_I2C2)
/* I2C2 Driver wrapper functions */
static int32_t I2C2_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C2_Resources);
}
static int32_t I2C2_Uninitialize (void) {
  return I2C_Uninitialize(&I2C2_Resources);
}
static int32_t I2C2_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C2_Resources);
}
static int32_t I2C2_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C2_Resources);
}
static int32_t I2C2_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C2_Resources);
}
static int32_t I2C2_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C2_Resources);
}
static int32_t I2C2_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C2_Resources);
}
static int32_t I2C2_GetDataCount (void) {
  return I2C_GetDataCount(&I2C2_Resources);
}
static int32_t I2C2_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C2_Resources);
}
static ARM_I2C_STATUS I2C2_GetStatus (void) {
  return I2C_GetStatus(&I2C2_Resources);
}
void I2C2_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C2_Resources);
}
void I2C2_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C2_Resources);
}

#if defined(MX_I2C2_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C2_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C2_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c2_rx);
}
#endif
void I2C2_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_COMPLETED, &I2C2_Resources);
}
void I2C2_RX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_ERROR, &I2C2_Resources);
}
#endif

#if defined(MX_I2C2_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C2_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C2_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c2_tx);
}
#endif
void I2C2_TX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_COMPLETED, &I2C2_Resources);
}
void I2C2_TX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_ERROR, &I2C2_Resources);
}
#endif


/* I2C2 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C2 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C2_Initialize,
  I2C2_Uninitialize,
  I2C2_PowerControl,
  I2C2_MasterTransmit,
  I2C2_MasterReceive,
  I2C2_SlaveTransmit,
  I2C2_SlaveReceive,
  I2C2_GetDataCount,
  I2C2_Control,
  I2C2_GetStatus
};
#endif /* MX_I2C2 */


#if defined(MX_I2C3)
/* I2C3 Driver wrapper functions */
static int32_t I2C3_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C3_Resources);
}
static int32_t I2C3_Uninitialize (void) {
  return I2C_Uninitialize(&I2C3_Resources);
}
static int32_t I2C3_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C3_Resources);
}
static int32_t I2C3_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C3_Resources);
}
static int32_t I2C3_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C3_Resources);
}
static int32_t I2C3_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C3_Resources);
}
static int32_t I2C3_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C3_Resources);
}
static int32_t I2C3_GetDataCount (void) {
  return I2C_GetDataCount(&I2C3_Resources);
}
static int32_t I2C3_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C3_Resources);
}
static ARM_I2C_STATUS I2C3_GetStatus (void) {
  return I2C_GetStatus(&I2C3_Resources);
}
void I2C3_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C3_Resources);
}
void I2C3_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C3_Resources);
}

#if defined(MX_I2C3_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C3_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C3_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c3_rx);
}
#endif
void I2C3_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_COMPLETED, &I2C3_Resources);
}
void I2C3_RX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_ERROR, &I2C3_Resources);
}
#endif

#if defined(MX_I2C3_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C3_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C3_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c3_tx);
}
#endif
void I2C3_TX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_COMPLETED, &I2C3_Resources);
}
void I2C3_TX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_ERROR, &I2C3_Resources);
}
#endif


/* I2C3 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C3 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C3_Initialize,
  I2C3_Uninitialize,
  I2C3_PowerControl,
  I2C3_MasterTransmit,
  I2C3_MasterReceive,
  I2C3_SlaveTransmit,
  I2C3_SlaveReceive,
  I2C3_GetDataCount,
  I2C3_Control,
  I2C3_GetStatus
};
#endif /* MX_I2C3 */


#if defined(MX_I2C4)
/* I2C4 Driver wrapper functions */
static int32_t I2C4_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C4_Resources);
}
static int32_t I2C4_Uninitialize (void) {
  return I2C_Uninitialize(&I2C4_Resources);
}
static int32_t I2C4_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C4_Resources);
}
static int32_t I2C4_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C4_Resources);
}
static int32_t I2C4_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C4_Resources);
}
static int32_t I2C4_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C4_Resources);
}
static int32_t I2C4_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C4_Resources);
}
static int32_t I2C4_GetDataCount (void) {
  return I2C_GetDataCount(&I2C4_Resources);
}
static int32_t I2C4_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C4_Resources);
}
static ARM_I2C_STATUS I2C4_GetStatus (void) {
  return I2C_GetStatus(&I2C4_Resources);
}
void I2C4_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C4_Resources);
}
void I2C4_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C4_Resources);
}

#if defined(MX_I2C4_RX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C4_RX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C4_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c4_rx);
}
#endif
void I2C4_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_COMPLETED, &I2C4_Resources);
}
void I2C4_RX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_RxEvent (DMA_ERROR, &I2C4_Resources);
}
#endif

#if defined(MX_I2C4_TX_DMA_Instance)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
void I2C4_TX_DMA_Handler (void) {
  HAL_NVIC_ClearPendingIRQ(MX_I2C4_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_i2c4_tx);
}
#endif
void I2C4_TX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_COMPLETED, &I2C4_Resources);
}
void I2C4_TX_DMA_Error(DMA_HandleTypeDef *hdma) {
  I2C_DMA_TxEvent (DMA_ERROR, &I2C4_Resources);
}
#endif

/* I2C4 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C4 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C4_Initialize,
  I2C4_Uninitialize,
  I2C4_PowerControl,
  I2C4_MasterTransmit,
  I2C4_MasterReceive,
  I2C4_SlaveTransmit,
  I2C4_SlaveReceive,
  I2C4_GetDataCount,
  I2C4_Control,
  I2C4_GetStatus
};
#endif /* MX_I2C4 */

/*! \endcond */
