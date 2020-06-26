/*
 * nrf24_protocol.h
 *
 *  Created on: Oct 4, 2016
 *      Author: dsolano
 */

#ifndef INC_NRF24_PROTOCOL_H_
#define INC_NRF24_PROTOCOL_H_

#include <stdint.h>
#include <lpc_types.h>
#include <LPC11xx.h>

/*
 * Protocol Prototype Functions
 */
void nrf24_Start_Protocol(void);
void nrf24_protocol_Print_Details(void);
void nrf24_ChannelScanner(void);
void nrf24_Receive(char* Buf, int len);
void nrf24_Send(char* Buf, int len);
Bool nrf24_IsLinkUP(void);

#endif /* INC_NRF24_PROTOCOL_H_ */
