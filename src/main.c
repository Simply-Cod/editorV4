#include "buffer.h"
#include "bufferInfo.h"
#include "input.h"
#include "line.h"
#include "terminal.h"
#include "viewPort.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

Terminal term;

int main(int argc, char *argv[1]) {

    Buffer buff;
    buffInit(&buff);

    BufferInfo info;
    infoInit(&info);
    handleArgs(&info, argc, argv);

    ViewPort view;
    viewInit(&view);

    if (!terminalEnableRaw(&term)) exit(1);

    unsigned int input;
    bool quit = false;


    // Set up buffer either with provided arg or a new one
    buffCreateHead(&buff, &info);
    if (info.hasFileName && info.loadFile) {
        buffLoadFromFile(&buff, &info);
        buff.current = buff.head;
        view.render = RENDER_FULL;
        info.currentLineNumber = 1;

    } else {
        buff.current = buff.head;
    }

    while (!quit) {
        input = readInput();

        switch (info.mode) {


            case NORMAL: /*--------- NORMAL MODE ---------*/

                switch (input) {
                    case 0:
                        break;
                    case CTRL_Q:
                        quit = true;
                        break;
                    case 'i':
                        info.mode = INSERT;
                        break;
                    case 'a':
                        info.mode = INSERT;
                        if (lineMoveRight(&buff.current))
                            view.curX++;
                        break;
                    case LEFT:
                    case 'h':
                        if (lineMoveLeft(&buff.current)) {
                            view.curX--;
                        }
                        break;
                    case RIGHT:
                    case 'l':
                        if (lineMoveRight(&buff.current)) {
                            view.curX++;
                        }
                        break;
                    case UP:
                    case 'k':
                        if (buff.current->previous != NULL) {
                            buff.current = buff.current->previous;
                            info.currentLineNumber--;
                        }
                        break;
                    case DOWN:
                    case 'j':
                        if (buff.current->next != NULL) {
                            buff.current = buff.current->next;
                            info.currentLineNumber++;
                        }
                        break;
                    case 'o':
                        buffAddLineBelowCurrent(&buff, &info);
                        info.mode = INSERT;
                        break;
                    case 'O':
                        bufferAddLineAboveCurrent(&buff, &info);
                        info.mode = INSERT;
                        break;
                }

                char c = buff.current->buffer[buff.current->arrPos];

                if (buff.current->arrLength > 0 && c == '\0' && info.mode == NORMAL) {
                    if (lineMoveLeft(&buff.current))
                        view.curX--;
                }

                if (view.render == RENDER_WELCOME && info.mode == INSERT)
                    view.render = RENDER_FULL;

                break;   /*--------- NORMAL END ----------*/

            case INSERT: /*--------- INSERT MODE ---------*/

                switch (input) {
                    case 0:
                        break;
                    case ESC:
                        info.mode = NORMAL;
                        break;
                    case CTRL_Q:
                        quit = true;
                        break;
                    case 32 ... 126: // ascii 1 byte range
                    case 128 ... 247: // multi bytes range
                        lineInsertChar(&buff.current, input);
                        view.curX++;
                        break;
                    case BACKSPACE:
                        if (buff.current->arrPos <= 0) {
                            if (buff.current->previous == NULL) break;

                            if (buff.current->arrLength > 0) {
                                int newCurPos = buff.current->previous->arrLength;
                                lineMoveBuff(&buff.current, &buff.current->previous, buff.current->arrLength);
                                buff.current = buff.current->previous;
                                bufferDeleteLine(&buff, &info, &buff.current->next);
                                buff.current->arrPos = newCurPos;
                                info.currentLineNumber--;
                            } else {
                                int newCurPos = buff.current->previous->arrLength;
                                buff.current = buff.current->previous;
                                bufferDeleteLine(&buff, &info, &buff.current->next);
                                buff.current->arrPos = newCurPos;
                                info.currentLineNumber--;
                            }

                        } else {

                            if (lineRemoveChar(&buff.current)) {
                                view.curX--;
                            }
                        }
                        break;
                    case LEFT:
                        if (lineMoveLeft(&buff.current)) {
                            view.curX--;
                        }
                        break;
                    case RIGHT:
                        if (lineMoveRight(&buff.current)) {
                            view.curX++;
                        }
                        break;
                    case UP:
                        if (buff.current->previous != NULL) {
                            buff.current = buff.current->previous;
                            info.currentLineNumber--;
                        }
                        break;
                    case DOWN:
                        if (buff.current->next != NULL) {
                            buff.current = buff.current->next;
                            info.currentLineNumber++;
                        }
                        break;
                    case ENTER:
                        buffAddLineBelowCurrent(&buff, &info);
                        // ToDo: add RENDER_FROM_CURSOR
                        Line *l = buff.current->previous;

                        if (l->arrPos < l->arrLength) {
                            int count = l->arrLength - l->arrPos;

                            lineMoveBuff(&buff.current->previous, &buff.current, count);
                        }
                        break;
                }

                break;
        } /*------------ MODE END ------------*/


        viewGetTerminalSize(&view);
        viewCorrectCursor(&view, &buff);
        viewUpdate(&view, &info);

        viewDraw(&view, &buff, &info);
        viewSetCursorStyle(&info);
        fflush(stdout);

    }

    write(STDOUT_FILENO, "\x1b[2 q", 5); // Block
    write(STDOUT_FILENO, "\x1b[H\x1b[2K", 7); // clear

    if (info.fileName != NULL) {
        free(info.fileName);
    }

    buffFreeAll(&buff);
    terminalDisableRaw(&term);
    return 0;
}
