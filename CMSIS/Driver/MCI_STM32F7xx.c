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
 * Driver:       Driver_MCI0
 * Configured:   via RTE_Device.h configuration file
 * Project:      MCI Driver for ST STM32F7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value
 *   ---------------------                 -----
 *   Connect to hardware via Driver_MCI# = 0
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.1
 *    Enhanced STM32CubeMx compatibility
 *  Version 1.0
 *    Initial release
 */

/*! \page stm32f7_mci CMSIS-Driver MCI Setup 

The CMSIS-Driver MCI requires:
  - Setup of SDMMC1 with DMA for Rx and Tx DMA transfers.
  - Optional Configuration for Card Detect Pin:
    - Configure arbitrary pin in GPIO_Input mode and add User Label: MemoryCard_CD
  - Optional Configuration for Write Protect Pin:
    - Configure arbitrary pin in GPIO_Input mode and add User Label: MemoryCard_WP

\note The User Label name is used to connect the CMSIS-Driver to the GPIO pin.

The example below uses correct settings for STM32F746G-Discovery:  
  - SDMMC1 Mode:             SD 4bits Wide bus 
  - Card Detect Input pin:   PC13
  - Write Protect Input pin: not available

For different boards, refer to the hardware schematics to reflect correct setup values.

The STM32CubeMX configuration steps for Pinout, Clock, and System Configuration are 
listed below. Enter the values that are marked \b bold.
   
Pinout tab
----------
  1. Configure SDMMC1 mode
     - Peripherals \b SDMMC1: Mode=<b>SD 4bits Wide bus</b>
  2. Configure Card Detect pin:
     - Click in chip diagram on pin \b PC13 and select \b GPIO_Input. 
          
Clock Configuration tab
-----------------------
  1. Configure SDMMC Clock: "To SDMMC (MHz)": 48
  
Configuration tab
-----------------
  1. Under Connectivity open \b SDMMC1 Configuration:
     - <b>DMA Settings</b>: setup DMA transfers for Rx and Tx\n
       \b Add - Select \b SDMMC1_RX: Stream=DMA2 Stream 3, Direction=Peripheral to Memory, Priority=Low
          DMA Request Settings                  | Label             | Peripheral  | Memory
          :-------------------------------------|:------------------|:------------|:-------------
          Mode: <b>Peripheral Flow Control</b>  | Increment Address | off         |\b ON
          Use Fifo \b ON  Threshold: Full       | Data Width        |\b WORD      |\b WORD
          .                                     | Burst Size        |\b 4 Increm..|\b 4 Increm..
       \b Add - Select \b SDMMC1_TX: Stream=DMA2 Stream 6, Direction=Memory to Peripheral, Priority=Low
          DMA Request Settings                  | Label             | Peripheral  | Memory
          :-------------------------------------|:------------------|:------------|:-------------
          Mode: <b>Peripheral Flow Control</b>  | Increment Address | off         |\b ON
          Use Fifo \b ON  Threshold: Full       | Data Width        |\b WORD      |\b WORD
          .                                     | Burst Size        |\b 4 Increm..|\b 4 Increm..

     - <b>GPIO Settings</b>: review settings, no changes required
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PC8      | SDMMC1_D0     | Alternate | No pull-up and no..| High        |.
          PC9      | SDMMC1_D1     | Alternate | No pull-up and no..| High        |.
          PC10     | SDMMC1_D2     | Alternate | No pull-up and no..| High        |.
          PC11     | SDMMC1_D3     | Alternate | No pull-up and no..| High        |.
          PC12     | SDMMC1_CK     | Alternate | No pull-up and no..| High        |.
          PD2      | SDMMC1_CMD    | Alternate | No pull-up and no..| High        |.

     - <b>NVIC Settings</b>: enable interrupts
          Interrupt Table                      | Enable | Preemption Priority | Sub Priority
          :------------------------------------|:-------|:--------------------|:--------------
          SDMMC1 global interrupt              |\b ON   | 0                   | 0
          DMA2 stream3 global interrupt        |   ON   | 0                   | 0
          DMA2 stream6 global interrupt        |   ON   | 0                   | 0

     - Parameter Settings: not used
     - User Constants: not used

     Click \b OK to close the SDMMC1 Configuration dialog
  2. Under System open \b GPIO Pin Configuration
     - Enter user label for Card Detect pin
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PC13     | n/a           | Input mode| No pull-up and no..| n/a         |\b MemoryCard_CD

     Click \b OK to close the Pin Configuration dialog
*/

/*! \cond */

#include "MCI_STM32F7xx.h"

#define ARM_MCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,1)  /* driver version */

