# CC1310 FreeRTOS simple radio 

Basic project demonstrating the usage of FreeRTOS and the radio on the Simplelink CC1310 mcu without TI RTOS/SDK's. It uses the GCC compiler to build. There is no need to install any TI SDK's, except what is needed to run CCS and debug with XDS110.

The radio is configured by default to work on the 868Mhz band, it's using the 5kbps Simplelink Long Range settings. It also implements only the basic functionality and it hardcode's a lot of things.

In order to use some other configuration, use SmartRF Studio to generate the commands and replace them in the project.

PS. There is no error checking for RX, meaning any invalid packet will be discarded.

This project is distributed under "FreeRTOS Open Source License". Full license text is available on the following link: http://www.freertos.org/a00114.html
The FreeRTOS version used is 10.0.1 The TI SDK version is not important since we are using the registers directly. The board used is LAUNCHXL-CC1310.