/*
 * bluetooth.c
 *
 *  Created on: 5/1/2015
 *      Author: dsolano
 */

#include <bluetooth.h>
#include <timeout_delay.h>
#include <string.h>
#include <ring_buffer.h>
#include <debug_frmwrk.h>


#define BTHC06_IntEventHandler	UART_IRQHandler

RINGBUFF_T bt_rb;					// Ring Buffer definition for UART2
char bt_Buffer[BT_RINGBUFFER_SIZE]; // Ringbuffer handler data buffer definition
char bt_response_data_buff[12];
volatile char bt_Rx_Data;			// used for the interrupt
volatile Bool Data_Ready;			// indicate that the BT device has being properly initialize by the Init routine
volatile Bool BT_AlreadyConn;		// Indicate that the system has already checked the flag  for BT_Connected so no repeat any actions

#define Ux		LPC_UART					// floating usart pointer
BT_CMD_t *Lclbtx;

static char*  parse_hc06_command(BT_CMD_t* bt);

static void bt_send_data_frame(char* frame, int len){
	int c;
	char* p = frame;
	while (c < len){
		UARTPutChar(p[c++]);
	}
}
void BTHC06_IntEventHandler(void)
{
	__disable_irq();
	{
		bt_Rx_Data = UARTGetChar();
		if (bt_Rx_Data == 'K') {
			Lclbtx->cmd_result = TRUE;
			Lclbtx->bt_state = BT_AT_MODE;
		}
		if (bt_Rx_Data == '*') Lclbtx->bt_state = BT_CONN;
		if (bt_Rx_Data == '#') Lclbtx->bt_state = BT_AT_MODE;
		RingBuffer_Insert(&bt_rb, (const void*) &bt_Rx_Data);
		NVIC_ClearPendingIRQ(UART0_IRQn);
	}
	__enable_irq();
}


static void init_uart(BT_CMD_t* bt) // UART
{
	debug_frmwrk_init(bt->baudrate);
	NVIC_SetPriority(UART0_IRQn , 1);
	NVIC_EnableIRQ(UART0_IRQn);

	// Init ring buffer
	RingBuffer_Init( &bt_rb , bt_Buffer, sizeof(char) , BT_RINGBUFFER_SIZE );
	__enable_irq();
}

/*
 * Baud accept value between 1 and 12
 * be careful you can be with no communication
 * after applying this command. Remember that if you are using
 * UART you need to change Baud Rate too.
 */
