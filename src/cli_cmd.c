/*
 * cli_cmd.c
 *
 *  Created on: Sep 19, 2015
 *      Author: dsolano
 */

#include <cli.h>
#include <cli_cmd.h>
#include <debug_frmwrk.h>

Bool cli_new_char;	//  variable for Cli ISR
char new_cli_data;	// Char received thru UART3



void UART_IRQHandler(void)
{
	new_cli_data = xfunc_in();
	cli_new_char = TRUE;
}


// Create CLI LED command structures
CLI_CMD_CREATE(cli_cmd_help,   "help",   0, 1, "<cmd(s) starts with...>", "Display list of commands with help. Optionally, the list can be reduced.")
CLI_CMD_CREATE(cli_cmd_print_buffer,   "buffer",   1, 2, "<cmd(s) starts with...>", "Print buffer content.")

// Declare CLI command list and add commands and groups
CLI_CMD_LIST_CREATE()
    CLI_CMD_ADD  (cli_cmd_help,   cli_cmd_help_fn)
	CLI_CMD_ADD(cli_cmd_print_buffer , cli_util_disp_buf)
CLI_CMD_LIST_END()


void cli_cmd_init(void)
{
	LPC_USART->IER = IER_RBR;	//Enable UART Module interrupt when Rx register gets data
	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);	/* Enable System Interrupt for UART channel */
	xdev_in(UARTGetChar);
	xdev_out(UARTPutChar);
	cli_init((const char*)"LPCXpresso Client DEMO - LPC1115\n\r");
}

void vCLI(void)
{
	if (cli_new_char)
	{
			cli_on_rx_char(new_cli_data);
			cli_new_char = FALSE;
	}
}


