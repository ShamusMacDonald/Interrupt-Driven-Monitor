/*
 * MessagePassing.c
 *
 *  Created on: Sep 13, 2018
 *      Author: Shamus MacDonald
 */

#include "MessagePassing.h"
#include "Interrupt.h"
#include "Systick.h"

/* Global                               *
 * Signals that the UART can transmit   */
extern volatile unsigned UART_IDLE;

/* InputQueue contains messages coming from either the UART or SYSTICK. *
 * The message info is received by the monitor.                         */
PRIVATE struct Queue InputQueue = {.capacity=QSIZE, .head=0, .tail=0, .size=0 };

/* OutputQueue contains messages coming from the monitor.                *
 * The message info is received by the UART which transmits the data     *
 * to the users terminal.                                                */
PRIVATE struct Queue OutputQueue = {.capacity=QSIZE, .head=0, .tail=0, .size=0 };

int SendMessage(char data, enum Sender src, enum Receiver dst)
{
    struct Message msg = { src, dst, data };
    struct Queue *Q = &InputQueue;

    if(dst == UART0_RX) {
        // IF UART_IDLE THEN UART_PutChar
        if(UART_IDLE) {
            UART_PutChar(data);
            return 1;
        }
        // ELSE add to output queue
        else {
            Q = &OutputQueue;
        }
    }

    if(Q->size == Q->capacity) {
        return 0;
    }

    Q->fifo[Q->tail] = msg;
    Q->tail = (Q->tail + 1) % Q->capacity;
    Q->size++;

    return 1;
}

int ReceiveMessage(char *data, enum Sender *src, enum Receiver dst)
{
    struct Queue *Q = &InputQueue;

    // if receiver is the uart then use output q
    if(dst == UART0_RX) {
        Q = &OutputQueue;
    }
    // if q empty return 0
    if(Q->size == 0) {
        return 0;
    }
    // get msg info
    *data = Q->fifo[Q->head].data;
    *src = Q->fifo[Q->head].source;
    // adjust queue
    Q->head = (Q->head + 1) % Q->capacity;
    Q->size--;

    return 1;
}

void InitMsgPassing(void)
{
    InitTerminal();
    InitClock();
    InterruptMasterEnable();
}
