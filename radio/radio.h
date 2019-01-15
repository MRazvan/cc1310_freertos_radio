/*
 * radio.h
 *
 *  Created on: Jan 7, 2019
 *      Author: razvanm
 */

#ifndef RADIO_H_
#define RADIO_H_

#include <stdint.h>
#include <driverlib/rf_data_entry.h>
#include <driverlib/rf_prop_cmd.h>
#include <FreeRTOS.h>
#include <queue.h>

#define PHY_OK                  0x01
#define PHY_ERROR               0x02
#define RX_MSG_COUNT            4
#define RX_MSG_SIZE             240
#define RX_ENTRY_SIZE           (sizeof(rfc_dataEntryGeneral_t) + RX_MSG_SIZE)

uint_fast8_t     PHY_open(QueueHandle_t);
uint_fast8_t     PHY_send(uint8_t*, uint8_t);
uint_fast8_t     PHY_close(void);

#endif /* RADIO_H_ */