/* Define Card Detect pin active state */
#if !defined(MemoryCard_CD_Pin_Active)
  #define MemoryCard_CD_Pin_Active GPIO_PIN_RESET
#endif

/* Define Write Protect pin active state */
#if !defined(MemoryCard_WP_Pin_Active)
  #define MemoryCard_WP_Pin_Active GPIO_PIN_SET
#endif

/* Disable High Speed bus mode */
#if defined(MemoryCard_Bus_Mode_HS_Disable)
  #define MCI_BUS_MODE_HS    0U
#else
  #define MCI_BUS_MODE_HS    1U
#endif

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern SD_HandleTypeDef hsd1;
#endif

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern
#endif
DMA_HandleTypeDef hdma_sdmmc1_rx;

#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
extern
#endif
DMA_HandleTypeDef hdma_sdmmc1_tx;

static MCI_INFO MCI;

/* DMA callback function */
void RX_DMA_Complete(struct __DMA_HandleTypeDef *hdma);
/* IRQ Handler prototype */
void SDMMC1_IRQHandler (void);


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_MCI_API_VERSION,
  ARM_MCI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_MCI_CAPABILITIES DriverCapabilities = {
  MCI_CD_PIN,                                     /* cd_state          */
  0U,                                             /* cd_event          */
  MCI_WP_PIN,                                     /* wp_state          */
  0U,                                             /* vdd               */
  0U,                                             /* vdd_1v8           */
  0U,                                             /* vccq              */
  0U,                                             /* vccq_1v8          */
  0U,                                             /* vccq_1v2          */
  MCI_BUS_WIDTH_4,                                /* data_width_4      */
  MCI_BUS_WIDTH_8,                                /* data_width_8      */
  0U,                                             /* data_width_4_ddr  */
  0U,                                             /* data_width_8_ddr  */
  MCI_BUS_MODE_HS,                                /* high_speed        */
  0U,                                             /* uhs_signaling     */
  0U,                                             /* uhs_tuning        */
  0U,                                             /* uhs_sdr50         */
  0U,                                             /* uhs_sdr104        */
  0U,                                             /* uhs_ddr50         */
  0U,                                             /* uhs_driver_type_a */
  0U,                                             /* uhs_driver_type_c */
  0U,                                             /* uhs_driver_type_d */
  1U,                                             /* sdio_interrupt    */
  1U,                                             /* read_wait         */
  0U,                                             /* suspend_resume    */
  0U,                                             /* mmc_interrupt     */
  0U,                                             /* mmc_boot          */
  0U,                                             /* rst_n             */
  0U,                                             /* ccs               */
  0U                                              /* ccs_timeout       */
};


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
  \fn          ARM_DRV_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_MCI_CAPABILITIES MCI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_MCI_CAPABILITIES
*/
static ARM_MCI_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn            int32_t Initialize (ARM_MCI_SignalEvent_t cb_event)
  \brief         Initialize the Memory Card Interface
  \param[in]     cb_event  Pointer to \ref ARM_MCI_SignalEvent
  \return        \ref execution_status
