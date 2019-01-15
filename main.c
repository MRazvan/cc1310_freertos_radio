#include <stdint.h>
#include <driverlib/setup.h>
#include <driverlib/prcm.h>
#include <driverlib/uart.h>
#include <driverlib/interrupt.h>
#include <driverlib/aon_batmon.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <radio/radio.h>
#include "board.h"
#include "utils.h"



#define TX_NODE
//#define EXAMPLE_1




// Queue to hold pointers to the payload entry. We don't want to copy the data so we will use pointers to the rfc_dataEntry structures
QueueHandle_t   xRfRXQueue;

#ifdef EXAMPLE_1
    #include "example_01.h"
#else
    #include "example_02.h"
#endif

#pragma GCC diagnostic ignored "-Wmain"
__attribute__((noreturn))
void main(void)
{
    SetupTrimDevice();
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON){}
    // Enable clock for GPIO / UART
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while(!PRCMLoadGet()){}

    // Initialize UART
    IntMasterDisable();
    IOCPinTypeUart(UART_DEV, UART_RX, UART_TX, UART_CTS, UART_RTS);
    UARTFIFODisable(UART_DEV);
    UARTIntDisable(UART_DEV, UART_INT_OE | UART_INT_BE | UART_INT_PE |
                                           UART_INT_FE | UART_INT_RT | UART_INT_TX |
                                           UART_INT_RX | UART_INT_CTS);

    UARTConfigSetExpClk(UART_DEV, F_CPU, UART_BAUD, UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8);
    UARTEnable(UART_DEV);
    IntMasterEnable();

    // Create our RX handler task
#ifdef TX_NODE
    xTaskCreate(&task_tx_handler, "TX", 128, NULL, 0xFF, NULL);
#else
    xTaskCreate(&task_rx_handler, "RX", 128, NULL, 0xFF, NULL);
#endif

    // Create the QUEUE used for passing RX data around
    // the queue will contain pointers to the rfc_dataEntry structures used by the radio RX command
    xRfRXQueue = xQueueCreate(RX_MSG_COUNT, sizeof(void*));

    // Open the Radio and set the rx queue
    PHY_open(xRfRXQueue);

    // Start the scheduler
    vTaskStartScheduler();

    while(true);
}
