/*
 * Application.h
 *
 *  Created on: Sep 17, 2018
 *      Author: Shamus
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#define TICKS_PER_SEC       1
#define SEC_PER_MIN         60
#define MIN_PER_HOUR        60
#define HOUR_PER_DAY        24
#define MONTH_PER_YR        12

enum MONTH_INDEX {JAN, FEB, MAR, APR, MAY, JUN,
                  JUL, AUG, SEP, OCT, NOV, DEC};

struct Time {
    unsigned ticks;
    unsigned sec;   // 0 - 59
    unsigned min;   // 0- 59
    unsigned hour;  // 0 - 23
    unsigned day;   // Range depends on month
    enum MONTH_INDEX month;    // 0 - 11
    unsigned year;  // 0000 - 9999
};

int time(char *,char *);

void UpdateTime(void);

#endif /* APPLICATION_H_ */
