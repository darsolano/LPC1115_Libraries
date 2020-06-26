#ifndef __CLI_CFG_H__
#define __CLI_CFG_H__
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
    
    Title:          cli_cfg.h : CLI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-15
    Revision Info:  $Id: cli_cfg_example.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/** 
    @addtogroup CLI
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Maximum number of arguments (including command)
#define CLI_ARGV_MAX            16

/// Define the maximum length of a command line
#define CLI_LINE_LENGTH_MAX     64

/// Define the maximum depth of command strings
#define CLI_TREE_DEPTH_MAX      2

/** 
    Define the command line history size (use 0 to remove history).
 
    It must be able to accept at least one cmd line completely!
    CLI_HISTORY_SIZE must be equal or less than 256. If not zero,
    CLI_HISTORY_SIZE must also be equal or greater than CLI_LINE_LENGTH_MAX.
 */
#define CLI_HISTORY_SIZE        64

/// Display help strings (1) or remove help strings (0) to reduce code size
#define CLI_HELP_STR            1

/// Specify maximum command name string length (not zero) or calculate run time (zero)
#define CLI_NAME_STR_MAX_SIZE   16

/// Specify maximum param string length (not zero) or calculate run time (zero)
#define CLI_PARAM_STR_MAX_SIZE  42

/// @}
#endif
