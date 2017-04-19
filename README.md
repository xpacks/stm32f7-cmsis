# STM32F7 CMSIS

This project, available from [GitHub](https://github.com/xpacks/stm32f7-cmsis),
includes the STM32F7 CMSIS files.

## Version

* v2.9.0

## Documentation

The latest CMSIS documentation is available from
[keil.com](http://www.keil.com/cmsis).

The list of latest packs is available from [keil.com](https://www.keil.com/dd2/pack/).

## Original files

The original files are available from the `originals` branch.

These files were extracted from `Keil.STM32F7xx_DFP.2.9.0.pack`.

To save space, the following folders/files were removed:

* all non-portable *.exe files
* \_htmresc
* CMSIS/Flash/
* Debug/
* Documentation/
* Drivers/BSP/
* Drivers/CMSIS/CMSIS?END*.*
* Drivers/CMSIS/index.html
* Drivers/CMSIS/README.txt
* Drivers/CMSIS/Documentation
* Drivers/CMSIS/DSP_Lib/
* Drivers/CMSIS/Include/
* Drivers/CMSIS/Lib/
* Drivers/CMSIS/RTOS/
* Drivers/STM32F?xx_HAL_Driver/
* Flash/
* MDK/
* Middlewares/
* Projects/
* Utilities/
* package.xml

## Changes

The actual files used by the package are in the `xpack` branch.

Most of the files should be unchanged.

## Vectors

The `vectors_*.c` files were created using the ARM assembly files.

```
$ bash scripts/generate-vectors.sh
startup_stm32f745xx.s -> vectors_stm32f745xx.c
startup_stm32f746xx.s -> vectors_stm32f746xx.c
startup_stm32f756xx.s -> vectors_stm32f756xx.c
```

## Tests

```
export PATH=/usr/local/gcc-arm-none-eabi-5_2-2015q4/bin:$PATH
bash ../../../scripts/run-tests.sh
```