static void bt_GetDataStruct(BT_CMD_t* bt, CMD_TYPE_t br)
{
	switch (br)
	{
		case B1200:
			bt->cmd = CMD_B1200_1;
			bt->cmd_len = CMD_LEN(CMD_B1200_1);
			bt->response = RESP_B1200_1;
			bt->response_len = RESP_B1200_1_LEN;
			bt->cmd_type = B1200;
			break;
		case B2400:
			bt->cmd = CMD_B2400_2;
			bt->cmd_len = CMD_LEN(CMD_B2400_2);
			bt->response = RESP_B2400_2;
			bt->response_len = RESP_B2400_2_LEN;
			bt->cmd_type = B2400;
			break;
		case B4800:
			bt->cmd = CMD_B4800_3;
			bt->cmd_len = CMD_LEN(CMD_B4800_3);
			bt->response = RESP_B4800_3;
			bt->response_len = RESP_B4800_3_LEN;
			bt->cmd_type = B4800;
			break;
		case B9600:
			bt->cmd = CMD_B9600_4;
			bt->cmd_len = CMD_LEN(CMD_B9600_4);
			bt->response = RESP_B9600_4;
			bt->response_len = RESP_B9600_4_LEN;
			bt->cmd_type = B9600;
			break;
		case B19200:
			bt->cmd = CMD_B19200_5;
			bt->cmd_len = CMD_LEN(CMD_B19200_5);
			bt->response = RESP_B19200_5;
			bt->response_len = RESP_B19200_5_LEN;
			bt->cmd_type = B19200;
			break;
		case B38400:
			bt->cmd = CMD_B38400_6;
			bt->cmd_len = CMD_LEN(CMD_B38400_6);
			bt->response = RESP_B38400_6;
			bt->response_len = RESP_B38400_6_LEN;
			break;
		case B57600:
			bt->cmd = CMD_B57600_7;
			bt->cmd_len = CMD_LEN(CMD_B57600_7);
			bt->response = RESP_B57600_7;
			bt->response_len = RESP_B57600_7_LEN;
			bt->cmd_type = B57600;
			break;
		case B115200:
			bt->cmd = CMD_B115200_8;
			bt->cmd_len = CMD_LEN(CMD_B115200_8);
			bt->response = RESP_B115200_8;
			bt->response_len = RESP_B115200_8_LEN;
			bt->cmd_type = B115200;
			break;
		case B230400:
			bt->cmd = CMD_B230400_9;
			bt->cmd_len = CMD_LEN(CMD_B230400_9);
			bt->response = RESP_B230400_9;
			bt->response_len = RESP_B230400_9_LEN;
			bt->cmd_type = B230400;
			break;
		case B460800:
			bt->cmd = CMD_B460800_A;
			bt->cmd_len = CMD_LEN(CMD_B1200_1);
			bt->response = RESP_B1200_1;
			bt->response_len = RESP_B1200_1_LEN;
			bt->cmd_type = B460800;
			break;
		case B921600:
			bt->cmd = CMD_B921600_B;
			bt->cmd_len = CMD_LEN(CMD_B921600_B);
			bt->response = RESP_B921600_B;
			bt->response_len = RESP_B921600_B_LEN;
			bt->cmd_type = B921600;
			break;
		case B1382400:
			bt->cmd = CMD_B1382400_C;
			bt->cmd_len = CMD_LEN(CMD_B1382400_C);
			bt->response = RESP_B1382400_C;
			bt->response_len = RESP_B1382400_C_LEN;
			bt->cmd_type = B1382400;
			break;
		case AT:
			bt->cmd = CMD_AT;
			bt->cmd_len = CMD_LEN(CMD_AT);
			bt->response = RESP_AT;
			bt->response_len = RESP_AT_LEN;
			bt->cmd_type = AT;
			break;
		case ATPIN:
			bt->cmd = CMD_PIN;
			bt->cmd_len = CMD_LEN(CMD_PIN);
			bt->response = RESP_PIN;
			bt->response_len = RESP_PIN_LEN;
			bt->cmd_type = ATPIN;
			break;
		case ATNAME:
			bt->cmd = CMD_NAME;
			bt->cmd_len = CMD_LEN(CMD_NAME);
			bt->response = RESP_NAME;
			bt->response_len = RESP_NAME_LEN;
			bt->cmd_type = ATNAME;
			break;
		case ATPN:
			bt->cmd = CMD_PARITY_NONE;
			bt->cmd_len = CMD_LEN(CMD_PARITY_NONE);
			bt->response = RESP_PARITY_NONE;
			bt->response_len = RESP_PARITY_NONE_LEN;
			bt->cmd_type = ATPN;
			break;
		case ATPO:
			bt->cmd = CMD_PARITY_ODD;
			bt->cmd_len = CMD_LEN(CMD_PARITY_ODD);
			bt->response = RESP_PARITY_ODD;
			bt->response_len = RESP_PARITY_ODD_LEN;
			bt->cmd_type = ATPO;
			break;
		case ATPE:
			bt->cmd = CMD_PARITY_EVEN;
			bt->cmd_len = CMD_LEN(CMD_PARITY_EVEN);
			bt->response = RESP_PARITY_EVEN;
			bt->response_len = RESP_PARITY_EVEN_LEN;
			bt->cmd_type = ATPE;
			break;
		case ATVER:
			bt->cmd = CMD_GET_VERSION;
			bt->cmd_len = CMD_LEN(CMD_GET_VERSION);
			bt->response = RESP_GET_VERSION;
			bt->response_len = RESP_GET_VERSION_LEN;
			bt->cmd_type = ATVER;
			break;
		default:
			bt->cmd = 0;
			bt->cmd_len = 0;
			bt->response = 0;
			bt->response_len = 0;
			break;
	}
}

static char*  parse_hc06_command(BT_CMD_t* bt){
	bt_send_data_frame(bt->cmd, bt->cmd_len);	// Send command frame to BT module
	timeout_start();							// start timer count for timeout
	while (!bt->cmd_result)					// wait for the character to arrive on UART port from BT module
	{										// in response to the command frame
		if (timeout_get_curr_value() >= 2000){
			timeout_stop();					// this is timeout 2 seconds or 2000ms
			return ("Timeout...");
		}
	}
	while (RingBuffer_GetCount(&bt_rb) < bt->response_len);			// wait for ringbuffer to get all chars from BT module
	RingBuffer_PopMult(&bt_rb, bt->data_buffer, bt->response_len);	// get all data out of ringbuffer
	if ( !memcmp (bt->data_buffer, (const char*)"OK", 2) ){								// compare the first 2 char from data buffer to OK
		bt->cmd_result = FALSE;
		return bt->data_buffer;
	}
	return 0;
}


//************************************************************
//			PUBLIC FUNCTION
//************************************************************
/*
 *BT need to be disconnected from host or data will pass thru
 */
void bt_init(BT_CMD_t* bt, int baudrate)
{
	Lclbtx = bt;
	bt->baudrate = baudrate;
	bt->data_buffer = bt_response_data_buff;
	timeout_timer_init(1000);
	init_uart(bt);
}


char* bt_Is_Alive(BT_CMD_t* bt){
	bt_GetDataStruct(bt, AT);
	return parse_hc06_command(bt);
}




BT_STATE_t bt_ModuleMonitor(BT_CMD_t* bt)
{
	return bt->bt_state;
}

