#include "viewPort.h"
#include "buffer.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void viewInit(ViewPort *view) {
    view->curX = 0;
    view->curY = 0;
    view->height = 0;
    view->width = 0;
    view->topLine = 0;
    view->oldTopLine = 0;
    view->render = RENDER_WELCOME;
}

int viewGetTerminalSize(ViewPort *view) {

    struct winsize win;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1) return 0;

    view->width = win.ws_col;
    view->height = win.ws_row;

    return 1;
}

void viewUpdate(ViewPort *view) {

    view->height -= 2;
}

// TODO
int viewDraw(ViewPort *view, Buffer *buff) {

    return 1;
}

void tmpViewDraw(ViewPort *view, Buffer *buff) {

    Line *printPtr = buff->current;

    write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7); // Move cursor to beginning
    
    write(STDOUT_FILENO, printPtr->buffer, strlen(printPtr->buffer));

    char cursor[32];
    int n = snprintf(cursor, sizeof(cursor), "\x1b[%d;%dH", view->curY, view->curX + 1);
    write(STDOUT_FILENO, cursor, n);
}
