#pragma once

#include <termios.h>

typedef struct Terminal {
    struct termios old_term;
    struct termios raw_term;
}Terminal;


int terminalEnableRaw(Terminal *terminal);
int terminalDisableRaw(Terminal *terminal);
