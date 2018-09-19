/*
 *  Monitor.h
 *
 *  Created on: Sep 16, 2018
 *  Author:     Shamus MacDonald
 *  Contains the S/W for monitoring if messages have been queued
 *  from H/W by the MessagePassing S/W. If messages are present in the
 *  queue then the appropriate application S/W will be called.
 */

#ifndef MONITOR_H_
#define MONITOR_H_

// Max Tokens from CommandLine
#define MAX_TOKENS          2

/* CommandLine Indexes
 * Token 0 is the Command
 * Token 1 is the argument if applicable */
#define COMMAND             0
#define ARGUMENT            1

// Command Line buffer length is 80
#define CMDLINE_LEN         80
//
#define DONE                -1

// Total commands supported
#define TOTAL_CMDS          1

struct CommandInfo {
    char *cmd;
    int (*func)(char *,char *);
};

void Monitor(void);
void UART_MsgHandler(char, char *, char [][]);
void SYSTICK_MsgHandler(void);

int BuildTokens(char, char *, char [][]);
int CstrAppend(char *, char);
void PrintString(char *);
int CommandHandler(char *, char *, char *);

#endif /* MONITOR_H_ */
