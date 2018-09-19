/*
 - SysTick sample code
 - Originally written for the Stellaris (2013)
 - Will need to use debugger to "see" interrupts
 - Code uses bit-operations to access SysTick bits
*/

#include "Systick.h"
#include "MessagePassing.h"

void SysTickStart(void)
{
//  Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
//  Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
//  For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
    ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
//  Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
//  Clear the interrupt bit in STCTRL
    ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

void InitClock(void)
{
    // Initialize SYSTICK
    SysTickPeriod(MAX_WAIT);
    SysTickIntEnable();
    SysTickStart();
}

void SysTickHandler(void)
{
    if( !SendMessage('$', SYSTICK_TX, MONITOR_RX) ) {
        // TODO: Handle if Queue Full
    }
}
