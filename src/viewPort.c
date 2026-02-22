#include "viewPort.h"
#include "buffer.h"
#include "bufferInfo.h"

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
    view->currentLineNumb = 0;
    view->oldLineNumb = 0;
    view->render = RENDER_WELCOME;
}

int viewGetTerminalSize(ViewPort *view) {

    struct winsize win;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1) return 0;

    view->width = win.ws_col;
    view->height = win.ws_row;

    return 1;
}

void viewUpdate(ViewPort *view, BufferInfo *info) {


    if (info->currentLineNumber <= view->topLine)
        view->topLine = info->currentLineNumber - 1;
    else if (info->currentLineNumber >= view->topLine + (view->height - 2)) // - 2 to have space for the status line
        view->topLine = info->currentLineNumber - (view->height - 2);


    view->curY = info->currentLineNumber - view->topLine;

    if (view->oldTopLine != view->topLine) {
        view->render = RENDER_FULL;
        view->oldTopLine = view->topLine;
    }

    view->currentLineNumb = info->currentLineNumber;
}

// TODO
int viewDraw(ViewPort *view, Buffer *buff, BufferInfo *info) {

    switch (view->render) {
        case RENDER_WELCOME:
            viewDrawWelcome(view);
            return 1;
        case RENDER_FULL:
            viewDrawFull(view, buff, info);
            break;
        case RENDER_FROM_CURSOR:
            break;
        case RENDER_LINE:
            viewDrawLine(view, buff, info);
            break;
    }

    return 1;
}

void viewPlaceCursorOnCurrent(ViewPort *view) {

    const int offset = 6;
    char cursor[32];
    int n = snprintf(cursor, sizeof(cursor), "\x1b[%d;%dH", view->curY, view->curX + offset);
    write(STDOUT_FILENO, cursor, n);
}

// Welcome screen
void viewDrawWelcome(ViewPort *view) {
    write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7);

    int msgY = view->height / 2;
    int msgX = view->width / 2;
    char msg[32];
    for (int i = 0; i < view->height; i++) {
        if (i == msgY) {

            int n = snprintf(msg, sizeof(msg), "\x1b[%dG", msgX);
            write(STDOUT_FILENO, "~", 1);
            write(STDOUT_FILENO, msg, n);
            write(STDOUT_FILENO, "Welcome\n", 9);
        } else if (i == msgY + 2) {
            int n = snprintf(msg, sizeof(msg), "\x1b[%dG", msgX - 5);
            write(STDOUT_FILENO, "~", 1);
            write(STDOUT_FILENO, msg, n);
            write(STDOUT_FILENO, "type Ctrl-Q to quit\n", 21);

        } else {
            write(STDOUT_FILENO, "~\n", 2);
        }

    }
    // Place cursor
    const int offset = 6;
    char cursor[32];
    int n = snprintf(cursor, sizeof(cursor), "\x1b[%d;%dH", view->curY, view->curX + offset);
    write(STDOUT_FILENO, cursor, n);

    fflush(stdout);
}


void viewDrawFull(ViewPort *view, Buffer *buff, BufferInfo *info) {

    int index = 0;
    Line *printPtr = buff->head;

    if (view->topLine >= 1) {

        while (printPtr != NULL && index < view->topLine) {
            printPtr = printPtr->next;
            index++;
        }

    }
    write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7); // Move to beginning and clear

    char lineNumb[32];
    int n = 0;
    for (int i = 0; i < view->height - 2; i++) {

        if (printPtr == buff->current)
            n = snprintf(lineNumb, sizeof(lineNumb), "\x1b[1m%4d\x1b[0m ", (index + i) + 1);
        else
            n = snprintf(lineNumb, sizeof(lineNumb), "\x1b[38;5;102m%4d\x1b[0m ", (index + i) + 1);

        if (printPtr != NULL) {
            write(STDOUT_FILENO, lineNumb, n);
            viewPrintLine(printPtr);
            write(STDOUT_FILENO, "\n", 1);
            printPtr = printPtr->next;
        } else {
            write(STDOUT_FILENO, "~\n", 2);
        }
    }
}

