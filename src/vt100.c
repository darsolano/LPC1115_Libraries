/* =============================================================================

    Copyright (c) 2012 Pieter Conradie [http://piconomic.co.za]
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met: 
    
    1. Redistributions of source code must retain the above copyright notice, 
       this list of conditions and the following disclaimer. 
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution. 
    3. Credit must appear prominently in all internet publications (including
       advertisements) of products or projects that use this software by
       including the following text and URL:
       "Uses Piconomic FW Library <http://piconomic.co.za>"
 
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
    POSSIBILITY OF SUCH DAMAGE.
    
    Title:          vt100.h : VT100/VT100 Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012/09/15
    Revision Info:  $Id: vt100.c 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "vt100.h"
#include <xprintf.h>


/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static u8_t vt100_state;

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void vt100_init()
{
    // Reset state
    vt100_state = 0;

    // Reset terminal
    xputc(VT100_CHAR_ESC);
    xputc('c');

    // Enable line wrap
    xputc(VT100_CHAR_ESC);
    xputc('[');
    xputc('7');
    xputc('h');

    // Wait until Terminal has resetted
    int i;
    for (i=0;i<100000;i++);
    //_delay(dly_10ms);
}

vt100_state_t vt100_on_rx_char(char data)
{
    switch(vt100_state)
    {
    case 0:
        if(data == VT100_CHAR_ESC)
        {
            // Escape sequence detected
            vt100_state++;
            // Indicate that received character should be discarded
            return VT100_ESC_SEQ_BUSY;
        }
        if(data >= 0x80)
        {
            // Invalid character received
            return VT100_CHAR_INVALID;            
        }
        // Normal character received
        return VT100_CHAR_NORMAL;        

    case 1:
        if(data == '[')
        {
            // Escape sequence detected
            vt100_state++;
            // Indicate that received character should be ignored
            return VT100_ESC_SEQ_BUSY;
        }
        // Incorrect escape sequence
        vt100_state = 0;
        // Indicate that received character should be ignored
        return VT100_ESC_SEQ_BUSY;

    case 2:
        // Reset state first
        vt100_state = 0;

        // Detect sequence
        switch(data)
        {
        case 'A': return VT100_ESC_SEQ_ARROW_UP;
        case 'B': return VT100_ESC_SEQ_ARROW_DN;
        case 'C': return VT100_ESC_SEQ_ARROW_LEFT;
        case 'D': return VT100_ESC_SEQ_ARROW_RIGHT;
        default:  return VT100_CHAR_INVALID;
        }

    default:
        //Reset state
        vt100_state = 0;
        // Indicate that received character should be discarded
        return VT100_CHAR_INVALID;
    }
}

void vt100_clr_screen(void)
{
    xputc(VT100_CHAR_ESC);
    xputc('[');
    xputc('2');
    xputc('J');
}

void vt100_erase_line(void)
{
    xputc(VT100_CHAR_ESC);
    xputc('[');
    xputc('2');
    xputc('K');
}

void vt100_del_chars(u8_t nr_of_chars)
{
    while(nr_of_chars != 0)
    {
        xputc(VT100_CHAR_BS);
        xputc(' ');
        xputc(VT100_CHAR_BS);
        nr_of_chars--;
    }
}
