/*
 *  Interrupt.c
 *
 *  Created on: Sep 11, 2018
 *      Author: Shamus MacDonald
 *      Disclaimer: The code contained in this file and the declarations contained
 *      in the associated header file were edited to be used in this project
 *      from work obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems
 *      Description: Contains the software for handling Tiva
 *      interrupts and exceptions.
 */

/*
 * TODO: Sept 14/18
 * 1. Test MessagePassing
 *      - Create a Queue
 *      - Change UART ISR to SendMessage
 *      - Poll ReceiveMessage
 *      - Print Char to screen as it is received
 * 2.`Implement SYSTICK
 */

#include <stdlib.h>
#include <string.h>
#include "MessagePassing.h"
#include "Interrupt.h"

#define FALSE   0
#define TRUE    1

volatile unsigned UART_IDLE = TRUE;

void UART0_Init(void)
{
    volatile int wait;

    // Initialize UART0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for PORTA
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for UART0
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;                           // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;         // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;  // Enable the UART
    wait = 0;                       // wait; give UART time to enable itself.
}

void InterruptEnable(unsigned long InterruptIndex)
{
    // Indicate to CPU which device is to interrupt
    if(InterruptIndex < 32)
        NVIC_EN0_R = 1 << InterruptIndex;           // Enable the interrupt in the EN0 Register
    else
        NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

void UART0_IntEnable(unsigned long flags)
{
    // Set specified bits for interrupt
    UART0_IM_R |= flags;
}

void InitTerminal(void)
{
    UART0_Init();                               // Initialize UART0
    InterruptEnable(INT_VEC_UART0);             // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
}

void UART0_IntHandler(void)
{
    char data;
    enum Sender src;

    if (UART0_MIS_R & UART_INT_RX)
    {
        // RECV done - clear interrupt and make char available to application
        UART0_ICR_R |= UART_INT_RX;
        if( !SendMessage(UART0_DR_R, UART0_TX, MONITOR_RX) ) {
            // TODO: Handle if Queue Full
        }
    }

    if (UART0_MIS_R & UART_INT_TX)
    {
        // XMIT done - clear interrupt
        UART0_ICR_R |= UART_INT_TX;
        // Check if anymore chars in output queue
        if(ReceiveMessage(&data,&src,UART0_RX)) {
            UART0_DR_R = data;
        }
        else {
            UART_IDLE = TRUE;
        }
    }
}

void InterruptMasterEnable(void)
{
    // enable CPU interrupts
    __asm(" cpsie   i");
}

void UART_PutChar(char data)
{
    UART0_DR_R = data;
    UART_IDLE = FALSE;
}
