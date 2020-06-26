/*
 * rtc_shell.h
 *
 *  Created on: Mar 8, 2016
 *      Author: dsolano
 */

#ifndef INC_RTC_SHELL_H_
#define INC_RTC_SHELL_H_

// Public functions
void rtc_hwd_setup(void);
Status rtc_read_ram(uint8_t addr , int8_t len);
void rtc_display_time_date(void);
void rtc_time_init(void);
void rtc_LCDMenu(void);

#endif /* INC_RTC_SHELL_H_ */
