#ifndef __CLI_H__
#define __CLI_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [http://piconomic.co.za]
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
    Revision Info:  $Id: cli.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/** 
    @ingroup COMMS
    @defgroup CLI cli.h : Command Line Interpreter

    Implements a command line interpreter interfacing with a ANSI/VT100
    terminal emulator.
    
    File(s):
    - comms/cli.h
    - comms/cli.c (general version)
    - comms/cli_P.c (minimise RAM usage by using Program Memory (ATTR_PGM)
 
    @see
    - http://en.wikipedia.org/wiki/VT100_escape_code
    - http://www.termsys.demon.co.uk/vtansi.htm
 
    This CLI supports autocomplete using TAB and recalling old commands
    using UP/DOWN (command history).
 
    @image html images/cli.png "CLI using Tera Term terminal emulator"
    
    Example:
 
    @include comms/test/cli_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"
#include "vt100.h"
#include <xprintf.h>

#include <cr_section_macros.h>
// Include project specific config. See "cli_cfg_example.h"
#include <cli_cfg.h>

// Check that all project specific options have been correctly specified
#ifndef CLI_ARGV_MAX
#error "CLI_ARGV_MAX not defined"
#endif

#ifndef CLI_LINE_LENGTH_MAX
#error "CLI_LINE_LENGTH_MAX not defined"
#endif
#if (CLI_LINE_LENGTH_MAX > 255)
#error "CLI_LINE_LENGTH_MAX must be less than 256"
#endif

#ifndef CLI_TREE_DEPTH_MAX
#error "CLI_TREE_DEPTH_MAX not defined"
#endif
#if (CLI_TREE_DEPTH_MAX < 1) || (CLI_TREE_DEPTH_MAX > CLI_ARGV_MAX)
#error "CLI_LINE_LENGTH_MAX must be at least 1 and less than or equal to CLI_ARGV_MAX"
#endif

#ifndef CLI_HISTORY_SIZE
#error "CLI_HISTORY_SIZE not defined"
#endif
#if (CLI_HISTORY_SIZE != 0) && (CLI_HISTORY_SIZE < CLI_LINE_LENGTH_MAX)
#error "CLI_HISTORY_SIZE must be equal or greater than CLI_LINE_LENGTH_MAX"
#endif
#if (CLI_HISTORY_SIZE > 256)
#error "CLI_HISTORY_SIZE must be equal or less than 256"
#endif

#ifndef CLI_HELP_STR
#error "CLI_HELP_STR not defined"
#endif

#ifndef CLI_NAME_STR_MAX_SIZE
#error "CLI_NAME_STR_MAX_SIZE not defined"
#endif

#ifndef CLI_PARAM_STR_MAX_SIZE
#error "CLI_PARAM_STR_MAX_SIZE not defined"
#endif

#if (CLI_NAME_STR_MAX_SIZE != 0) && (CLI_PARAM_STR_MAX_SIZE == 0)
#error "CLI_PARAM_STR_MAX_SIZE must also be specifed to reduce code size"
#endif
#if (CLI_NAME_STR_MAX_SIZE == 0) && (CLI_PARAM_STR_MAX_SIZE != 0)
#error "CLI_NAME_STR_MAX_SIZE must also be specifed to reduce code size"
#endif


/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Pointer to a function that will be called to handle a command
typedef const char* (*cli_handler_t)(u8_t argc, char* argv[]);

/// Command structure
typedef struct
{
    const char *  name;        ///< Command name
    u8_t          argc_min;    ///< Minimum number of valid arguments
    u8_t          argc_max;    ///< Maximum number of valid arguments
    const char *  param;       ///< Parameter string to be displayed when 'help' is executed
#if CLI_HELP_STR
    const char *  help;        ///< Help string to be displayed when 'help' is executed
#endif
} cli_cmd_t;

/// Command group structure
typedef struct
{
    const char *                       name;  ///< Command name
    const struct cli_cmd_list_item_s * list;  ///< Pointer to command list array
} cli_group_t;

/// Command list item declaration
typedef struct cli_cmd_list_item_s
{
    cli_handler_t   handler;        ///< Function to be called when command is executed
    union
    {
        const cli_cmd_t *   cmd;    ///< pointer to command (if handler != NULL)
        const cli_group_t * group;  ///< pointer to group   (if handler = NULL)
    };
} cli_cmd_list_item_t;

/// ARGV converted value types
typedef union
{
    u8_t     u8;
    u16_t    u16;
    u32_t    u32;
    s8_t     s8;
    s16_t    s16;
    s32_t    s32;
    float_t  f;
    double_t d;
} cli_argv_val_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Converted argument value using cli_util_argv_to_...() conversion function
extern cli_argv_val_t cli_argv_val;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
   Initialise command line module.

   @param startup   Start up string to display.
 */
