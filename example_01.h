/*
 * example_01.h
 *
 *  Created on: Jan 15, 2019
 *      Author: razvanm
 */

#ifndef EXAMPLE_01_H_
#define EXAMPLE_01_H_

#include <stdint.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h>
#include <driverlib/aon_batmon.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <radio/radio.h>
#include "board.h"
#include "utils.h"

// Buffer to hold the string printf
uint8_t         g_printf_buffer[128] = {0};

//***************************************************************************
//***************************************************************************
//                                RX
//***************************************************************************
//***************************************************************************
void task_rx_handler(void* params){
    UNUSED(params);

    // Map this structure over the rfc_dataEntry so we have easy access to what we need
    struct __attribute__((packed)) {
        rfc_dataEntry_t entry;
        uint8_t         length;
        uint8_t         payload[RX_MSG_SIZE-1/*length size*/];
    }  *data;

    IOCPinTypeGpioOutput(LED_GREEN);
    UART_write("Starting RX task.\r\n");

    while(1){

        // Wait for RX queue to have something
        if (xQueueReceive(xRfRXQueue, &data, portMAX_DELAY) == pdFALSE){
            // Nothing in the queue even after MAX_DELAY
            //  go an start the wait again
            continue;
        }

        // Show some indication of a received packet
        GPIO_toggleDio(LED_GREEN);

        // We have the packet in our hands, print the content
        uint8_t idx = 0;
        while(idx < data->length && idx < RX_MSG_SIZE-1)
            UARTCharPut(UART_DEV, data->payload[idx++]);
        UARTCharPut(UART_DEV, '\r');
        UARTCharPut(UART_DEV, '\n');

        // Mark the rfc_dataEntry as pending so it can be reused
        data->entry.status = DATA_ENTRY_PENDING;
    }
}
//***************************************************************************
//***************************************************************************
//                                TX
//***************************************************************************
//***************************************************************************
void task_tx_handler(void* params){
    UNUSED(params);

    IOCPinTypeGpioOutput(LED_RED);
    UART_write("Starting TX task.\r\n");

    uint32_t packet = 0;
    while(1){
        // Create the payload
        uint8_t totalChars = simple_sprintf((char*)&g_printf_buffer[0], "Packet: %d. Temperature: %d deg C.", packet++, AONBatMonTemperatureGetDegC());
        // Send the data
        PHY_send(g_printf_buffer, totalChars);
        // Some indication that something has happened
        GPIO_toggleDio(LED_RED);
        // Wait for 1 second
        vTaskDelay(1000);
    }
}


#endif /* EXAMPLE_01_H_ */
