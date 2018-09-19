/*
 *  Monitor.c
 *
 *  Created on: Sep 16, 2018
 *  Author:     Shamus MacDonald
 *  Contains the S/W for monitoring if messages have been queued
 *  from H/W by the MessagePassing S/W. If messages are present in the
 *  queue then the appropriate application S/W will be called.
 */
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "Monitor.h"
#include "Application.h"
#include "MessagePassing.h"

#define ERR_MSG  "?\r"

void Monitor(void)
{
    char data;
    enum Sender src;
    char CommandLine[CMDLINE_LEN] = {};
    char tokens[MAX_TOKENS][CMDLINE_LEN/2] = {};

    InitMsgPassing();
    PrintString(">");

    while(1) {
        while( ReceiveMessage(&data, &src, MONITOR_RX) ) {
            if(src == UART0_TX) {
                UART_MsgHandler(data,CommandLine,tokens);
            }
            else if(src == SYSTICK_TX) {
                SYSTICK_MsgHandler();
            }
        }
    }
}

void PrintString(char *str)
{
    int i = 0;
    while( i < strlen(str) ) {
        SendMessage(str[i],MONITOR_TX,UART0_RX);
        i++;
    }
}

/* UART_EventHandler is responsible for handling data received        *
 * from the UART by building the received characters into a cstring   *
 * that can be compared with application commands.                    */
void UART_MsgHandler(char data, char *CommandLine, char tokens[MAX_TOKENS][CMDLINE_LEN/2])
{
    char OutStr[12];

    if(BuildTokens(data,CommandLine,tokens) == DONE) {
        if(CommandHandler(tokens[COMMAND],tokens[ARGUMENT],OutStr)) {
            PrintString(OutStr);
        }
        else {
            PrintString(ERR_MSG);
        }
        PrintString(">");

    }
}

void SYSTICK_MsgHandler(void)
{
    UpdateTime();
}

int CommandHandler(char *command, char *argument, char *OutStr)
{
    struct CommandInfo CmdInfo[TOTAL_CMDS] = {{"TIME",time}};
    int i = 0;

    while(i < TOTAL_CMDS) {
        // if command is valid
        if(strcmp(CmdInfo[i].cmd,command) == 0 ) {
            // If invalid command, break and ret 0
            if( CmdInfo[i].func(argument,OutStr) ) {
                return 1;
            }
            break;
        }
        i++;
    }
    return 0;
}

int BuildTokens(char data, char *CommandLine, char tokens[MAX_TOKENS][CMDLINE_LEN/2])
{
    char *cmd;
    char *arg;

    // Build string until Buffer Full or '\r'
    if( data == '\r' || !CstrAppend(CommandLine,data) ) {
      // Tokenize String, only takes 2 tokens, ignores others if present
      cmd = strtok(CommandLine," ");
      arg = strtok(NULL," ");
      // Copy first token into command
      strcpy(tokens[COMMAND] , cmd );
      if(arg == NULL) {
        // If no arg, create empty arg token
        strcpy(tokens[ARGUMENT],"\0");
      }
      else {
        // Else copy second token into argument
        strcpy(tokens[ARGUMENT], arg);
      }
 //     PrintString(">");
      PrintString(tokens[COMMAND]);
      PrintString(" ");
      PrintString(tokens[ARGUMENT]);
      PrintString("\r");
      CommandLine[0] = '\0';
      return DONE;
    }
    return !DONE;
}

int CstrAppend(char *string, char data)
{
    int StringLen;

    StringLen = strlen(string);
    if(StringLen == CMDLINE_LEN) {
        // If buffer full, return 0
        return 0;
    }
    string[StringLen] = (char)toupper(data);
    string[StringLen+1] = '\0';

    return 1;
}



