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
 * $Date:        08. November 2016
 * $Revision:    V1.4
 *
 * Driver:       Driver_MCI0, Driver_MCI1
 * Configured:   via RTE_Device.h configuration file
 * Project:      MCI Driver for ST STM32F7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   SDMMC Interface
 *   ---------------------                 -----   ---------------
 *   Connect to hardware via Driver_MCI# = 0       use SDMMC1
 *   Connect to hardware via Driver_MCI# = 1       use SDMMC2
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.4
 *    Added Driver_MCI1 instance (SDMMC2)
 *  Version 1.3
 *    Corrected PowerControl function for:
 *      - Unconditional Power Off
 *      - Conditional Power full (driver must be initialized)
 *  Version 1.2
 *    Clock power save bit handling removed from ARM_MCI_BUS_SPEED control
 *  Version 1.1
 *    Enhanced STM32CubeMx compatibility
 *  Version 1.0
 *    Initial release
 */

/*! \page stm32f7_mci CMSIS-Driver MCI Setup 

The CMSIS-Driver MCI requires:
  - Setup of SDMMC1 with DMA for Rx and Tx DMA transfers.
  - Optional Configuration for Card Detect Pin:
    - Configure arbitrary pin in GPIO_Input mode and add User Label: MemoryCard_CD0
  - Optional Configuration for Write Protect Pin:
    - Configure arbitrary pin in GPIO_Input mode and add User Label: MemoryCard_WP0

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
     - Peripherals \b SDMMC1: Mode=<b>SD 4 bits Wide bus</b>
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
          PC13     | n/a           | Input mode| No pull-up and no..| n/a         |\b MemoryCard_CD0

     Click \b OK to close the Pin Configuration dialog
*/

/*! \cond */

#include "MCI_STM32F7xx.h"

#define ARM_MCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,4)  /* driver version */

/* MCI0: Define Card Detect pin active state */
#if !defined(MemoryCard_CD0_Pin_Active)
  #define MemoryCard_CD0_Pin_Active GPIO_PIN_RESET
#endif

/* MCI1: Define Card Detect pin active state */
#if !defined(MemoryCard_CD1_Pin_Active)
  #define MemoryCard_CD1_Pin_Active GPIO_PIN_RESET
#endif

/* MCI0: Define Write Protect pin active state */
#if !defined(MemoryCard_WP0_Pin_Active)
  #define MemoryCard_WP0_Pin_Active GPIO_PIN_SET
#endif

/* MCI1: Define Write Protect pin active state */
#if !defined(MemoryCard_WP1_Pin_Active)
  #define MemoryCard_WP1_Pin_Active GPIO_PIN_SET
#endif

/* MCI0: Disable High Speed bus mode */
#if defined(MemoryCard_Bus0_Mode_HS_Disable)
  #define MCI0_BUS_MODE_HS    0U
#else
  #define MCI0_BUS_MODE_HS    1U
#endif

/* MCI1: Disable High Speed bus mode */
#if defined(MemoryCard_Bus1_Mode_HS_Disable)
  #define MCI1_BUS_MODE_HS    0U
#else
  #define MCI1_BUS_MODE_HS    1U
#endif


#if defined(MX_SDMMC1)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_sdmmc1_rx = { 0U };
static DMA_HandleTypeDef hdma_sdmmc1_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_sdmmc1_rx;
extern DMA_HandleTypeDef hdma_sdmmc1_tx;
extern SD_HandleTypeDef  hsd1;
#endif

/* DMA receive complete callback prototype */
void MCI0_RX_DMA_Complete(DMA_HandleTypeDef *hdma);

/* IRQ handler prototype */
void SDMMC1_IRQHandler (void);

#if (MCI0_CD_PIN != 0U)
static MCI_IO MCI0_IO_CD = { MX_MemoryCard_CD0_GPIOx,
                             MX_MemoryCard_CD0_GPIO_Pin,
                             MX_MemoryCard_CD0_GPIO_PuPd,
                             MemoryCard_CD0_Pin_Active };
#endif
#if (MCI0_WP_PIN != 0U)
static MCI_IO MCI0_IO_WP = { MX_MemoryCard_WP0_GPIOx,
                             MX_MemoryCard_WP0_GPIO_Pin,
                             MX_MemoryCard_WP0_GPIO_PuPd,
                             MemoryCard_WP0_Pin_Active };
#endif

