/*
 * example_01.h
 *
 *  Created on: Jan 15, 2019
 *      Author: razvanm
 */

#ifndef EXAMPLE_01_H_
#define EXAMPLE_01_H_

#include <stdint.h>
#include <driverlib/gpio.h>


#include <FreeRTOS.h>
#include <queue.h>

#include <radio/radio.h>
#include "board.h"
#include "utils.h"

//***************************************************************************
//***************************************************************************
//                                RX
//***************************************************************************
//***************************************************************************
typedef struct __attribute__((packed)) {
    uint8_t     green:1;
    uint8_t     red  :1;
    uint8_t          :6;
} stLedMask;

void task_rx_handler(void* params){
    UNUSED(params);

    // Map this structure over the rfc_dataEntry so we have easy access to what we need
    struct __attribute__((packed)) {
        rfc_dataEntry_t entry;
        uint8_t         length;
        stLedMask       leds;
    }  *data;

    IOCPinTypeGpioOutput(LED_GREEN);
    IOCPinTypeGpioOutput(LED_RED);
    while(1){

        // Wait for RX queue to have something
        if (xQueueReceive(xRfRXQueue, &data, portMAX_DELAY) == pdFALSE){
            // Nothing in the queue even after MAX_DELAY
            //  go an start the wait again
            continue;
        }
        // We have the packet in our hands, print the content
        if (data->leds.green){
            GPIO_toggleDio(LED_GREEN);
        }
        if (data->leds.red){
            GPIO_toggleDio(LED_RED);
        }
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

    IOCPinTypeGpioInput(BTN_1);
    IOCIOPortPullSet(BTN_1, IOC_IOPULL_UP);
    IOCPinTypeGpioInput(BTN_2);
    IOCIOPortPullSet(BTN_2, IOC_IOPULL_UP);
    IOCPinTypeGpioOutput(LED_GREEN);
    IOCPinTypeGpioOutput(LED_RED);

    stLedMask   masks;
    while(1){
        masks.green = GPIO_readDio(BTN_2) ^ 0x1;
        masks.red = GPIO_readDio(BTN_1) ^ 0x1;

        // If any button is pressed send the packet
        if (masks.green || masks.red){
            PHY_send((uint8_t*)&masks, sizeof(masks));
        }

        // Toggle our leds so we have some feedback
        if (masks.green)
            GPIO_setDio(LED_GREEN);
        else
            GPIO_clearDio(LED_GREEN);
        if (masks.red)
            GPIO_setDio(LED_RED);
        else
            GPIO_clearDio(LED_RED);
        // Delay for 10ms
        vTaskDelay(10);
    }
}


#endif /* EXAMPLE_01_H_ */
