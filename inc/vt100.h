#ifndef __ANSI_ESC_H__
#define __ANSI_ESC_H__
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
    
    Title:          vt100.h : VT100/ANSI Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012/09/15
    Revision Info:  $Id: vt100.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/** 
    @ingroup COMMS
    @defgroup VT100 vt100.h : VT100/ANSI Terminal module
 
    Parses VT100 / ANSI escape sequences to interface with a terminal emulator.
    
    File(s):
    - comms/vt100.h
    - comms/vt100.c
    
    @see
    - http://en.wikipedia.org/wiki/ANSI_escape_code
    - http://www.termsys.demon.co.uk/vtansi.htm
    
    Example:
 
    @include comms/test/cli_test.c
  */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// @name Special ASCII values
//@{
#define VT100_CHAR_BEL      0x07
#define VT100_CHAR_BS       0x08
#define VT100_CHAR_TAB      0x09
#define VT100_CHAR_CR       0x0D
#define VT100_CHAR_LF       0x0A
#define VT100_CHAR_ESC      0x1B
#define VT100_CHAR_DEL      0x7F
//@}

/// VT100 Terminal receive state
typedef enum
{
    VT100_CHAR_NORMAL,          ///< A normal key has been pressed and must be used
    VT100_CHAR_INVALID,         ///< An invalid key code has been sent and must be discarded
    VT100_ESC_SEQ_BUSY,         ///< Busy with escape sequence; data must be discarded
    VT100_ESC_SEQ_ARROW_UP,     ///< Up Arrow has been pressed
    VT100_ESC_SEQ_ARROW_DN,     ///< Down Arrow has been pressed
    VT100_ESC_SEQ_ARROW_LEFT,   ///< Left Arrow has been pressed
    VT100_ESC_SEQ_ARROW_RIGHT,  ///< Right Arrow has been pressed
} vt100_state_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise module
extern void vt100_init(void);

/**
    Process a received character byte. 
    
    If an ANSI escape sequence is detected, VT100_ESC_SEQ_BUSY will be returned
    to indicate that received character should be ignored, otherwise
    VT100_CHAR_NORMAL is returned to indicate that the character must be used. 
    
    If an ANSI escape sequence is decoded it is indicated with a
    VT100_ESC_SEQ... state.
    
    @param data             Received character to be process for ANSI Escape Sequences
    
    @return vt100_state_t   VT100_CHAR_NORMAL if the character must be used as
                            normal; VT100_CHAR_INVALID or VT100_ESC_SEQ_BUSY if
                            character should be ignored; otherwise the decoded
                            ANSI Escape Sequence.
    
 */
extern vt100_state_t vt100_on_rx_char(char data);

/// Send 'clear screen' command to terminal
extern void vt100_clr_screen(void);

/// Send 'erase line' command to terminal
extern void vt100_erase_line(void);

/** 
    Delete specified number of characters
    
    @param nr_of_chars  Number of characters to delete
 */
extern void vt100_del_chars(u8_t nr_of_chars);

/* _____MACROS_______________________________________________________________ */

/// @}
#endif