#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/* MCI0 I/O pin configuration */
static MCI_IO MCI0_IO[] = {
/* CMD */{RTE_SDMMC1_CMD_PORT, RTE_SDMMC1_CMD_PIN, GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* CK */ {RTE_SDMMC1_CK_PORT,  RTE_SDMMC1_CK_PIN,  GPIO_NOPULL, GPIO_AF12_SDMMC1},
/* D0 */ {RTE_SDMMC1_D0_PORT,  RTE_SDMMC1_D0_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
#if (MCI0_BUS_WIDTH_4 != 0)
/* D1 */ {RTE_SDMMC1_D1_PORT,  RTE_SDMMC1_D1_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* D2 */ {RTE_SDMMC1_D2_PORT,  RTE_SDMMC1_D2_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* D3 */ {RTE_SDMMC1_D3_PORT,  RTE_SDMMC1_D3_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
#endif
#if (MCI0_BUS_WIDTH_8 != 0)
/* D4 */ {RTE_SDMMC1_D4_PORT,  RTE_SDMMC1_D4_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* D5 */ {RTE_SDMMC1_D5_PORT,  RTE_SDMMC1_D5_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* D6 */ {RTE_SDMMC1_D6_PORT,  RTE_SDMMC1_D6_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
/* D7 */ {RTE_SDMMC1_D7_PORT,  RTE_SDMMC1_D7_PIN,  GPIO_PULLUP, GPIO_AF12_SDMMC1},
#endif
};
#endif

/* MCI0 Rx DMA */
static const MCI_DMA MCI0_RX_DMA = {
  &hdma_sdmmc1_rx,
  &MCI0_RX_DMA_Complete,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_SDMMC1_RX_DMA_Instance,
  MX_SDMMC1_RX_DMA_IRQn,
  MX_SDMMC1_RX_DMA_Channel,
  MX_SDMMC1_RX_DMA_Priority
  #endif
};

/* MCI0 Tx DMA */
static const MCI_DMA MCI0_TX_DMA = {
  &hdma_sdmmc1_tx,
  NULL,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_SDMMC1_TX_DMA_Instance,
  MX_SDMMC1_TX_DMA_IRQn,
  MX_SDMMC1_TX_DMA_Channel,
  MX_SDMMC1_TX_DMA_Priority
  #endif
};

/* MCI0 Information (Run-Time) */
static MCI_INFO MCI0_Info;

/* MCI0 Resources */
static MCI_RESOURCES MCI0_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hsd1,
#endif
  SDMMC1,
  &MCI0_RX_DMA,
  &MCI0_TX_DMA,
  SDMMC1_IRQn,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  sizeof (MCI0_IO) / sizeof (MCI0_IO[0]),
  MCI0_IO,
  #else
  0,
  NULL,
  #endif
  #if (MCI0_CD_PIN != 0U)
  &MCI0_IO_CD,
  #else
  NULL,
  #endif
  #if (MCI0_WP_PIN != 0U)
  &MCI0_IO_WP,
  #else
  NULL,
  #endif
  &MCI0_Info
};

/* MCI0 Driver Capabilities */
static const ARM_MCI_CAPABILITIES MCI0_DriverCapabilities = {
  MCI0_CD_PIN,                                    /* cd_state          */
  0U,                                             /* cd_event          */
  MCI0_WP_PIN,                                    /* wp_state          */
  0U,                                             /* vdd               */
  0U,                                             /* vdd_1v8           */
  0U,                                             /* vccq              */
  0U,                                             /* vccq_1v8          */
  0U,                                             /* vccq_1v2          */
  MCI0_BUS_WIDTH_4,                               /* data_width_4      */
  MCI0_BUS_WIDTH_8,                               /* data_width_8      */
  0U,                                             /* data_width_4_ddr  */
  0U,                                             /* data_width_8_ddr  */
  MCI0_BUS_MODE_HS,                               /* high_speed        */
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

#endif /* MX_SDMMC1 */

#if defined(MX_SDMMC2)
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
static DMA_HandleTypeDef hdma_sdmmc2_rx = { 0U };
static DMA_HandleTypeDef hdma_sdmmc2_tx = { 0U };
#else
extern DMA_HandleTypeDef hdma_sdmmc2_rx;
extern DMA_HandleTypeDef hdma_sdmmc2_tx;
extern SD_HandleTypeDef  hsd2;
#endif

/* DMA receive complete callback prototype */
void MCI1_RX_DMA_Complete(DMA_HandleTypeDef *hdma);

/* IRQ handler prototype */
void SDMMC2_IRQHandler (void);

#if (MCI1_CD_PIN != 0U)
static MCI_IO MCI1_IO_CD = { MX_MemoryCard_CD1_GPIOx,
                             MX_MemoryCard_CD1_GPIO_Pin,
                             MX_MemoryCard_CD1_GPIO_PuPd,
                             MemoryCard_CD1_Pin_Active };
#endif
#if (MCI1_WP_PIN != 0U)
static MCI_IO MCI1_IO_WP = { MX_MemoryCard_WP1_GPIOx,
                             MX_MemoryCard_WP1_GPIO_Pin,
                             MX_MemoryCard_WP1_GPIO_PuPd,
                             MemoryCard_WP1_Pin_Active };
#endif

#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/* MCI1 I/O pin configuration */
static MCI_IO MCI1_IO[] = {
/* CMD */{RTE_SDMMC2_CMD_PORT, RTE_SDMMC2_CMD_PIN, GPIO_PULLUP, RTE_SDMMC2_CMD_AF},
/* CK */ {RTE_SDMMC2_CK_PORT,  RTE_SDMMC2_CK_PIN,  GPIO_NOPULL, RTE_SDMMC2_CK_AF},
/* D0 */ {RTE_SDMMC2_D0_PORT,  RTE_SDMMC2_D0_PIN,  GPIO_PULLUP, RTE_SDMMC2_D0_AF},
#if (MCI1_BUS_WIDTH_4 != 0)
/* D1 */ {RTE_SDMMC2_D1_PORT,  RTE_SDMMC2_D1_PIN,  GPIO_PULLUP, RTE_SDMMC2_D1_AF},
/* D2 */ {RTE_SDMMC2_D2_PORT,  RTE_SDMMC2_D2_PIN,  GPIO_PULLUP, RTE_SDMMC2_D2_AF},
/* D3 */ {RTE_SDMMC2_D3_PORT,  RTE_SDMMC2_D3_PIN,  GPIO_PULLUP, RTE_SDMMC2_D3_AF},
#endif
#if (MCI1_BUS_WIDTH_8 != 0)
/* D4 */ {RTE_SDMMC2_D4_PORT,  RTE_SDMMC2_D4_PIN,  GPIO_PULLUP, RTE_SDMMC2_D4_AF},
/* D5 */ {RTE_SDMMC2_D5_PORT,  RTE_SDMMC2_D5_PIN,  GPIO_PULLUP, RTE_SDMMC2_D5_AF},
/* D6 */ {RTE_SDMMC2_D6_PORT,  RTE_SDMMC2_D6_PIN,  GPIO_PULLUP, RTE_SDMMC2_D6_AF},
/* D7 */ {RTE_SDMMC2_D7_PORT,  RTE_SDMMC2_D7_PIN,  GPIO_PULLUP, RTE_SDMMC2_D7_AF},
#endif
};
#endif

/* MCI1 Rx DMA */
static const MCI_DMA MCI1_RX_DMA = {
  &hdma_sdmmc2_rx,
  &MCI1_RX_DMA_Complete,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_SDMMC2_RX_DMA_Instance,
  MX_SDMMC2_RX_DMA_IRQn,
  MX_SDMMC2_RX_DMA_Channel,
  MX_SDMMC2_RX_DMA_Priority
  #endif
};

/* MCI1 Tx DMA */
static const MCI_DMA MCI1_TX_DMA = {
  &hdma_sdmmc2_tx,
  NULL,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  MX_SDMMC2_TX_DMA_Instance,
  MX_SDMMC2_TX_DMA_IRQn,
  MX_SDMMC2_TX_DMA_Channel,
  MX_SDMMC2_TX_DMA_Priority
  #endif
};

/* MCI1 Information (Run-Time) */
static MCI_INFO MCI1_Info;

/* MCI1 Resources */
static MCI_RESOURCES MCI1_Resources = {
#if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
  &hsd2,
#endif
  SDMMC2,
  &MCI1_RX_DMA,
  &MCI1_TX_DMA,
  SDMMC2_IRQn,
  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  sizeof (MCI1_IO) / sizeof (MCI1_IO[0]),
  MCI1_IO,
  #else
  0,
  NULL,
  #endif
  #if (MCI1_CD_PIN != 0U)
  &MCI1_IO_CD,
  #else
  NULL,
  #endif
  #if (MCI1_WP_PIN != 0U)
  &MCI1_IO_WP,
  #else
  NULL,
  #endif
  &MCI1_Info
};

/* MCI1 Driver Capabilities */
static const ARM_MCI_CAPABILITIES MCI1_DriverCapabilities = {
  MCI1_CD_PIN,                                    /* cd_state          */
  0U,                                             /* cd_event          */
  MCI1_WP_PIN,                                    /* wp_state          */
  0U,                                             /* vdd               */
  0U,                                             /* vdd_1v8           */
  0U,                                             /* vccq              */
  0U,                                             /* vccq_1v8          */
  0U,                                             /* vccq_1v2          */
  MCI1_BUS_WIDTH_4,                               /* data_width_4      */
  MCI1_BUS_WIDTH_8,                               /* data_width_8      */
  0U,                                             /* data_width_4_ddr  */
  0U,                                             /* data_width_8_ddr  */
  MCI1_BUS_MODE_HS,                               /* high_speed        */
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

#endif /* MX_SDMMC2 */


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_MCI_API_VERSION,
  ARM_MCI_DRV_VERSION
};

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

/**
  \fn          void Control_SDMMC_Reset (SDMMC_TypeDef *instance)
  \brief       Reset SDMMC peripheral
*/
void Control_SDMMC_Reset (SDMMC_TypeDef *instance) {

  if (instance == SDMMC1) {
    __HAL_RCC_SDMMC1_FORCE_RESET();
    __NOP(); __NOP(); __NOP(); __NOP();
    __HAL_RCC_SDMMC1_RELEASE_RESET();
  }
#if defined(SDMMC2)
  else {
    __HAL_RCC_SDMMC2_FORCE_RESET();
    __NOP(); __NOP(); __NOP(); __NOP();
    __HAL_RCC_SDMMC2_RELEASE_RESET();
  }
#endif
}

/**
  \fn          void Control_SDMMC_Clock (SDMMC_TypeDef *instance)
  \brief       Enable/or disable SDMMC peripheral input clock
*/
void Control_SDMMC_Clock (SDMMC_TypeDef *instance, uint32_t enable) {

  if (enable != 0U) {
    /* Enable SDMMC clock */
    if (instance == SDMMC1) { __HAL_RCC_SDMMC1_CLK_ENABLE(); }
#if defined(SDMMC2)
    else                    { __HAL_RCC_SDMMC2_CLK_ENABLE(); }
#endif
  }
  else {
    /* Disable SDMMC clock */
    if (instance == SDMMC1) { __HAL_RCC_SDMMC1_CLK_DISABLE(); }
#if defined(SDMMC2)
    else                    { __HAL_RCC_SDMMC2_CLK_DISABLE(); }
#endif
  }
}


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
static ARM_MCI_CAPABILITIES GetCapabilities (MCI_RESOURCES *mci) {
  ARM_MCI_CAPABILITIES cap;

#if defined(MX_SDMMC1)
  if (mci->reg == SDMMC1) { cap = MCI0_DriverCapabilities; }
#endif
#if defined(MX_SDMMC2)
  if (mci->reg == SDMMC2) { cap = MCI1_DriverCapabilities; }
#endif

  return (cap);
}


/**
  \fn            int32_t Initialize (ARM_MCI_SignalEvent_t cb_event)
  \brief         Initialize the Memory Card Interface
  \param[in]     cb_event  Pointer to \ref ARM_MCI_SignalEvent
  \return        \ref execution_status
*/
static int32_t Initialize (ARM_MCI_SignalEvent_t cb_event, MCI_RESOURCES *mci) {
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  uint32_t i;
#endif
  GPIO_InitTypeDef GPIO_InitStruct;

  if (mci->info->flags & MCI_INIT) { return ARM_DRIVER_OK; }

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* Configure pins */
    for (i = 0; i < mci->io_cnt; i++) {
      Enable_GPIO_Clock (mci->io[i].port);

      GPIO_InitStruct.Pin       = mci->io[i].pin;
      GPIO_InitStruct.Pull      = mci->io[i].pull;
      GPIO_InitStruct.Alternate = mci->io[i].af;

      GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

      HAL_GPIO_Init(mci->io[i].port, &GPIO_InitStruct);
    }

    /* Configure CD (Card Detect) Pin */
    #if (defined (USE_STM32756G_EVAL) || defined (USE_STM32F769I_EVAL)) && (MCI0_CD_PIN != 0U)
      BSP_IO_Init();
      BSP_IO_ConfigPin(IO_PIN_15, IO_MODE_INPUT_PU);
    #endif

    /* Enable DMA peripheral clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Configure DMA receive stream */
    mci->dma_rx->h->Instance                 = mci->dma_rx->stream;
    mci->dma_rx->h->Init.Channel             = mci->dma_rx->channel;
    mci->dma_rx->h->Init.Direction           = DMA_PERIPH_TO_MEMORY;
    mci->dma_rx->h->Init.PeriphInc           = DMA_PINC_DISABLE;
    mci->dma_rx->h->Init.MemInc              = DMA_MINC_ENABLE;
    mci->dma_rx->h->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    mci->dma_rx->h->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    mci->dma_rx->h->Init.Mode                = DMA_PFCTRL;
    mci->dma_rx->h->Init.Priority            = mci->dma_rx->priority;
    mci->dma_rx->h->Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    mci->dma_rx->h->Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    mci->dma_rx->h->Init.MemBurst            = DMA_MBURST_INC4;
    mci->dma_rx->h->Init.PeriphBurst         = DMA_PBURST_INC4;

    if (HAL_DMA_Init(mci->dma_rx->h) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }

    /* Configure DMA transmit stream */
    mci->dma_tx->h->Instance                 = mci->dma_tx->stream;
    mci->dma_tx->h->Init.Channel             = mci->dma_tx->channel;
    mci->dma_tx->h->Init.Direction           = DMA_MEMORY_TO_PERIPH;
    mci->dma_tx->h->Init.PeriphInc           = DMA_PINC_DISABLE;
    mci->dma_tx->h->Init.MemInc              = DMA_MINC_ENABLE;
    mci->dma_tx->h->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    mci->dma_tx->h->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    mci->dma_tx->h->Init.Mode                = DMA_PFCTRL;
    mci->dma_tx->h->Init.Priority            = mci->dma_tx->priority;
    mci->dma_tx->h->Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    mci->dma_tx->h->Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    mci->dma_tx->h->Init.MemBurst            = DMA_MBURST_INC4;
    mci->dma_tx->h->Init.PeriphBurst         = DMA_PBURST_INC4;

    if (HAL_DMA_Init(mci->dma_tx->h) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  #else
    mci->h->Instance = mci->reg;
  #endif

  if (mci->io_cd != NULL) {
    /* Configure Card Detect pin */
    Enable_GPIO_Clock (mci->io_cd->port);

    GPIO_InitStruct.Pin   = mci->io_cd->pin;
    GPIO_InitStruct.Pull  = mci->io_cd->pull;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(mci->io_cd->port, &GPIO_InitStruct);
  }
  if (mci->io_wp != NULL) {
    /* Configure Write Protect pin */
    Enable_GPIO_Clock (mci->io_wp->port);

    GPIO_InitStruct.Pin   = mci->io_wp->pin;
    GPIO_InitStruct.Pull  = mci->io_wp->pull;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(mci->io_wp->port, &GPIO_InitStruct);
  }

  /* Set DMA callback function */
  mci->dma_rx->h->XferCpltCallback = mci->dma_rx->cb_complete;

  /* Clear control structure */
  memset (mci->info, 0, sizeof (MCI_INFO));

  mci->info->cb_event = cb_event;
  mci->info->flags    = MCI_INIT;

  if (mci->reg == SDMMC1) {
    #if (MCI0_BUS_MODE_HS != 0U)
      mci->info->flags |= MCI_BUS_HS;
    #endif
  } else {
    #if (MCI1_BUS_MODE_HS != 0U)
      mci->info->flags |= MCI_BUS_HS;
    #endif
  }

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t Uninitialize (void)
  \brief         De-initialize Memory Card Interface.
  \return        \ref execution_status
*/
static int32_t Uninitialize (MCI_RESOURCES *mci) {
#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
  uint32_t i;
#endif

  mci->info->flags = 0U;

  #if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
    mci->h->Instance = NULL;
  #endif

  #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
    /* Unconfigure pins */
    for (i = 0; i < mci->io_cnt; i++) {
      HAL_GPIO_DeInit (mci->io[i].port, mci->io[i].pin);
      Enable_GPIO_Clock (mci->io[i].port);
    }
  #endif

  if (mci->io_cd != NULL) {
    /* Unconfigure CD (Card Detect) Pin */
    #if (defined (USE_STM32756G_EVAL) || defined (USE_STM32F769I_EVAL))
      BSP_IO_ConfigPin(IO_PIN_15, IO_MODE_OFF);
    #else
      HAL_GPIO_DeInit (mci->io_cd->port, mci->io_cd->pin);
    #endif
  }
  
  if (mci->io_wp != NULL) {
    /* Unconfigure WP (Write Protect) Pin */
    HAL_GPIO_DeInit (mci->io_wp->port, mci->io_wp->pin);
  }

  return ARM_DRIVER_OK;
}


/**
  \fn            int32_t PowerControl (ARM_POWER_STATE state)
  \brief         Control Memory Card Interface Power.
  \param[in]     state   Power state \ref ARM_POWER_STATE
  \return        \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state, MCI_RESOURCES *mci) {
  int32_t status;

  status = ARM_DRIVER_OK;

  switch (state) {
    case ARM_POWER_OFF:
      /* Reset/Dereset SDMMC peripheral */
      Control_SDMMC_Reset (mci->reg);

      #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
        /* Disable SDIO interrupts in NVIC */
        HAL_NVIC_DisableIRQ (mci->irq_num);

        /* Disable DMA streams interrupts in NVIC */
        HAL_NVIC_DisableIRQ (mci->dma_rx->irq_num);
        HAL_NVIC_DisableIRQ (mci->dma_tx->irq_num);

        /* Disable DMA streams */
        if (HAL_DMA_DeInit (mci->dma_rx->h) != HAL_OK) {
          status = ARM_DRIVER_ERROR;
        }
        if (HAL_DMA_DeInit (mci->dma_tx->h) != HAL_OK) {
          status = ARM_DRIVER_ERROR;
        }

        /* SDMMC peripheral clock disable */
        Control_SDMMC_Clock (mci->reg, 0);
      #else
        HAL_SD_MspDeInit (mci->h);
      #endif

      /* Clear status */
      mci->info->status.command_active   = 0U;
      mci->info->status.command_timeout  = 0U;
      mci->info->status.command_error    = 0U;
      mci->info->status.transfer_active  = 0U;
      mci->info->status.transfer_timeout = 0U;
      mci->info->status.transfer_error   = 0U;
      mci->info->status.sdio_interrupt   = 0U;
      mci->info->status.ccs              = 0U;

      mci->info->flags &= ~MCI_POWER;
      break;

    case ARM_POWER_FULL:
      if ((mci->info->flags & MCI_INIT)  == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((mci->info->flags & MCI_POWER) != 0U) {
        return ARM_DRIVER_OK;
      }
      #if defined(RTE_DEVICE_FRAMEWORK_CUBE_MX)
        HAL_SD_MspInit (mci->h);
      #else
        /* Enable SDMMC peripheral clock */
        Control_SDMMC_Clock (mci->reg, 1);
      #endif

      /* Clear response and transfer variables */
      mci->info->response = NULL;
      mci->info->xfer.cnt = 0U;

      /* Enable SDMMC peripheral interrupts */
      mci->reg->MASK = SDMMC_MASK_DATAENDIE  |
                       SDMMC_MASK_CMDSENTIE  |
                       SDMMC_MASK_CMDRENDIE  |
                       SDMMC_MASK_DTIMEOUTIE |
                       SDMMC_MASK_CTIMEOUTIE |
                       SDMMC_MASK_DCRCFAILIE |
                       SDMMC_MASK_CCRCFAILIE ;

      /* Set max data timeout */
      mci->reg->DTIMER = 0xFFFFFFFF;

      /* Enable clock to the card (SDIO_CK) */
      mci->reg->POWER = SDMMC_POWER_PWRCTRL_1 | SDMMC_POWER_PWRCTRL_0;

      #if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
        /* Enable DMA streams interrupts in NVIC */
        HAL_NVIC_EnableIRQ (mci->dma_rx->irq_num);
        HAL_NVIC_EnableIRQ (mci->dma_tx->irq_num);

        HAL_NVIC_ClearPendingIRQ (mci->irq_num);
        HAL_NVIC_EnableIRQ       (mci->irq_num);
      #endif

      mci->info->flags |= MCI_POWER;
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
static int32_t CardPower (uint32_t voltage, MCI_RESOURCES *mci) {

  if ((mci->info->flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}


/**
  \fn            int32_t ReadCD (void)
  \brief         Read Card Detect (CD) state.
  \return        1:card detected, 0:card not detected, or error
*/
static int32_t ReadCD (MCI_RESOURCES *mci) {

  if ((mci->info->flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  /* Read CD (Card Detect) Pin */
  #if (MCI0_CD_PIN != 0U) || (MCI1_CD_PIN != 0U)
    #if (defined (USE_STM32756G_EVAL) || defined (USE_STM32F769I_EVAL))
      if (BSP_IO_ReadPin (IO_PIN_15) == BSP_IO_PIN_RESET) {
        /* Card Detect switch is active */
        return (1);
      }
    #else
      if (mci->io_cd != NULL) {
        /* Note: io->af holds MemoryCard_CD_Pin_Active definiton */
        if (HAL_GPIO_ReadPin (mci->io_cd->port, mci->io_cd->pin) == mci->io_cd->af) {
          /* Card Detect switch is active */
          return (1);
        }
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
static int32_t ReadWP (MCI_RESOURCES *mci) {

  if ((mci->info->flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  /* Read WP (Write Protect) Pin */
  #if (MCI0_WP_PIN != 0U) || (MCI1_WP_PIN != 0U)
    if (mci->io_wp != NULL) {
      /* Note: io->af holds MemoryCard_WP_Pin_Active definiton */
      if (HAL_GPIO_ReadPin (mci->io_wp->port, mci->io_wp->pin) == mci->io_wp->af) {
        /* Write protect switch is active */
        return (1);
      }
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
static int32_t SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response, MCI_RESOURCES *mci) {
  uint32_t i, clkcr;

  if (((flags & MCI_RESPONSE_EXPECTED_Msk) != 0U) && (response == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((mci->info->flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (mci->info->status.command_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  mci->info->status.command_active   = 1U;
  mci->info->status.command_timeout  = 0U;
  mci->info->status.command_error    = 0U;
  mci->info->status.transfer_timeout = 0U;
  mci->info->status.transfer_error   = 0U;
  mci->info->status.ccs              = 0U;

  if (flags & ARM_MCI_CARD_INITIALIZE) {
    clkcr = mci->reg->CLKCR;

    if (((clkcr & SDMMC_CLKCR_CLKEN) == 0) || ((clkcr & SDMMC_CLKCR_PWRSAV) != 0)) {
      mci->reg->CLKCR = (mci->reg->CLKCR & ~SDMMC_CLKCR_PWRSAV) | SDMMC_CLKCR_CLKEN;

      i = HAL_RCC_GetHCLKFreq();
      for (i = (i/5000000U)*1000U; i; i--) {
        ; /* Wait for approximate 1000us */
      }
      mci->reg->CLKCR = clkcr;
    }
  }

  /* Set command register value */
  cmd = SDMMC_CMD_CPSMEN | (cmd & 0xFFU);

  mci->info->response = response;
  mci->info->flags   &= ~(MCI_RESP_CRC | MCI_RESP_LONG);

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
      mci->info->flags |= MCI_RESP_LONG;
      /* Long response expected (wait CMDREND or CCRCFAIL) */
      cmd |= SDMMC_CMD_WAITRESP_1 | SDMMC_CMD_WAITRESP_0;
      break;

    default:
      return ARM_DRIVER_ERROR;
  }
  if (flags & ARM_MCI_RESPONSE_CRC) {
    mci->info->flags |= MCI_RESP_CRC;
  }
  if (flags & ARM_MCI_TRANSFER_DATA) {
    mci->info->flags |= MCI_DATA_XFER;
  }

  /* Clear all interrupt flags */
  mci->reg->ICR = SDMMC_ICR_BIT_Msk;

  /* Send the command */
  mci->reg->ARG = arg;
  mci->reg->CMD = cmd;

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
static int32_t SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode, MCI_RESOURCES *mci) {
  uint32_t sz, cnt, dctrl;

  if ((data == NULL) || (block_count == 0U) || (block_size == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }

  if ((mci->info->flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (mci->info->status.transfer_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  mci->info->xfer.buf = data;
  mci->info->xfer.cnt = block_count * block_size;

  cnt = mci->info->xfer.cnt;
  if (cnt > 0xFFFFU) {
    cnt = 0xFFFFU;
  }

  mci->info->xfer.cnt -= cnt;
  mci->info->xfer.buf += cnt;

  dctrl = 0U;

  if ((mode & ARM_MCI_TRANSFER_WRITE) == 0) {
    /* Direction: From card to controller */
    mci->info->flags |= MCI_DATA_READ;
    dctrl |= SDMMC_DCTRL_DTDIR;
  }
  else {
    mci->info->flags &= ~MCI_DATA_READ;
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
    if (HAL_DMA_Start_IT (mci->dma_tx->h, (uint32_t)data, (uint32_t)&(mci->reg->FIFO), cnt) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }
  else {
    /* Enable RX DMA stream */
    if (HAL_DMA_Start_IT (mci->dma_rx->h, (uint32_t)&(mci->reg->FIFO), (uint32_t)data, cnt) != HAL_OK) {
      return ARM_DRIVER_ERROR;
    }
  }

  mci->info->dlen   = cnt;
  mci->info->dctrl  = dctrl | (sz << 4) | SDMMC_DCTRL_DMAEN;

  return (ARM_DRIVER_OK);
}


/**
  \fn            int32_t AbortTransfer (void)
  \brief         Abort current read/write data transfer.
  \return        \ref execution_status
*/
static int32_t AbortTransfer (MCI_RESOURCES *mci) {
  int32_t  status;
  uint32_t mask;

  if ((mci->info->flags & MCI_SETUP) == 0U) { return ARM_DRIVER_ERROR; }

  status = ARM_DRIVER_OK;

  /* Disable SDIO interrupts */
  mask = mci->reg->MASK;
  mci->reg->MASK = 0U;

  /* Disable DMA and clear data transfer bit */
  mci->reg->DCTRL &= ~(SDMMC_DCTRL_DMAEN | SDMMC_DCTRL_DTEN);

  if (HAL_DMA_Abort (mci->dma_rx->h) != HAL_OK) {
    status = ARM_DRIVER_ERROR;
  }
  if (HAL_DMA_Abort (mci->dma_tx->h) != HAL_OK) {
    status = ARM_DRIVER_ERROR;
  }

  /* Clear SDIO FIFO */
  while (mci->reg->FIFOCNT) {
    mci->reg->FIFO;
  }

  mci->info->status.command_active  = 0U;
  mci->info->status.transfer_active = 0U;
  mci->info->status.sdio_interrupt  = 0U;
  mci->info->status.ccs             = 0U;

  /* Clear pending SDIO interrupts */
  mci->reg->ICR = SDMMC_ICR_BIT_Msk;

  /* Enable SDIO interrupts */
  mci->reg->MASK = mask;

  return status;
}


/**
  \fn            int32_t Control (uint32_t control, uint32_t arg)
  \brief         Control MCI Interface.
  \param[in]     control  Operation
  \param[in]     arg      Argument of operation (optional)
  \return        \ref execution_status
*/
static int32_t Control (uint32_t control, uint32_t arg, MCI_RESOURCES *mci) {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t val, clkdiv, bps;

  if ((mci->info->flags & MCI_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  switch (control) {
    case ARM_MCI_BUS_SPEED:
      /* Determine clock divider and set bus speed */
      bps = arg;

      if ((bps < SDMMCCLK) || ((mci->info->flags & MCI_BUS_HS) == 0U)) {
        /* bps = SDIOCLK / (clkdiv + 2) */
        clkdiv = (SDMMCCLK + bps - 1U) / bps;

        if (clkdiv < 2) { clkdiv  = 0U; }
        else            { clkdiv -= 2U; }

        if (clkdiv > SDMMC_CLKCR_CLKDIV) {
          clkdiv  = SDMMC_CLKCR_CLKDIV;
        }

        mci->reg->CLKCR = (mci->reg->CLKCR & ~(SDMMC_CLKCR_CLKDIV | SDMMC_CLKCR_BYPASS)) |
                           SDMMC_CLKCR_CLKEN | clkdiv;
        bps = SDMMCCLK / (clkdiv + 2U);
      }
      else {
        /* Max output clock is SDIOCLK */
        mci->reg->CLKCR |= SDMMC_CLKCR_BYPASS | SDMMC_CLKCR_PWRSAV | SDMMC_CLKCR_CLKEN;

        bps = SDMMCCLK;
      }

      for (val = (SDMMCCLK/5000000U)*20U; val; val--) {
        ; /* Wait a bit to get stable clock */
      }

      /* Bus speed configured */
      mci->info->flags |= MCI_SETUP;
      return ((int32_t)bps);

    case ARM_MCI_BUS_SPEED_MODE:
      switch (arg) {
        case ARM_MCI_BUS_DEFAULT_SPEED:
          /* Speed mode up to 25MHz */
          mci->reg->CLKCR &= ~SDMMC_CLKCR_NEGEDGE;
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

      GPIO_InitStruct.Pin = (mci->reg == SDMMC1) ? (GPIO_PIN_2) : (GPIO_PIN_7);
      GPIO_InitStruct.Mode = val;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

      if (mci->reg == SDMMC1) {
        GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
      }
      #if defined(SDMMC2)
      if (mci->reg == SDMMC2) {
        GPIO_InitStruct.Alternate = GPIO_AF11_SDMMC2;
      }
      #endif

      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
      break;

    case ARM_MCI_BUS_DATA_WIDTH:
      switch (arg) {
        case ARM_MCI_BUS_DATA_WIDTH_1:
          mci->reg->CLKCR &= ~SDMMC_CLKCR_WIDBUS;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_4:
          mci->reg->CLKCR = (mci->reg->CLKCR & ~SDMMC_CLKCR_WIDBUS) | SDMMC_CLKCR_WIDBUS_0;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_8:
          mci->reg->CLKCR = (mci->reg->CLKCR & ~SDMMC_CLKCR_WIDBUS) | SDMMC_CLKCR_WIDBUS_1;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_CONTROL_CLOCK_IDLE:
      if (arg) {
        /* Clock generation enabled when idle */
        mci->reg->CLKCR &= ~SDMMC_CLKCR_PWRSAV;
      }
      else {
        /* Clock generation disabled when idle */
        mci->reg->CLKCR |= SDMMC_CLKCR_PWRSAV;
      }
      break;

    case ARM_MCI_DATA_TIMEOUT:
      mci->reg->DTIMER = arg;
      break;

    case ARM_MCI_MONITOR_SDIO_INTERRUPT:
      mci->info->status.sdio_interrupt = 0U;
      mci->reg->MASK |= SDMMC_MASK_SDIOITIE;
      break;

    case ARM_MCI_CONTROL_READ_WAIT:
      if (arg) {
        /* Assert read wait */
        mci->info->flags |= MCI_READ_WAIT;
      }
      else {
        /* Clear read wait */
        mci->info->flags &= ~MCI_READ_WAIT;
        mci->reg->DCTRL &= ~SDMMC_DCTRL_RWSTOP;
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
static ARM_MCI_STATUS GetStatus (MCI_RESOURCES *mci) {
  return mci->info->status;
}


/* SDMMC interrupt handler */
void SDMMC_IRQHandler (MCI_RESOURCES *mci) {
  uint32_t sta, icr, event, mask;

  event = 0U;
  icr   = 0U;

  /* Read SDMMC interrupt status */
  sta = mci->reg->STA;

  if (sta & SDMMC_STA_ERR_BIT_Msk) {
    /* Check error interrupts */
    if (sta & SDMMC_STA_CCRCFAIL) {
      icr |= SDMMC_ICR_CCRCFAILC;
      /* Command response CRC check failed */
      if (mci->info->flags & MCI_RESP_CRC) {
        mci->info->status.command_error = 1U;

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
      mci->info->status.transfer_error = 1U;

      event |= ARM_MCI_EVENT_TRANSFER_ERROR;
    }
    if (sta & SDMMC_STA_CTIMEOUT) {
      icr |= SDMMC_ICR_CTIMEOUTC;
      /* Command response timeout */
      mci->info->status.command_timeout = 1U;

      event |= ARM_MCI_EVENT_COMMAND_TIMEOUT;
    }
    if (sta & SDMMC_STA_DTIMEOUT) {
      icr |= SDMMC_ICR_DTIMEOUTC;
      /* Data timeout */
      mci->info->status.transfer_timeout = 1U;

      event |= ARM_MCI_EVENT_TRANSFER_TIMEOUT;
    }
  }

  if (sta & SDMMC_STA_CMDREND) {
    icr |= SDMMC_ICR_CMDRENDC;
    /* Command response received */
    event |= ARM_MCI_EVENT_COMMAND_COMPLETE;

    if (mci->info->response) {
      /* Read response registers */
      if (mci->info->flags & MCI_RESP_LONG) {
        mci->info->response[0] = mci->reg->RESP4;
        mci->info->response[1] = mci->reg->RESP3;
        mci->info->response[2] = mci->reg->RESP2;
        mci->info->response[3] = mci->reg->RESP1;
      }
      else {
        mci->info->response[0] = mci->reg->RESP1;
      }
    }
    if (mci->info->flags & MCI_DATA_XFER) {
      mci->info->flags &= ~MCI_DATA_XFER;

      if (mci->info->flags & MCI_READ_WAIT) {
        mci->info->dctrl |= SDMMC_DCTRL_RWSTART;
      }

      /* Start data transfer */
      mci->reg->DLEN  = mci->info->dlen;
      mci->reg->DCTRL = mci->info->dctrl | SDMMC_DCTRL_DTEN;

      mci->info->status.transfer_active = 1U;
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
    if ((mci->info->flags & MCI_DATA_READ) == 0) {
    /* Write transfer */
      mci->reg->MASK |= SDMMC_MASK_DBCKENDIE;
    }
  }
  if (sta & SDMMC_STA_DBCKEND) {
    icr |= SDMMC_ICR_DBCKENDC;
    /* Data block sent/received (CRC check passed) */
    if ((mci->info->flags & MCI_DATA_READ) == 0) {
      /* Write transfer */
      if (mci->info->xfer.cnt == 0) {
        event |= ARM_MCI_EVENT_TRANSFER_COMPLETE;
      }
    }
    mci->reg->MASK &= ~SDMMC_MASK_DBCKENDIE;
  }
  if (sta & SDMMC_STA_SDIOIT) {
    icr |= SDMMC_ICR_SDIOITC;
    /* Disable interrupt (must be re-enabled using Control) */
    mci->reg->MASK &= SDMMC_MASK_SDIOITIE;

    event |= ARM_MCI_EVENT_SDIO_INTERRUPT;
  }

  /* Clear processed interrupts */
  mci->reg->ICR = icr;

  if (event) {
    /* Check for transfer events */
    mask = ARM_MCI_EVENT_TRANSFER_ERROR   |
           ARM_MCI_EVENT_TRANSFER_TIMEOUT |
           ARM_MCI_EVENT_TRANSFER_COMPLETE;
    if (event & mask) {
      mci->info->status.transfer_active = 0U;

      if (mci->info->cb_event) {
        if (event & ARM_MCI_EVENT_TRANSFER_ERROR) {
          (mci->info->cb_event)(ARM_MCI_EVENT_TRANSFER_ERROR);
        }
        else if (event & ARM_MCI_EVENT_TRANSFER_TIMEOUT) {
          (mci->info->cb_event)(ARM_MCI_EVENT_TRANSFER_TIMEOUT);
        }
        else {
          (mci->info->cb_event)(ARM_MCI_EVENT_TRANSFER_COMPLETE);
        }
      }
    }
    /* Check for command events */
    mask = ARM_MCI_EVENT_COMMAND_ERROR   |
           ARM_MCI_EVENT_COMMAND_TIMEOUT |
           ARM_MCI_EVENT_COMMAND_COMPLETE;
    if (event & mask) {
      mci->info->status.command_active = 0U;

      if (mci->info->cb_event) {
        if (event & ARM_MCI_EVENT_COMMAND_ERROR) {
          (mci->info->cb_event)(ARM_MCI_EVENT_COMMAND_ERROR);
        }
        else if (event & ARM_MCI_EVENT_COMMAND_TIMEOUT) {
          (mci->info->cb_event)(ARM_MCI_EVENT_COMMAND_TIMEOUT);
        }
        else {
          (mci->info->cb_event)(ARM_MCI_EVENT_COMMAND_COMPLETE);
        }
      }
    }
    /* Check for SDIO INT event */
    if (event & ARM_MCI_EVENT_SDIO_INTERRUPT) {
      mci->info->status.sdio_interrupt = 1U;

      if (mci->info->cb_event) {
        (mci->info->cb_event)(ARM_MCI_EVENT_SDIO_INTERRUPT);
      }
    }
  }
}


/* Rx DMA Callback */
void RX_DMA_Complete (DMA_HandleTypeDef *hdma, MCI_RESOURCES *mci) {

  mci->info->status.transfer_active = 0U;

  if (mci->info->cb_event) {
    (mci->info->cb_event)(ARM_MCI_EVENT_TRANSFER_COMPLETE);
  }
}


#if defined (MX_SDMMC1)
/* MCI0 Driver wrapper functions */
static ARM_MCI_CAPABILITIES MCI0_GetCapabilities (void) {
  return GetCapabilities (&MCI0_Resources);
}
static int32_t MCI0_Initialize (ARM_MCI_SignalEvent_t cb_event) {
  return Initialize (cb_event, &MCI0_Resources);
}
static int32_t MCI0_Uninitialize (void) {
  return Uninitialize (&MCI0_Resources);
}
static int32_t MCI0_PowerControl (ARM_POWER_STATE state) {
  return PowerControl (state, &MCI0_Resources);
}
static int32_t MCI0_CardPower (uint32_t voltage) {
  return CardPower (voltage, &MCI0_Resources);
}
static int32_t MCI0_ReadCD (void) {
  return ReadCD (&MCI0_Resources);
}
static int32_t MCI0_ReadWP (void) {
  return ReadWP (&MCI0_Resources);
}
static int32_t MCI0_SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response) {
  return SendCommand (cmd, arg, flags, response, &MCI0_Resources);
}
static int32_t MCI0_SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode) {
  return SetupTransfer (data, block_count, block_size, mode, &MCI0_Resources);
}
static int32_t MCI0_AbortTransfer (void) {
  return AbortTransfer (&MCI0_Resources);
}
static int32_t MCI0_Control (uint32_t control, uint32_t arg) {
  return Control (control, arg, &MCI0_Resources);
}
static ARM_MCI_STATUS MCI0_GetStatus (void) {
  return GetStatus (&MCI0_Resources);
}
void SDMMC1_IRQHandler (void) {
  SDMMC_IRQHandler (&MCI0_Resources);
}

#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/* RX DMA Stream IRQ Handler */
void MX_SDMMC1_RX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(MX_SDMMC1_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc1_rx);
}

/* TX DMA Stream IRQ Handler */
void MX_SDMMC1_TX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(MX_SDMMC1_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc1_tx);
}
#endif

void MCI0_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  RX_DMA_Complete (hdma, &MCI0_Resources);
}

/* MCI0 Driver Control Block */
ARM_DRIVER_MCI Driver_MCI0 = {
  GetVersion,
  MCI0_GetCapabilities,
  MCI0_Initialize,
  MCI0_Uninitialize,
  MCI0_PowerControl,
  MCI0_CardPower,
  MCI0_ReadCD,
  MCI0_ReadWP,
  MCI0_SendCommand,
  MCI0_SetupTransfer,
  MCI0_AbortTransfer,
  MCI0_Control,
  MCI0_GetStatus
};
#endif /* MX_MCI0 */


#if defined (MX_SDMMC2)
/* MCI1 Driver wrapper functions */
static ARM_MCI_CAPABILITIES MCI1_GetCapabilities (void) {
  return GetCapabilities (&MCI1_Resources);
}
static int32_t MCI1_Initialize (ARM_MCI_SignalEvent_t cb_event) {
  return Initialize (cb_event, &MCI1_Resources);
}
static int32_t MCI1_Uninitialize (void) {
  return Uninitialize (&MCI1_Resources);
}
static int32_t MCI1_PowerControl (ARM_POWER_STATE state) {
  return PowerControl (state, &MCI1_Resources);
}
static int32_t MCI1_CardPower (uint32_t voltage) {
  return CardPower (voltage, &MCI1_Resources);
}
static int32_t MCI1_ReadCD (void) {
  return ReadCD (&MCI1_Resources);
}
static int32_t MCI1_ReadWP (void) {
  return ReadWP (&MCI1_Resources);
}
static int32_t MCI1_SendCommand (uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response) {
  return SendCommand (cmd, arg, flags, response, &MCI1_Resources);
}
static int32_t MCI1_SetupTransfer (uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode) {
  return SetupTransfer (data, block_count, block_size, mode, &MCI1_Resources);
}
static int32_t MCI1_AbortTransfer (void) {
  return AbortTransfer (&MCI1_Resources);
}
static int32_t MCI1_Control (uint32_t control, uint32_t arg) {
  return Control (control, arg, &MCI1_Resources);
}
static ARM_MCI_STATUS MCI1_GetStatus (void) {
  return GetStatus (&MCI1_Resources);
}
void SDMMC2_IRQHandler (void) {
  SDMMC_IRQHandler (&MCI1_Resources);
}

#if defined(RTE_DEVICE_FRAMEWORK_CLASSIC)
/* RX DMA Stream IRQ Handler */
void MX_SDMMC2_RX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(MX_SDMMC2_RX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc2_rx);
}

/* TX DMA Stream IRQ Handler */
void MX_SDMMC2_TX_DMA_Handler (void) {

  HAL_NVIC_ClearPendingIRQ(MX_SDMMC2_TX_DMA_IRQn);
  HAL_DMA_IRQHandler(&hdma_sdmmc2_tx);
}
#endif

void MCI1_RX_DMA_Complete(DMA_HandleTypeDef *hdma) {
  RX_DMA_Complete (hdma, &MCI1_Resources);
}

/* MCI1 Driver Control Block */
ARM_DRIVER_MCI Driver_MCI1 = {
  GetVersion,
  MCI1_GetCapabilities,
  MCI1_Initialize,
  MCI1_Uninitialize,
  MCI1_PowerControl,
  MCI1_CardPower,
  MCI1_ReadCD,
  MCI1_ReadWP,
  MCI1_SendCommand,
  MCI1_SetupTransfer,
  MCI1_AbortTransfer,
  MCI1_Control,
  MCI1_GetStatus
};
#endif /* MX_MCI1 */

/*! \endcond */
