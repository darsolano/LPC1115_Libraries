/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [http://piconomic.co.za]
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
    
    Title:          cli.h : Command Line Interpreter and command dispatcher
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/01
    Revision Info:  $Id: cli.c 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <debug_frmwrk.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "cli.h"



#include "dbg.h"
//DBG_DECL_NAME("cli")


/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Externally defined command list that must be defined using CLI_CMD_LIST_CREATE macro
extern const cli_cmd_list_item_t cli_cmd_list[];

/// Converted argument value using cli_util_argv_to_...() conversion function
cli_argv_val_t cli_argv_val;

/* _____LOCAL VARIABLES______________________________________________________ */
/// Buffer for command line
static char cli_line_buf[CLI_LINE_LENGTH_MAX];
static u8_t cli_line_buf_index;

/// Autocomplete index that is used to mark start of word used for match
static u8_t cli_autocomplete_start_index;
/// Autocomplete index that is used to mark end of word used for match
static u8_t cli_autocomplete_end_index;

#if CLI_HISTORY_SIZE
/** 
    Circular buffer to store history of cmd line strings entered by user.
    
    Cmd line strings are saved in this circular buffer as a series of zero
    terminated strings. If the newest string partially overwrites the oldest
    string, the remaining characters of the oldest string is also zero'd.
    
    Example:
    
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                              cli_hist_index_last
                                                       |
    |  0  | 'O' | 'N' | 'E' |  0  | 'T' | 'W' | 'O' |  0  |  0  |  0  |
                                     |
                             cli_hist_index_now
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    @see cli_hist_save_cmd()
 */
static char cli_hist_circ_buf[CLI_HISTORY_SIZE];
/// Zero terminated *END* of last cmd line string stored in circular buffer
static u8_t cli_hist_index_last;
/// Zero terminated *START* of current cmd line string displayed
static u8_t cli_hist_index_now;
#endif

/// List of pointers to strings (command line string broken up into words)
static char* cli_argv[CLI_ARGV_MAX];

// Current command list item tree being processed
static const cli_cmd_list_item_t * cli_tree[CLI_TREE_DEPTH_MAX];

// Index of current command list item tree being processed
static u8_t cli_tree_index;

// Current command list item being processed
static const cli_cmd_list_item_t * cli_cmd_list_item;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static bool_t cli_cmd_get_item(const cli_cmd_list_item_t * item);
static bool_t cli_cmd_item_get_root(void);
static bool_t cli_cmd_item_get_parent(void);
static bool_t cli_cmd_item_get_child(void);
static bool_t cli_cmd_item_get_first(void);
static bool_t cli_cmd_item_get_next(void);

static u8_t cli_cmd_line_to_args(void);

#if CLI_HISTORY_SIZE
static u8_t cli_hist_circ_buf_index_prev(u8_t index) ;
static u8_t cli_hist_circ_buf_index_next(u8_t index) ;
static void cli_hist_copy(void);
static void cli_hist_save_cmd(void);
static void cli_hist_load_older_cmd(void);
static void cli_hist_load_newer_cmd(void);
#endif

static void cli_autocomplete_reset(void);
static bool_t cli_autocomplete(void);

static void cli_cmd_exe(void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool_t cli_cmd_get_item(const cli_cmd_list_item_t * item)
{
    cli_cmd_list_item = item;

    // End of list?
    if(cli_cmd_list_item->cmd == NULL)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static bool_t cli_cmd_item_get_root(void)
{
    cli_tree_index    = 0;
    cli_tree[0]       = cli_cmd_list;

    return cli_cmd_get_item(cli_tree[0]);
}

static bool_t cli_cmd_item_get_parent(void)
{
    // Already in root list?
    if(cli_tree_index == 0)
    {
        DBG_ERR("Already in root\n");
        return FALSE;
    }
        
    // Go back to parent list
    cli_tree_index--;

    return cli_cmd_get_item(cli_tree[cli_tree_index]);
}

static bool_t cli_cmd_item_get_child(void)
{
    // End of list or command item?
    if(  (cli_cmd_list_item->cmd     == NULL)
       ||(cli_cmd_list_item->handler != NULL)  )
    {
        DBG_ERR("Not a group item\n");
        return FALSE;
    }

    // Maximum depth reached?
    if(cli_tree_index >= (CLI_TREE_DEPTH_MAX-1))
    {
        DBG_ERR("Maximum command depth exceeded\n");
        return FALSE;
    }

    // Go to child list
    cli_tree_index++;
    cli_tree[cli_tree_index] = cli_cmd_list_item->group->list;

    return cli_cmd_get_item(cli_tree[cli_tree_index]);
}

static bool_t cli_cmd_item_get_first(void)
{
    // Root list?
    if(cli_tree_index == 0)
    {
        // Reset to start of root list
        cli_tree[0] = cli_cmd_list;
    }
    else
    {
        // Get parent item
        cli_cmd_get_item(cli_tree[cli_tree_index-1]);
        // Reset to start of list
        cli_tree[cli_tree_index] = cli_cmd_list_item->group->list;
    }
    
    return cli_cmd_get_item(cli_tree[cli_tree_index]);
}

static bool_t cli_cmd_item_get_next(void)
{
    // End of list reached?
    if(cli_cmd_list_item->cmd == NULL)
    {
        DBG_ERR("End of list already reached\n");
        return FALSE;
    }

    // Next item in list
    cli_tree[cli_tree_index]++;

    return cli_cmd_get_item(cli_tree[cli_tree_index]);
}

static u8_t cli_cmd_line_to_args(void)
{
    u8_t   argc;
    char * str;

    // Zero terminate command line
    cli_line_buf[cli_line_buf_index] = '\0';

    /* 
       Break command line string up into separate words:
       Array of pointers to zero terminated strings
     */
    argc = 0;
    str  = cli_line_buf;
    while(TRUE)
    {
        // Replace spaces with zero termination
        while(*str == ' ')
        {
            *str++ = '\0';
        }
        // End of line?
        if(*str == '\0')
        {
            break;
        }
        // Maximum number of arguments exceeded?
        if(argc >= CLI_ARGV_MAX)
        {
            break;
        }
        // Save start of word and increment argument count
        cli_argv[argc++] = str;

        // Move to end of word
        while(  (*str != ' ') && (*str != '\0')  )
        {
            str++;
        }
    }

    return argc;
}

#if CLI_HISTORY_SIZE
static u8_t cli_hist_circ_buf_index_next(u8_t index)
{
    if(index >= (CLI_HISTORY_SIZE-1))
    {
        return 0;
    }
    else
    {
        return (index+1);
    }
}

static u8_t cli_hist_circ_buf_index_prev(u8_t index)
{
    if(index == 0)
    {
        return (CLI_HISTORY_SIZE-1);
    }
    else
    {
        return (index-1);
    }
}

static void cli_hist_copy(void)
{
    u8_t data;
    u8_t i;
    u8_t j;

    // Delete old command from terminal
    vt100_del_chars(cli_line_buf_index);

    // Copy characters from history to command line
    i = 0;
    j = cli_hist_index_now;
    while(TRUE)
    {
        // Fetch character from history
        data = cli_hist_circ_buf[j];
        // End reached?
        if(data == '\0')
        {
            break;
        }
        // Send character to terminal
        xputc(data);
        // Copy character to cmd line buffer
        cli_line_buf[i++] = data;
        // Next index
        j = cli_hist_circ_buf_index_next(j);
    }
    cli_line_buf_index = i;
}

static void cli_hist_save_cmd(void)
{
    u8_t i;
    u8_t j;

    // Empty command?
    if(cli_line_buf_index == 0)
    {
        // Reset up/down history to end of latest saved command
        cli_hist_index_now = cli_hist_index_last;
        return;
    }

    // Duplicate command?
    i = cli_line_buf_index;
    j = cli_hist_index_last;
    while(TRUE)
    {
        // Previous index
        i--;
        j = cli_hist_circ_buf_index_prev(j);

        // No match?
        if(cli_line_buf[i] != cli_hist_circ_buf[j])
        {
            // New command
            break;
        }
        // Complete match?
        if(i == 0)
        {
            // Duplicate command... reset up/down history
            cli_hist_index_now = cli_hist_index_last;
            return;
        }
    }

    // Append command line string (except terminating zero) in history circular 
    // buffer
    i = 0;
    j = cli_hist_index_last;
    do
    {
        // Next index
        j = cli_hist_circ_buf_index_next(j);
        // Append character from line buffer
        cli_hist_circ_buf[j] = cli_line_buf[i++];        
    }
    while(i < cli_line_buf_index);

    // Remember end of last saved string
    j = cli_hist_circ_buf_index_next(j);
    cli_hist_index_last = j;
    // Reset up/down history to end of latest saved command
    cli_hist_index_now = j;

    /* 
       Zero terminate and eat remaining characters of oldest command line in
       history that may have been partially overwritten.
     */
    while(TRUE)
    {
        // Terminating zero reached?
        if(cli_hist_circ_buf[j] == '\0')
        {
            // Stop
            break;
        }
        else
        {
            // Reset to zero
            cli_hist_circ_buf[j] = '\0';
        }
        // Next index
        j = cli_hist_circ_buf_index_next(j);
    }
}

static void cli_hist_load_older_cmd(void)
{
    u8_t i;

    // Oldest in history already displayed?
    i = cli_hist_circ_buf_index_prev(cli_hist_index_now);
    if(i == cli_hist_index_last)
    {
        return;
    }
    i = cli_hist_circ_buf_index_prev(i);
    if(cli_hist_circ_buf[i] == '\0')
    {
        return;
    }

    // Find start of older cmd saved in history
    while(cli_hist_circ_buf[i] != '\0')
    {
        i = cli_hist_circ_buf_index_prev(i);
    }
    cli_hist_index_now = cli_hist_circ_buf_index_next(i);

    // Replace current command line with one stored in history
    cli_hist_copy();    
}

static void cli_hist_load_newer_cmd(void)
{
    u8_t i;

    // Find start of newer cmd saved in history
    i = cli_hist_index_now;
    while(cli_hist_circ_buf[i] != '\0')
    {
        i = cli_hist_circ_buf_index_next(i);
    }

    // Newest command already displayed?
    if(i != cli_hist_index_last)
    {
        // Move index to start of string
        i = cli_hist_circ_buf_index_next(i);
    }
    cli_hist_index_now = i;

    // Replace current command line with one stored in history
    cli_hist_copy();    
}
#endif

static void cli_autocomplete_reset(void)
{
    // Reset autocomplete to last typed character
    cli_autocomplete_start_index = 0; 
    cli_autocomplete_end_index   = cli_line_buf_index;

    // Start at beginning of list
    cli_cmd_item_get_root();
}

static bool_t cli_autocomplete(void)
{
    u8_t         i;
    const char * name;
    const cli_cmd_list_item_t * cmd_start = cli_tree[cli_tree_index];

    i = cli_autocomplete_start_index;
    while(TRUE)
    {
        name = cli_cmd_list_item->cmd->name;

        // Does name match line?
        while(i < cli_autocomplete_end_index)
        {
            // Fetch line character
            char line_char = cli_line_buf[i++];
            // Fetch name character
            char name_char = *name++;

            // End of name *and* end of word reached?
            if(  (name_char == '\0') && (line_char == ' ')  )
            {
                // Name match... is this a group item?
                if(cli_cmd_list_item->handler == NULL)
                {
                    // Proceed to child name
                    cli_cmd_item_get_child();
                    name = cli_cmd_list_item->cmd->name;
                    // Set start index to start of child command
                    cli_autocomplete_start_index = i;
                    break;
                }
                else
                {
                    // This is a command item... no match
                    return FALSE;
                }
            }
            // String and name character mismatch?
            else if(line_char != name_char)
            {
                // No match.. reset to start of word
                i = cli_autocomplete_start_index;
                // Proceed to next item
                if(!cli_cmd_item_get_next())
                {
                    // Go back to start of list
                    cli_cmd_item_get_first();
                }
                // Reset to start of word
                i = cli_autocomplete_start_index;
                break;
            }
        }

        // Autocomplete match reached?
        if(i >= cli_autocomplete_end_index)
        {
            // (Partial) match
            break;
        }

        // Cycled through list?
        if(cli_tree[cli_tree_index] == cmd_start)
        {
            // No match
            return FALSE;
        }
    }

    // Autocomplete rest of name
    vt100_del_chars(cli_line_buf_index-cli_autocomplete_end_index);
    while(TRUE)
    {
        char name_char = *name++;
        if(name_char == '\0')
        {
            break;
        }
        if(i >= (CLI_LINE_LENGTH_MAX-1))
        {
            break;
        }
        cli_line_buf[i++] = name_char;
        xputc(name_char);
    }
    cli_line_buf_index = i;

    // Next item in list for next autocomplete
    if(!cli_cmd_item_get_next())
    {
        // Go back to start of list
        cli_cmd_item_get_first();            
    }

    return TRUE;
}

static void cli_cmd_exe(void)
{
    u8_t         argc;
    char **      argv;
    const char * report_str;

    /* 
       Break command line string up into separate words:
       Array of pointers to zero terminated strings
     */
    argc = cli_cmd_line_to_args();

    // Ignore empty command
    if(argc == 0)
    {
        return;
    }
    
    // Find command in command list
    cli_cmd_item_get_root();
    while(TRUE)
    {
        // End of list or not enough arguments?
        if(  (cli_cmd_list_item->cmd == NULL)
           ||(cli_tree_index         >= argc)  )
        {
            // Command not found in list
            xputs("Error! Command not found\n");
            return;
        }

        // Does the argument match the command string?
        if(strcmp(cli_argv[cli_tree_index],
                    cli_cmd_list_item->cmd->name) == 0)
        {
            // Is this a command item?
            if(cli_cmd_list_item->handler != NULL)
            {
                // Command match
                break;
            }
            else
            {
                // Group item match... proceed to child list
                cli_cmd_item_get_child();
                
            }
        }
        else
        {
            // Next item in list
            cli_cmd_item_get_next();
        }
    }

    // Remove command argument(s)
    argc -= (cli_tree_index+1);
    argv  = &cli_argv[cli_tree_index+1];

    // Does number of parameters exceed bounds?
    if(  (argc < cli_cmd_list_item->cmd->argc_min) 
       ||(argc > cli_cmd_list_item->cmd->argc_max)  )
    {
        xputs("Error! Number of parameters incorrect\n");
        return;
    }

    // Execute command with parameters
    report_str = (*(cli_cmd_list_item->handler))(argc, argv);

    // Did handler report a string to display?
    if(report_str != NULL)
    {
        // Display string
        xputs(report_str);
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void cli_init(const char* startup_str)
{
#if CLI_HISTORY_SIZE
    u8_t i;

    // Clear history buffer
    for(i=0; i<CLI_HISTORY_SIZE; i++)
    {
        cli_hist_circ_buf[i] = '\0';
    }
    cli_hist_index_last = 0;
    cli_hist_index_now = 0;
#endif

    // Reset
    cli_line_buf_index = 0;
    cli_autocomplete_reset();

    // Reset Terminal
    vt100_init();    

    // Display startup string
    if(startup_str != NULL)
    {
        xputs(startup_str);
    }

    // Display start up help advice
#if CLI_HELP_STR
    xprintf("Type 'help' to get list of cmds with help descriptions\n\n"
#else
    xprintf("Type 'help' to get list of cmds\n\n"
#endif
           "TAB   to cycle autocomplete cmd(s)\n"
#if CLI_HISTORY_SIZE
           "UP/DN to traverse history of cmds entered\n"
#endif
           "ENTER to execute cmd\n\n");

    // Display prompt
    xputc('>');
}

void cli_on_rx_char(char data)
{
    // Process received character to detect ANSI Escape Sequences
    switch(vt100_on_rx_char(data))
    {
    case VT100_CHAR_NORMAL:
        switch(data)
        {
        // ENTER has been pressed
        case VT100_CHAR_CR:
            // Terminate line
            xputc('\n');
#if CLI_HISTORY_SIZE
            // Save command
            cli_hist_save_cmd();
#endif
            // Execute command
            cli_cmd_exe();
            // Reset command buffer
            cli_line_buf_index = 0;
            // Reset autocomplete
            cli_autocomplete_reset();
            // Display prompt
            xputc('>');
            return;
        
        // BACK SPACE has been pressed
        case VT100_CHAR_BS:
            // Buffer not empty?
            if(cli_line_buf_index > 0)
            {
                // Remove last character from buffer
                cli_line_buf_index--;
                // Remove last character from terminal screen
                vt100_del_chars(1);
                // Reset autocomplete to last character
                cli_autocomplete_reset();
            }
            else
            {
                // No characters to delete
                xputc(VT100_CHAR_BEL);
            }
            return;
    
        // TAB has been pressed
        case VT100_CHAR_TAB:
            if(!cli_autocomplete())
            {
                // Autocomplete failed
                xputc(VT100_CHAR_BEL);
            }
            return;

        default:
            break;
        }

        // Ignore invalid values
        if(data < 0x20)
        {
            return;
        }
        // Buffer not full?
        if(cli_line_buf_index < (CLI_LINE_LENGTH_MAX-1))
        {
            // Add character to line buffer
            cli_line_buf[cli_line_buf_index++] = data;
            // Reset autocomplete to last character
            cli_autocomplete_reset();
            // Echo character
            xputc(data);
        }
        else
        {
            // Buffer full
            xputc(VT100_CHAR_BEL);
        }
        return;

    case VT100_CHAR_INVALID:
        // Discard/ignore character
        return;

    case VT100_ESC_SEQ_BUSY:
        // Discard/ignore character
        return;

#if CLI_HISTORY_SIZE
    case VT100_ESC_SEQ_ARROW_UP:
        cli_hist_load_older_cmd();
        return;

    case VT100_ESC_SEQ_ARROW_DN:
        cli_hist_load_newer_cmd();
        return;
#endif

    default:
        return;
    }    
}

const char* cli_cmd_help_fn(u8_t argc, char* argv[])
{
    u8_t   i;
    u8_t   len;
    u8_t   name_char_cnt;
    u8_t   param_char_cnt;
    bool_t line_break;

    // Find longest command and param length
    name_char_cnt  = CLI_NAME_STR_MAX_SIZE;
    param_char_cnt = CLI_PARAM_STR_MAX_SIZE;

#if ((CLI_NAME_STR_MAX_SIZE == 0) || (CLI_PARAM_STR_MAX_SIZE == 0))
    cli_cmd_item_get_root();
    while(TRUE)
    {
        // End of list?
        if(cli_cmd_list_item->cmd == NULL)
        {
            // Root list?
            if(cli_tree_index == 0)
            {
                // The end has been reached
                break;
            }
            else
            {
                // Return to parent list
                cli_cmd_item_get_parent();
                // Next item
                cli_cmd_item_get_next();
                continue;
            }
        }

        // Is this a command item?
        if(cli_cmd_list_item->handler != NULL)
        {
            u8_t len;

            // Longest command string?
            len = 0;
            for(i=0; i<=cli_tree_index; i++)
            {
                cli_cmd_get_item(cli_tree[i]);
                len += strlen(cli_cmd_list_item->cmd->name) + 1;
            }
            if(name_char_cnt < len)
            {
                // Remember longest command string
                name_char_cnt = len;
            }
            // Longest param string?
            len = strlen(cli_cmd_list_item->cmd->param);
            if(param_char_cnt < len)
            {
                // Remember longest param string
                param_char_cnt = len;
            }

            // Next item in list
            cli_cmd_item_get_next();
        }
        else
        {
            // Group item... proceed to child list
            cli_cmd_item_get_child();
        }        
    }
    DBG_INFO("Max command chars = %d\n", name_char_cnt);
    DBG_INFO("Max param chars = %d\n", param_char_cnt);
#endif

    // Display help for each command in list
    cli_cmd_item_get_root();
    line_break = FALSE;
    while(TRUE)
    {
        // End of list?
        if(cli_cmd_list_item->cmd == NULL)
        {
            // Root list?
            if(cli_tree_index == 0)
            {
                // The end has been reached
                break;
            }
            else
            {
                // Return to parent list
                cli_cmd_item_get_parent();
                // Next item
                cli_cmd_item_get_next();
                // Insert line break
                line_break = TRUE;
                continue;
            }
        }

        // Is this a command item?
        if(cli_cmd_list_item->handler != NULL)
        {
            cli_cmd_get_item(cli_tree[0]);
            if(  (argc == 0)
               ||(strncmp(argv[0],
                            cli_cmd_list_item->cmd->name,
                            strlen(argv[0])                 ) == 0)  )
            {
                // Insert line break?
                if(  (argc == 0) && (line_break)  )
                {
                    line_break = FALSE;
                    xputc('\n');
                }

                // Display all command strings
                len = 0;
                for(i=0; i<=cli_tree_index; i++)
                {
                    // Display name
                    cli_cmd_get_item(cli_tree[i]);
                    xprintf(cli_cmd_list_item->cmd->name);
                    xputc(' ');
                    len += strlen(cli_cmd_list_item->cmd->name) + 1;
                }
    
                // Adjust column
                for(i = len; i < name_char_cnt; i++)
                {
                    xputc(' ');
                }
    
                // Display param
                xprintf(cli_cmd_list_item->cmd->param);
#if CLI_HELP_STR    
                // Adjust column
                len = strlen(cli_cmd_list_item->cmd->param);
                for(i = len; i < param_char_cnt; i++)
                {
                    xputc(' ');
                }

                xputs(" : ");
                // Display help string
                xprintf(cli_cmd_list_item->cmd->help);
#endif
                xputc('\n');
            }

            // Next item in list
            cli_cmd_item_get_next();
        }
        else
        {
            // Group item... proceed to child list
            cli_cmd_item_get_child();
            // Insert line break;
            line_break = TRUE;
        }
    }

    return NULL;
}

u8_t cli_util_argv_to_option(u8_t argv_index, const char* options)
{
    u8_t index = 0;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    while(strlen(options) != 0)
    {
        if(strcmp(cli_argv[argv_index], options) == 0)
        {
            return index;
        }
        options += strlen(options) + 1;
        index++;
    }

    return 0xff;
}

bool_t cli_util_argv_to_u8(u8_t argv_index, u8_t min, u8_t max)
{
    unsigned long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtoul(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.u8 = (u8_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_u16(u8_t argv_index, u16_t min, u16_t max)
{
    unsigned long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtoul(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.u16 = (u16_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_u32(u8_t argv_index, u32_t min, u32_t max)
{
    unsigned long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtoul(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.u32 = (u32_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_s8(u8_t argv_index, s8_t min, s8_t max)
{
    long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtol(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.s8 = (s8_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_s16(u8_t argv_index, s16_t min, s16_t max)
{
    long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtol(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.s16 = (s16_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_s32(u8_t argv_index, s32_t min, s32_t max)
{
    long i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtol(cli_argv[argv_index], &end, 0);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    if((i >= min) && (i <= max) )
    {
        cli_argv_val.s32 = (s32_t)i;
        return TRUE;
    }
    return FALSE;
}

bool_t cli_util_argv_to_float(u8_t argv_index)
{
    double i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtod(cli_argv[argv_index], &end);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    cli_argv_val.f = (float)i;
    return TRUE;
}

bool_t cli_util_argv_to_double(u8_t argv_index)
{
    double i;
    char *end;

    // Adjust index
    argv_index += cli_tree_index+1;
    DBG_ASSERT(argv_index < CLI_ARGV_MAX);

    i = strtod(cli_argv[argv_index], &end);

    if(  (end == cli_argv[argv_index]) || (*end != '\0')  )
    {
        return FALSE;
    }
    cli_argv_val.d = i;
    return TRUE;
}

void cli_util_disp_buf(const u8_t * data, size_t nr_of_bytes)
{
    size_t i, j;

    for(i=0; i<nr_of_bytes; i+= 16)
    {
        for(j=i; j<(i+16); j++)
        {
            if(j<nr_of_bytes)
            {
                xprintf("%02hX ", data[j]);
            }
            else
            {
                xputs("   ");
            }
        }
        for(j=i; j<(i+16); j++)
        {
            if(j<nr_of_bytes)
            {
                if( (data[j] >= 32) && (data[j] <= 127) )
                {
                    xprintf("%c", data[j]);
                }
                else
                {
                    xputc('.');
                }
            }
            else
            {
            	xputc(' ');
            }
        }
        xputc('\n');
    }
}