void viewDrawLine(ViewPort *view, Buffer *buff, BufferInfo *info) {

    if (view->oldLineNumb != view->currentLineNumb) {
        write(STDOUT_FILENO, "\x1b[0G", 4);
        write(STDOUT_FILENO, "    ", 4);
        write(STDOUT_FILENO, "\x1b[0G", 4);

        char lineNumb[32];
        int n = snprintf(lineNumb, sizeof(lineNumb), "\x1b[38;5;102m%4d\x1b[0m", (view->oldLineNumb));
        write(STDOUT_FILENO, lineNumb, n);
    }

    char move[32];
    int nMove = snprintf(move, sizeof(move), "\x1b[%d;0H\x1b[0K", view->curY);
    write(STDOUT_FILENO, move, nMove);


    char lineNumb[32];
    int n = snprintf(lineNumb, sizeof(lineNumb), "\x1b[1m%4d\x1b[0m ", (view->currentLineNumb));
    write(STDOUT_FILENO, lineNumb, n);

    viewPrintLine(buff->current);
}

void viewDrawStatusLine(ViewPort *view, Buffer *buff, BufferInfo *info) {

    char pos[32];
    int nPos = snprintf(pos, sizeof(pos), "\x1b[%d;0H\x1b[2K", view->height - 1);
    write(STDOUT_FILENO, pos, nPos);

    char mode[64];
    int nMode = snprintf(mode, sizeof(mode), "-- %s MODE\x1b[0m --\t",
            (info->mode == INSERT) ? "\x1b[41mINSERT" : "\x1b[44mNORMAL");
    write(STDOUT_FILENO, mode, nMode);

    char name[128];
    if (info->hasFileName) {
        strncpy(name, info->fileName, strlen(info->fileName));
    } else {
        strcpy(name, "<no name>");
    }

    int nameLen = strlen(name);
    write(STDOUT_FILENO, name, nameLen);

    if (info->dirty) {
        write(STDOUT_FILENO, " [+]", 4);
    }

}

void viewPrintLine(Line *line) {

    if (line->arrLength <= 0) return;

    for (int i = 0; i < line->arrLength; i++) {
        write(STDOUT_FILENO, &line->buffer[i], 1);
    }

}

void tmpViewDraw(ViewPort *view, Buffer *buff) {

    Line *printPtr = buff->current;

    write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7); // Move cursor to beginning

    write(STDOUT_FILENO, printPtr->buffer, strlen(printPtr->buffer));

    char test[64];
    int t = snprintf(test, sizeof(test), "\n\ncurrent char: %c", buff->current->buffer[buff->current->arrPos]);
    write(STDOUT_FILENO, test, t);

    char cursor[32];
    int n = snprintf(cursor, sizeof(cursor), "\x1b[%d;%dH", view->curY, view->curX + 1);
    write(STDOUT_FILENO, cursor, n);
}

void viewSetCursorStyle(BufferInfo *info) {

    if (info->mode == NORMAL) {
        write(STDOUT_FILENO, "\x1b[2 q", 5); // Block
    } else {
        write(STDOUT_FILENO, "\x1b[6 q", 5); // I
    }

}

void viewCorrectCursor(ViewPort *view, Buffer *buff) {

    if (buff->current->arrLength <= 0) {
        view->curX = 0;
        return;
    }

    int visualChars = buff->current->arrPos;

    for (int i = 0; i < buff->current->arrPos; i++) {
        unsigned char n = buff->current->buffer[i];

        if (n >= 128 && n <= 191)
            visualChars--;
    }

    view->curX = visualChars;

}

int viewMoveCurOnY(Line *oldLine, Line *newLine) {

    if (oldLine->arrPos > newLine->arrLength) {
        return newLine->arrLength;
    } else {
        return oldLine->arrPos;
    }
}