extern void cli_init(const char* startup);

/** 
   Function called to handle a received character.

   This function drives the command line parser. All actions are taken in 
   response to a received character.

   @param data   The received character.
 */
extern void cli_on_rx_char(char data);

/**
    Handler function to call when "help" command is invoked.

    @param argc         Number of arguments
    @param argv         List of zero terminated argument strings
    
    @return const char* Response string to display; return NULL to display nothing
 */
extern const char* cli_cmd_help_fn(u8_t argc, char* argv[]);

/**
    Utility function to convert an ARGV string to an option.

    The ARGV string is compared to a series of zero terminated strings with the
    end indicated by an empty string ('\0'). The index of the string that it
    matches is returned. 0xff is returned if no match is found.
    
    Example:
    
        @code
        switch(cli_util_argv_to_option(1, "on\0off\0")))
        {
        case 0:
            LED_ON();
            break;
        case 1:
            LED_OFF();
            break;
        default:
            return "Error: incorrect parameters!";
        }
        @endcode    

    @param argv_index Index of ARGV to convert into an option
    @param options    Series of zero terminated strings. End is indicated by an
                      empty string.
    
    @return u8_t      Index of the string that matches ARGV string.
    @retval 0xff      No match was found.
 */
u8_t cli_util_argv_to_option(u8_t argv_index, const char* options);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_u8(u8_t argv_index, u8_t min, u8_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_u16(u8_t argv_index, u16_t min, u16_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_u32(u8_t argv_index, u32_t min, u32_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_s8(u8_t argv_index, s8_t min, s8_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_s16(u8_t argv_index, s16_t min, s16_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters. The string is converted 
   into a number and it is tested if the number is within the specified 
   bounds (min <= val <= max).

   @param argv_index Index of ARGV to convert
   @param min        Minimum value accepted for number
   @param max        Maximum value accepted for number

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_s32(u8_t argv_index, s32_t min, s32_t max);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters.

   @param argv_index Index of ARGV to convert

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_float(u8_t argv_index);

/** 
   Utility function to convert an ARGV string to a number.

   This function is useful to parse ARGV parameters.

   @param argv_index Index of ARGV to convert

   @retval TRUE      String has been succesfully converted into a number and 
                     lies between the specified bounds.
 */
extern bool_t cli_util_argv_to_double(u8_t argv_index);

/**
   Utility function to display the content of a buffer.
    
   This function displays the content of a buffer in HEX and ASCII. The output 
   is formatted in columns of 16 bytes of HEX data and adjacent 16 characters 
   of ASCII data. If the ASCII data is not displayable (code < 32 or code > 127) 
   then a dot ('.') is displayed instead.
   
   @param data          Pointer to buffer containing data to display
   @param nr_of_bytes   Number of bytes in buffer to display.
 */
void cli_util_disp_buf(const u8_t * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */
/**
    Macro to create a new CLI command structure.
 
    Example:
 
        @code
        CLI_CMD_CREATE(cli_cmd_help, "help", "Display list of commands", 0, 0)
        @endcode
 
    @param cli_cmd_name     Name of command structure
    @param name_str         Name of command
    @param nr_arg_min       Minimum number of valid arguments
    @param nr_arg_max       Maximum number of valid arguments
    @param param_str        Parameter string displayed when 'help' command is
                            executed
    @param help_str         Help string displayed when 'help' command is
                            executed    
 */
#if CLI_HELP_STR
#define CLI_CMD_CREATE(cli_cmd_name, name_str, nr_arg_min, nr_arg_max, param_str, help_str) \
		__RODATA(Flash2) static const char cli_cmd_name ## _name[]   = name_str; \
		__RODATA(Flash2) static const char cli_cmd_name ## _param[]  = param_str; \
		__RODATA(Flash2) static const char cli_cmd_name ## _help[]   = help_str; \
		__RODATA(Flash2) const cli_cmd_t cli_cmd_name  = \
    { \
        .name     = cli_cmd_name ## _name, \
        .argc_min = nr_arg_min, \
        .argc_max = nr_arg_max, \
        .param    = cli_cmd_name ## _param, \
        .help     = cli_cmd_name ## _help, \
    };
#else
#define CLI_CMD_CREATE(cli_cmd_name, name_str, nr_arg_min, nr_arg_max, param_str, help_str) \
		__RODATA(Flash2) static const char cli_cmd_name ## _name[]   = name_str; \
		__RODATA(Flash2) static const char cli_cmd_name ## _param[]  = param_str; \
		__RODATA(Flash2) const cli_cmd_t cli_cmd_name ATTR_PGM = \
    { \
        .name     = cli_cmd_name ## _name, \
        .argc_min = nr_arg_min, \
        .argc_max = nr_arg_max, \
        .param    = cli_cmd_name ## _param, \
    };
#endif

/**
    Macro to create a new CLI command group structure.
 
    Example:
 
        @code
        CLI_GROUP_CREATE(cli_group_i2c, "i2c")
        @endcode
 
    @param cli_group_name   Name of command group structure
    @param name_str         Name of command
 */
#define CLI_GROUP_CREATE(cli_group_name, name_str) \
		__RODATA(Flash2) static const char                cli_group_name ## _str[]  = name_str; \
		__RODATA(Flash2) static const cli_cmd_list_item_t cli_group_name ## _list[] ; \
		__RODATA(Flash2) const cli_group_t cli_group_name  = \
    { \
        .name = cli_group_name ## _str, \
        .list = cli_group_name ## _list, \
    }; \
    __RODATA(Flash2) static const cli_cmd_list_item_t cli_group_name ## _list[]  = \
    {

/// Macro to end a command list declaration
#define CLI_GROUP_END() \
        { \
            .handler = NULL, \
            .cmd     = NULL, \
        }, \
    };

/// Macro to start a command list declaration.
#define CLI_CMD_LIST_CREATE() \
		__RODATA(Flash2) const cli_cmd_list_item_t cli_cmd_list[]  = \
    {

/**
    Macro to add a created command structure to the list
    
    @param cli_cmd_name     Name of command structure
    @param handler_fn       Function (of type cli_handler_t) to be called when
                            command is executed
    
  */
#define CLI_CMD_ADD(cli_cmd_name, handler_fn) \
        { \
            .handler = handler_fn, \
            .cmd     = &cli_cmd_name, \
        },

/**
    Macro to add a created command group structure to the list
    
    @param cli_group_name     Name of command group structure
  */
#define CLI_GROUP_ADD(cli_group_name) \
        { \
            .handler = NULL, \
            .group   = &cli_group_name, \
        },

/// Macro to end a command list declaration
#define CLI_CMD_LIST_END() \
        { \
            .handler = NULL, \
            .cmd     = NULL, \
        }, \
    };

/// @}
#endif
