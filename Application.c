/*
 * Application.c
 *
 *  Created on: Sep 17, 2018
 *      Author: Shamus
 */
#include "Application.h"
#include <stdio.h>
#include <string.h>

#define PRIVATE     static

PRIVATE struct Time CurrentTime = {0,0,0,0,1,JAN};  // default time
PRIVATE char *Month[MONTH_PER_YR] = {"JAN","FEB","MAR","APR","MAY","JUN",
                                       "JUL","AUG","SEP","OCT","NOV","DEC"};
PRIVATE unsigned DaysPerMonth[MONTH_PER_YR] = {31, 28, 31, 30, 31, 30,
                                                 31, 31, 30, 31, 30, 31};
int time(char *arg, char *OutputStr)
{
    unsigned sec, min, hr;
    // ARG
    if(*arg != '\0') {
        sscanf(arg,"%d:%d:%d",&hr,&min,&sec);
        // valid times
        if(hr < 24 && min < 60 && sec < 60) {
            CurrentTime.hour = hr;
            CurrentTime.min = min;
            CurrentTime.sec = sec;
        }
        else {
            return 0;
        }
    }
    sprintf(OutputStr,"%02d:%02d:%02d\r",
            CurrentTime.hour, CurrentTime.min,
            CurrentTime.sec);
    return 1;
}



void UpdateTime(void)
{
    // Increment clock
    CurrentTime.ticks++;

    if(CurrentTime.ticks % TICKS_PER_SEC == 0) {
        CurrentTime.sec++;
    }
    if(CurrentTime.sec == SEC_PER_MIN) {
        CurrentTime.min++;
        CurrentTime.sec = 0;
    }
    if(CurrentTime.min == MIN_PER_HOUR) {
        CurrentTime.hour++;
        CurrentTime.min = 0;
    }
    if(CurrentTime.hour == HOUR_PER_DAY) {
        CurrentTime.day++;
        CurrentTime.hour = 0;
    }
    if(CurrentTime.day > DaysPerMonth[CurrentTime.month]) {
        CurrentTime.month++;
        CurrentTime.day = 1;
    }
    if(CurrentTime.month > MONTH_PER_YR) {
        CurrentTime.month = JAN;
        CurrentTime.year++;
    }

}