*/
static int32_t Initialize (ARM_MCI_SignalEvent_t cb_event) {
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  GPIO_InitTypeDef GPIO_InitStruct;
#endif

  if (MCI.flags & MCI_INIT) { return ARM_DRIVER_OK; }

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* GPIO Ports Clock Enable */
    Enable_GPIO_Clock (GPIOC);
    Enable_GPIO_Clock (GPIOD);

    /* Configure CMD, CK and D0 pins */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    #if (MCI_BUS_WIDTH_4)
      GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    #endif

    #if (MCI_BUS_WIDTH_8)
      Enable_GPIO_Clock (GPIOB);

      GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    #endif

    /* Configure CD (Card Detect) Pin */
    #if defined (MX_MemoryCard_CD_Pin)
      #if defined (USE_STM32756G_EVAL)
        BSP_IO_Init();
        BSP_IO_ConfigPin(IO_PIN_15, IO_MODE_INPUT_PU);
      #else
        Enable_GPIO_Clock (MX_MemoryCard_CD_GPIOx);

        GPIO_InitStruct.Pin  = MX_MemoryCard_CD_GPIO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = MX_MemoryCard_CD_GPIO_PuPd;
        HAL_GPIO_Init(MX_MemoryCard_CD_GPIOx, &GPIO_InitStruct);
      #endif
    #endif

    /* Configure WP (Write Protect) Pin */
    #if defined (MX_MemoryCard_WP_Pin)
      Enable_GPIO_Clock (MX_MemoryCard_WP_GPIOx);

      GPIO_InitStruct.Pin  = MX_MemoryCard_WP_GPIO_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = MX_MemoryCard_WP_GPIO_PuPd;
      HAL_GPIO_Init(MX_MemoryCard_WP_GPIOx, &GPIO_InitStruct);
    #endif

    /* Enable DMA peripheral clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Configure DMA receive stream */
    hdma_sdmmc1_rx.Instance                 = MX_SDMMC1_RX_DMA_Instance;
    hdma_sdmmc1_rx.Init.Channel             = MX_SDMMC1_RX_DMA_Channel;
    hdma_sdmmc1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_sdmmc1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sdmmc1_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sdmmc1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_sdmmc1_rx.Init.Mode                = DMA_PFCTRL;
    hdma_sdmmc1_rx.Init.Priority            = MX_SDMMC1_RX_DMA_Priority;
    hdma_sdmmc1_rx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc1_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc1_rx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_sdmmc1_rx.Init.PeriphBurst         = DMA_PBURST_INC4;

    if (HAL_DMA_Init(&hdma_sdmmc1_rx) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    /* Configure DMA transmit stream */
    hdma_sdmmc1_tx.Instance                 = MX_SDMMC1_TX_DMA_Instance;
    hdma_sdmmc1_tx.Init.Channel             = MX_SDMMC1_TX_DMA_Channel;
    hdma_sdmmc1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_sdmmc1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sdmmc1_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sdmmc1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_sdmmc1_tx.Init.Mode                = DMA_PFCTRL;
    hdma_sdmmc1_tx.Init.Priority            = MX_SDMMC1_TX_DMA_Priority;
    hdma_sdmmc1_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc1_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc1_tx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_sdmmc1_tx.Init.PeriphBurst         = DMA_PBURST_INC4;

    if (HAL_DMA_Init(&hdma_sdmmc1_tx) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  #else
    hsd1.Instance = SDMMC1;
  #endif

  /* Set DMA callback function */
  hdma_sdmmc1_rx.XferCpltCallback = &RX_DMA_Complete;

  /* Clear control structure */
  memset (&MCI, 0, sizeof (MCI_INFO));

  MCI.cb_event = cb_event;
  MCI.flags    = MCI_INIT;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t Uninitialize (void)
  \brief         De-initialize Memory Card Interface.
  \return        \ref execution_status
*/
static int32_t Uninitialize (void) {

  MCI.flags = 0U;

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* SDMMC1_CMD, SDMMC1_CK and SDMMC1_D0 pins */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12|GPIO_PIN_8);

    #if (MCI_BUS_WIDTH_4)
      /* SDMMC1_D[3..1] */
      HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_9);
    #endif

    #if (MCI_BUS_WIDTH_8)
      /* SDMMC1_D[7..4] */
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
      HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7|GPIO_PIN_6);
    #endif
  #endif /* RTE_DEVICE_FRAMEWORK_CLASSIC */

  #if defined (MX_MemoryCard_CD_Pin)
    /* Unconfigure CD (Card Detect) Pin */
    #if defined (USE_STM32756G_EVAL)
      BSP_IO_ConfigPin(IO_PIN_15, IO_MODE_OFF);
    #else
      HAL_GPIO_DeInit (MX_MemoryCard_CD_GPIOx, MX_MemoryCard_CD_GPIO_Pin);
    #endif
  #endif

  #if defined (MX_MemoryCard_WP_Pin)
    /* Unconfigure WP (Write Protect) Pin */
    HAL_GPIO_DeInit (MX_MemoryCard_WP_GPIOx, MX_MemoryCard_WP_GPIO_Pin);
  #endif

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t PowerControl (ARM_POWER_STATE state)
  \brief         Control Memory Card Interface Power.
  \param[in]     state   Power state \ref ARM_POWER_STATE
  \return        \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  int32_t status;

  status = ARM_DRIVER_OK;

  switch (state) {
    case ARM_POWER_OFF:
      /* Reset/Dereset SDMMC1 peripheral */
      __HAL_RCC_SDMMC1_FORCE_RESET();
      __NOP(); __NOP(); __NOP(); __NOP();
      __HAL_RCC_SDMMC1_RELEASE_RESET();

      #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
        /* Disable SDIO interrupts in NVIC */
        HAL_NVIC_DisableIRQ (SDMMC1_IRQn);

        /* Disable DMA stream interrupts in NVIC */
        HAL_NVIC_DisableIRQ (SDMMC1_RX_DMA_IRQn);
        HAL_NVIC_DisableIRQ (SDMMC1_TX_DMA_IRQn);

        /* Disable DMA stream */
        if (HAL_DMA_DeInit (&hdma_sdmmc1_rx) != HAL_OK) {
          status = ARM_DRIVER_ERROR;
        }
        if (HAL_DMA_DeInit (&hdma_sdmmc1_tx) != HAL_OK) {
          status = ARM_DRIVER_ERROR;
        }

        /* SDMMC1 peripheral clock disable */
        __HAL_RCC_SDMMC1_CLK_DISABLE();
      #else
        HAL_SD_MspDeInit (&hsd1);
      #endif

      /* Clear status */
      MCI.status.command_active   = 0U;
      MCI.status.command_timeout  = 0U;
      MCI.status.command_error    = 0U;
      MCI.status.transfer_active  = 0U;
      MCI.status.transfer_timeout = 0U;
      MCI.status.transfer_error   = 0U;
      MCI.status.sdio_interrupt   = 0U;
      MCI.status.ccs              = 0U;

      MCI.flags = MCI_INIT;
      break;

    case ARM_POWER_FULL:
      if ((MCI.flags & MCI_POWER) == 0) {
        #if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
          HAL_SD_MspInit (&hsd1);
        #else
        /* Enable SDMMC1 peripheral clock */
        __HAL_RCC_SDMMC1_CLK_ENABLE();
        #endif

        /* Clear response and transfer variables */
        MCI.response = NULL;
        MCI.xfer.cnt = 0U;

        /* Enable SDMMC1 peripheral interrupts */
        SDMMC1->MASK = SDMMC_MASK_DATAENDIE  |
                       SDMMC_MASK_CMDSENTIE  |
                       SDMMC_MASK_CMDRENDIE  |
                       SDMMC_MASK_DTIMEOUTIE |
                       SDMMC_MASK_CTIMEOUTIE |
                       SDMMC_MASK_DCRCFAILIE |
                       SDMMC_MASK_CCRCFAILIE ;

        /* Set max data timeout */
        SDMMC1->DTIMER = 0xFFFFFFFF;

        /* Enable clock to the card (SDIO_CK) */
        SDMMC1->POWER = SDMMC_POWER_PWRCTRL_1 | SDMMC_POWER_PWRCTRL_0;

        #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
          /* Enable DMA stream interrupts in NVIC */
          HAL_NVIC_EnableIRQ(SDMMC1_RX_DMA_IRQn);
          HAL_NVIC_EnableIRQ(SDMMC1_TX_DMA_IRQn);

          HAL_NVIC_ClearPendingIRQ(SDMMC1_IRQn);
          HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
        #endif

        MCI.flags |= MCI_POWER;
      }
      break;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return status;
}


/**
  \fn            int32_t CardPower (uint32_t voltage)
  \brief         Set Memory Card supply voltage.
  \param[in]     voltage  Memory Card supply voltage
  \return        \ref execution_status
*/
static int32_t CardPower (uint32_t voltage) {

  if ((MCI.flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}


/**
  \fn            int32_t ReadCD (void)
  \brief         Read Card Detect (CD) state.
  \return        1:card detected, 0:card not detected, or error
*/
static int32_t ReadCD (void) {

  if ((MCI.flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  /* Read CD (Card Detect) Pin */
  #if defined (MX_MemoryCard_CD_Pin)
    #if defined (USE_STM32756G_EVAL)
      if (BSP_IO_ReadPin (IO_PIN_15) == BSP_IO_PIN_RESET) {
        /* Card Detect switch is active */
        return (1);
      }
    #else
      if (HAL_GPIO_ReadPin (MX_MemoryCard_CD_GPIOx, MX_MemoryCard_CD_GPIO_Pin) == MemoryCard_CD_Pin_Active) {
        /* Card Detect switch is active */
        return (1);
      }
    #endif
  #endif
  return (0);
}


/**
  \fn            int32_t ReadWP (void)
  \brief         Read Write Protect (WP) state.
  \return        1:write protected, 0:not write protected, or error
*/
static int32_t ReadWP (void) {

  if ((MCI.flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  /* Read WP (Write Protect) Pin */
  #if defined (MX_MemoryCard_WP_Pin)
  if (HAL_GPIO_ReadPin (MX_MemoryCard_WP_GPIOx, MX_MemoryCard_WP_GPIO_Pin) == MemoryCard_WP_Pin_Active) {
    /* Write protect switch is active */
    return (1);
  }
  #endif
  return (0);
}


/**
  \fn            int32_t SendCommand (uint32_t  cmd,
                                      uint32_t  arg,
                                      uint32_t  flags,
                                      uint32_t *response)
  \brief         Send Command to card and get the response.
  \param[in]     cmd       Memory Card command
  \param[in]     arg       Command argument
  \param[in]     flags     Command flags
  \param[out]    response  Pointer to buffer for response
  \return        \ref execution_status
*/
static int32_t SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response) {
  uint32_t i, clkcr;

  if (((flags & MCI_RESPONSE_EXPECTED_Msk) != 0U) && (response == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((MCI.flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.command_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  MCI.status.command_active   = 1U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.ccs              = 0U;

  if (flags & ARM_MCI_CARD_INITIALIZE) {
    clkcr = SDMMC1->CLKCR;

    if (((clkcr & SDMMC_CLKCR_CLKEN) == 0) || ((clkcr & SDMMC_CLKCR_PWRSAV) != 0)) {
      SDMMC1->CLKCR = (SDMMC1->CLKCR & ~SDMMC_CLKCR_PWRSAV) | SDMMC_CLKCR_CLKEN;

      i = HAL_RCC_GetHCLKFreq();
      for (i = (i/5000000U)*1000U; i; i--) {
        ; /* Wait for approximate 1000us */
      }
      SDMMC1->CLKCR = clkcr;
    }
  }

  /* Set command register value */
  cmd = SDMMC_CMD_CPSMEN | (cmd & 0xFFU);

  MCI.response = response;
  MCI.flags   &= ~(MCI_RESP_CRC | MCI_RESP_LONG);

  switch (flags & ARM_MCI_RESPONSE_Msk) {
    case ARM_MCI_RESPONSE_NONE:
      /* No response expected (wait CMDSENT) */
      break;

    case ARM_MCI_RESPONSE_SHORT:
    case ARM_MCI_RESPONSE_SHORT_BUSY:
      /* Short response expected (wait CMDREND or CCRCFAIL) */
      cmd |= SDMMC_CMD_WAITRESP_0;
      break;

    case ARM_MCI_RESPONSE_LONG:
      MCI.flags |= MCI_RESP_LONG;
      /* Long response expected (wait CMDREND or CCRCFAIL) */
      cmd |= SDMMC_CMD_WAITRESP_1 | SDMMC_CMD_WAITRESP_0;
      break;

    default:
      return ARM_DRIVER_ERROR;
  }
  if (flags & ARM_MCI_RESPONSE_CRC) {
    MCI.flags |= MCI_RESP_CRC;
  }
  if (flags & ARM_MCI_TRANSFER_DATA) {
    MCI.flags |= MCI_DATA_XFER;
  }

  /* Clear all interrupt flags */
  SDMMC1->ICR = SDMMC_ICR_BIT_Msk;

  /* Send the command */
  SDMMC1->ARG = arg;
  SDMMC1->CMD = cmd;

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t SetupTransfer (uint8_t *data,
                                        uint32_t block_count,
                                        uint32_t block_size,
                                        uint32_t mode)
  \brief         Setup read or write transfer operation.
  \param[in,out] data         Pointer to data block(s) to be written or read
  \param[in]     block_count  Number of blocks
  \param[in]     block_size   Size of a block in bytes
  \param[in]     mode         Transfer mode
  \return        \ref execution_status
*/
static int32_t SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode) {
  uint32_t sz, cnt, dctrl;

  if ((data == NULL) || (block_count == 0U) || (block_size == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }

  if ((MCI.flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.transfer_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  MCI.xfer.buf = data;
  MCI.xfer.cnt = block_count * block_size;

  cnt = MCI.xfer.cnt;
  if (cnt > 0xFFFFU) {
    cnt = 0xFFFFU;
  }

  MCI.xfer.cnt -= cnt;
  MCI.xfer.buf += cnt;

  dctrl = 0U;

  if ((mode & ARM_MCI_TRANSFER_WRITE) == 0) {
    /* Direction: From card to controller */
    MCI.flags |= MCI_DATA_READ;
    dctrl |= SDMMC_DCTRL_DTDIR;
  }
  else {
    MCI.flags &= ~MCI_DATA_READ;
  }

  if (mode & ARM_MCI_TRANSFER_STREAM) {
    /* Stream or SDIO multibyte data transfer enable */
    dctrl |= SDMMC_DCTRL_DTMODE;
  }

  /* Set data block size */
  if (block_size == 512U) {
    sz = 9U;
  }
  else {
    if (block_size > 16384U) {
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    for (sz = 0U; sz < 14U; sz++) {
      if (block_size & (1UL << sz)) {
        break;
      }
    }
  }

  if (mode & ARM_MCI_TRANSFER_WRITE) {
    /* Enable TX DMA stream */
    if (HAL_DMA_Start_IT (&hdma_sdmmc1_tx, (uint32_t)data, (uint32_t)&(SDMMC1->FIFO), cnt) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }
  else {
    /* Enable RX DMA stream */
    if (HAL_DMA_Start_IT (&hdma_sdmmc1_rx, (uint32_t)&(SDMMC1->FIFO), (uint32_t)data, cnt) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }

  MCI.dlen   = cnt;
  MCI.dctrl  = dctrl | (sz << 4) | SDMMC_DCTRL_DMAEN;

  return (ARM_DRIVER_OK);
}


/**
  \fn            int32_t AbortTransfer (void)
  \brief         Abort current read/write data transfer.
  \return        \ref execution_status
*/
static int32_t AbortTransfer (void) {
  int32_t  status;
  uint32_t mask;

  if ((MCI.flags & MCI_SETUP) == 0U) { return ARM_DRIVER_ERROR; }

  status = ARM_DRIVER_OK;

  /* Disable SDIO interrupts */
  mask = SDMMC1->MASK;
  SDMMC1->MASK = 0U;

  /* Disable DMA and clear data transfer bit */
  SDMMC1->DCTRL &= ~(SDMMC_DCTRL_DMAEN | SDMMC_DCTRL_DTEN);

  if (HAL_DMA_Abort (&hdma_sdmmc1_rx) != HAL_OK) {
    status = ARM_DRIVER_ERROR;
  }
  if (HAL_DMA_Abort (&hdma_sdmmc1_tx) != HAL_OK) {
    status = ARM_DRIVER_ERROR;
  }

  /* Clear SDIO FIFO */
  while (SDMMC1->FIFOCNT) {
    SDMMC1->FIFO;
  }

  MCI.status.command_active  = 0U;
  MCI.status.transfer_active = 0U;
  MCI.status.sdio_interrupt  = 0U;
  MCI.status.ccs             = 0U;

  /* Clear pending SDIO interrupts */
  SDMMC1->ICR = SDMMC_ICR_BIT_Msk;

  /* Enable SDIO interrupts */
  SDMMC1->MASK = mask;

  return status;
}


/**
  \fn            int32_t Control (uint32_t control, uint32_t arg)
  \brief         Control MCI Interface.
  \param[in]     control  Operation
  \param[in]     arg      Argument of operation (optional)
  \return        \ref execution_status
*/
static int32_t Control (uint32_t control, uint32_t arg) {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t val, clkdiv, bps;

  if ((MCI.flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  switch (control) {
    case ARM_MCI_BUS_SPEED:
      /* Determine clock divider and set bus speed */
      bps = arg;

      if ((bps < SDMMCCLK) || (MCI_BUS_MODE_HS == 0U)) {
        /* bps = SDIOCLK / (clkdiv + 2) */
        clkdiv = (SDMMCCLK + bps - 1U) / bps;

        if (clkdiv < 2) { clkdiv  = 0U; }
        else            { clkdiv -= 2U; }

        if (clkdiv > SDMMC_CLKCR_CLKDIV) {
          clkdiv  = SDMMC_CLKCR_CLKDIV;
        }

        SDMMC1->CLKCR = (SDMMC1->CLKCR & ~(SDMMC_CLKCR_CLKDIV | SDMMC_CLKCR_BYPASS)) |
                         SDMMC_CLKCR_PWRSAV | SDMMC_CLKCR_CLKEN | clkdiv;
        bps = SDMMCCLK / (clkdiv + 2U);
      }
      else {
        /* Max output clock is SDIOCLK */
        SDMMC1->CLKCR |= SDMMC_CLKCR_BYPASS | SDMMC_CLKCR_PWRSAV | SDMMC_CLKCR_CLKEN;

        bps = SDMMCCLK;
      }

      for (val = (SDMMCCLK/5000000U)*20U; val; val--) {
        ; /* Wait a bit to get stable clock */
      }

      /* Bus speed configured */
      MCI.flags |= MCI_SETUP;
      return ((int32_t)bps);

    case ARM_MCI_BUS_SPEED_MODE:
      switch (arg) {
        case ARM_MCI_BUS_DEFAULT_SPEED:
          /* Speed mode up to 25MHz */
          SDMMC1->CLKCR &= ~SDMMC_CLKCR_NEGEDGE;
          break;
        case ARM_MCI_BUS_HIGH_SPEED:
          /* Speed mode up to 50MHz */
          break;
        default: return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_BUS_CMD_MODE:
      switch (arg) {
        case ARM_MCI_BUS_CMD_OPEN_DRAIN:
          /* Configure command line in open-drain mode */
          val = GPIO_MODE_AF_OD;
          break;
        case ARM_MCI_BUS_CMD_PUSH_PULL:
          /* Configure command line in push-pull mode */
          val = GPIO_MODE_AF_PP;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      GPIO_InitStruct.Pin = GPIO_PIN_2;
      GPIO_InitStruct.Mode = val;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
      break;

    case ARM_MCI_BUS_DATA_WIDTH:
      switch (arg) {
        case ARM_MCI_BUS_DATA_WIDTH_1:
          SDMMC1->CLKCR &= ~SDMMC_CLKCR_WIDBUS;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_4:
          SDMMC1->CLKCR = (SDMMC1->CLKCR & ~SDMMC_CLKCR_WIDBUS) | SDMMC_CLKCR_WIDBUS_0;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_8:
          SDMMC1->CLKCR = (SDMMC1->CLKCR & ~SDMMC_CLKCR_WIDBUS) | SDMMC_CLKCR_WIDBUS_1;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_CONTROL_CLOCK_IDLE:
      if (arg) {
        /* Clock generation enabled when idle */
        SDMMC1->CLKCR &= ~SDMMC_CLKCR_PWRSAV;
      }
      else {
        /* Clock generation disabled when idle */
        SDMMC1->CLKCR |= SDMMC_CLKCR_PWRSAV;
      }
      break;

    case ARM_MCI_DATA_TIMEOUT:
      SDMMC1->DTIMER = arg;
      break;

    case ARM_MCI_MONITOR_SDIO_INTERRUPT:
      MCI.status.sdio_interrupt = 0U;
      SDMMC1->MASK |= SDMMC_MASK_SDIOITIE;
      break;

    case ARM_MCI_CONTROL_READ_WAIT:
      if (arg) {
        /* Assert read wait */
        MCI.flags |= MCI_READ_WAIT;
      }
      else {
        /* Clear read wait */
        MCI.flags &= ~MCI_READ_WAIT;
        SDMMC1->DCTRL &= ~SDMMC_DCTRL_RWSTOP;
      }
      break;

    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}


/**
  \fn            ARM_MCI_STATUS GetStatus (void)
  \brief         Get MCI status.
  \return        MCI status \ref ARM_MCI_STATUS
*/
static ARM_MCI_STATUS GetStatus (void) {
  return MCI.status;
}


/* SDMMC1 interrupt handler */
void SDMMC1_IRQHandler (void) {
  uint32_t sta, icr, event, mask;

  event = 0U;
  icr   = 0U;

  /* Read SDMMC1 interrupt status */
  sta = SDMMC1->STA;

  if (sta & SDMMC_STA_ERR_BIT_Msk) {
    /* Check error interrupts */
    if (sta & SDMMC_STA_CCRCFAIL) {
      icr |= SDMMC_ICR_CCRCFAILC;
      /* Command response CRC check failed */
      if (MCI.flags & MCI_RESP_CRC) {
        MCI.status.command_error = 1U;

        event |= ARM_MCI_EVENT_COMMAND_ERROR;
      }
      else {
        /* Ignore CRC error and read the response */
        sta |= SDMMC_STA_CMDREND;
      }
    }
    if (sta & SDMMC_STA_DCRCFAIL) {
      icr |= SDMMC_ICR_DCRCFAILC;
      /* Data block CRC check failed */
      MCI.status.transfer_error = 1U;

      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    if (sta & SDMMC_STA_CTIMEOUT) {
      icr |= SDMMC_ICR_CTIMEOUTC;
      /* Command response timeout */
      MCI.status.command_timeout = 1U;

      event |= ARM_MCI_EVENT_COMMAND_TIMEOUT;
    }
    if (sta & SDMMC_STA_DTIMEOUT) {
      icr |= SDMMC_ICR_DTIMEOUTC;
      /* Data timeout */
      MCI.status.transfer_timeout = 1U;

      event |= ARM_MCI_EVENT_TRANSFER_TIMEOUT;
    }
  }

  if (sta & SDMMC_STA_CMDREND) {
    icr |= SDMMC_ICR_CMDRENDC;
    /* Command response received */
    event |= ARM_MCI_EVENT_COMMAND_COMPLETE;

    if (MCI.response) {
      /* Read response registers */
      if (MCI.flags & MCI_RESP_LONG) {
        MCI.response[0] = SDMMC1->RESP4;
        MCI.response[1] = SDMMC1->RESP3;
        MCI.response[2] = SDMMC1->RESP2;
        MCI.response[3] = SDMMC1->RESP1;
      }
      else {
        MCI.response[0] = SDMMC1->RESP1;
      }
    }
    if (MCI.flags & MCI_DATA_XFER) {
      MCI.flags &= ~MCI_DATA_XFER;

      if (MCI.flags & MCI_READ_WAIT) {
        MCI.dctrl |= SDMMC_DCTRL_RWSTART;
      }

      /* Start data transfer */
      SDMMC1->DLEN  = MCI.dlen;
      SDMMC1->DCTRL = MCI.dctrl | SDMMC_DCTRL_DTEN;

      MCI.status.transfer_active = 1U;
    }
  }
  if (sta & SDMMC_STA_CMDSENT) {
    icr |= SDMMC_ICR_CMDSENTC;
    /* Command sent (no response required) */
    event |= ARM_MCI_EVENT_COMMAND_COMPLETE;
  }
  if (sta & SDMMC_STA_DATAEND) {
    icr |= SDMMC_ICR_DATAENDC;
    /* Data end (DCOUNT is zero) */
    if ((MCI.flags & MCI_DATA_READ) == 0) {
    /* Write transfer */
      SDMMC1->MASK |= SDMMC_MASK_DBCKENDIE;
    }
  }
  if (sta & SDMMC_STA_DBCKEND) {
    icr |= SDMMC_ICR_DBCKENDC;
    /* Data block sent/received (CRC check passed) */
    if ((MCI.flags & MCI_DATA_READ) == 0) {
      /* Write transfer */
      if (MCI.xfer.cnt == 0) {
        event |= ARM_MCI_EVENT_TRANSFER_COMPLETE;
      }
    }
    SDMMC1->MASK &= ~SDMMC_MASK_DBCKENDIE;
  }
  if (sta & SDMMC_STA_SDIOIT) {
    icr |= SDMMC_ICR_SDIOITC;
    /* Disable interrupt (must be re-enabled using Control) */
    SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;

    event |= ARM_MCI_EVENT_SDIO_INTERRUPT;
  }

  /* Clear processed interrupts */
  SDMMC1->ICR = icr;

  if (event) {
    /* Check for transfer events */
    mask = ARM_MCI_EVENT_TRANSFER_ERROR   |
           ARM_MCI_EVENT_TRANSFER_TIMEOUT |
           ARM_MCI_EVENT_TRANSFER_COMPLETE;
    if (event & mask) {
      MCI.status.transfer_active = 0U;

      if (MCI.cb_event) {
        if (event & ARM_MCI_EVENT_TRANSFER_ERROR) {
          (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_ERROR);
        }
        else if (event & ARM_MCI_EVENT_TRANSFER_TIMEOUT) {
          (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_TIMEOUT);
        }
        else {
          (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_COMPLETE);
        }
      }
    }
    /* Check for command events */
    mask = ARM_MCI_EVENT_COMMAND_ERROR   |
           ARM_MCI_EVENT_COMMAND_TIMEOUT |
           ARM_MCI_EVENT_COMMAND_COMPLETE;
    if (event & mask) {
      MCI.status.command_active = 0U;

      if (MCI.cb_event) {
        if (event & ARM_MCI_EVENT_COMMAND_ERROR) {
          (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_ERROR);
        }
        else if (event & ARM_MCI_EVENT_COMMAND_TIMEOUT) {
          (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_TIMEOUT);
        }
        else {
          (MCI.cb_event)(ARM_MCI_EVENT_COMMAND_COMPLETE);
        }
      }
    }
    /* Check for SDIO INT event */
    if (event & ARM_MCI_EVENT_SDIO_INTERRUPT) {
      MCI.status.sdio_interrupt = 1U;

      if (MCI.cb_event) {
        (MCI.cb_event)(ARM_MCI_EVENT_SDIO_INTERRUPT);
      }
    }
  }
}


/* Rx DMA Callback */
void RX_DMA_Complete(struct __DMA_HandleTypeDef *hdma) {

  MCI.status.transfer_active = 0U;

  if (MCI.cb_event) {
    (MCI.cb_event)(ARM_MCI_EVENT_TRANSFER_COMPLETE);
  }
}

#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/* RX DMA Stream IRQ Handler */
void SDMMC1_RX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(SDMMC1_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc1_rx);
}

/* TX DMA Stream IRQ Handler */
void SDMMC1_TX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(SDMMC1_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc1_tx);
}
#endif


/* MCI Driver Control Block */
ARM_DRIVER_MCI Driver_MCI0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  CardPower,
  ReadCD,
  ReadWP,
  SendCommand,
  SetupTransfer,
  AbortTransfer,
  Control,
  GetStatus
};

/*! \endcond */
