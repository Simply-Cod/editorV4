#include "buffer.h"
#include "bufferInfo.h"
#include "input.h"
#include "line.h"
#include "terminal.h"
#include "viewPort.h"
#include "motions.h"
#include "command.h"
#include "notification.h"

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

    Line *command = malloc(sizeof(Line));
    command->buffer = malloc(sizeof(char) * LINE_CAP_32);
    if (!command->buffer) {
        perror("Failed to allocate for command buffer");
        exit(1);
    }
    command->capacity = LINE_CAP_32;
    command->arrLength = 0;
    command->previous = NULL;
    command->next = NULL;
    command->arrPos = 0;
    command->buffer[0] = '\0';

    enum CommandFunction cmdFunction = CMD_NONE;
    int cmdViewCurX = 0;

    Notification notif;
    notif.type = NOTIFY_NONE;
    notif.msg = malloc(sizeof(char) * 32);
    notif.started = false;
    if (!notif.msg) {
        perror("Failed to allocate for notifications");
        exit(1);
    }

    if (!terminalEnableRaw(&term)) exit(1);

    unsigned int input;
    bool quit = false;


    // Set up buffer either with provided arg or a new one
    buffCreateHead(&buff, &info);
    if (info.hasFileName && info.loadFile) {
        if (!buffLoadFromFile(&buff, &info))
            notifySet(&notif, NOTIFY_ERROR, "buffLoadFromFile failed");
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
                    case '!':
                        char debug_msg[32];
                        snprintf(debug_msg, sizeof(debug_msg), "%s", info.fileName);
                        notifySet(&notif, NOTIFY_ERROR, debug_msg);
                            break;
                    case '?':
                            if (info.hasFileName)
                                notifySet(&notif, NOTIFY_ERROR, "File name is set");
                            else
                                notifySet(&notif, NOTIFY_ERROR, "filename bool not set");
                            break;
                    case 'i':
                        info.mode = INSERT;
                        break;
                    case 'a':
                        info.mode = INSERT;
                        if (lineMoveRight(&buff.current)) {
                            buff.prefArrPos = buff.current->arrPos;
                        }
                        break;
                    case LEFT:
                    case 'h':
                        if (lineMoveLeft(&buff.current)) {
                            buff.prefArrPos = buff.current->arrPos;
                        }
                        break;
                    case RIGHT:
                    case 'l':
                        if (lineMoveRight(&buff.current)) {
                            buff.prefArrPos = buff.current->arrPos;
                        }
                        break;
                    case UP:
                    case 'k':
                        if (buff.current->previous != NULL) {
                            buff.current = buff.current->previous;
                            info.currentLineNumber--;
                            bufferSetPrefArrPos(&buff);
                        }
                        break;
                    case CTRL_U:
                        motion_scrollHalfPageUp(&view, &buff, &info);
                        view.render = RENDER_FULL;
                        break;
                    case DOWN:
                    case 'j':
                        if (buff.current->next != NULL) {
                            buff.current = buff.current->next;
                            info.currentLineNumber++;

                            bufferSetPrefArrPos(&buff);
                        }
                        break;
                    case CTRL_D:
                        motion_scrollHalfPageDown(&view, &buff, &info);
                        view.render = RENDER_FULL;
                        break;
                    case 'o':
                        buffAddLineBelowCurrent(&buff, &info);
                        info.mode = INSERT;
                        info.dirty = true;
                        view.render = RENDER_FULL;
                        break;
                    case 'O':
                        bufferAddLineAboveCurrent(&buff, &info);
                        info.mode = INSERT;
                        info.dirty = true;
                        view.render = RENDER_FULL;
                        break;
                    case ':':
                        info.mode = COMMANDLINE;
                        break;
                    case CTRL_S:
                        if (!info.hasFileName) {
                            info.mode = COMMANDLINE;
                            cmdFunction = CMD_GET_FILENAME;
                        } else {
                            if (buffWriteFile(&buff, info.fileName)) {
                                info.dirty = false;
                                notifySet(&notif, NOTIFY_SAVED, "File saved successfully!");
                            }
                        }
                        break;
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
                    case '\t':
                    case 32 ... 126: // ascii 1 byte range
                    case 128 ... 247: // multi bytes range
                        lineInsertChar(&buff.current, input);
                        buff.prefArrPos = buff.current->arrPos;
                        info.dirty = true;
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
                            view.render = RENDER_FULL;

                        } else {

                            if (lineRemoveChar(&buff.current)) {
                                buff.prefArrPos = buff.current->arrPos;
                            }
                        }
                        info.dirty = true;
                        break;
                    case LEFT:
                        if (lineMoveLeft(&buff.current)) {
                            buff.prefArrPos = buff.current->arrPos;
                        }
                        break;
                    case RIGHT:
                        if (lineMoveRight(&buff.current)) {
                            buff.prefArrPos = buff.current->arrPos;
                        }
                        break;
                    case UP:
                        if (buff.current->previous != NULL) {
                            buff.current = buff.current->previous;
                            info.currentLineNumber--;
                            bufferSetPrefArrPos(&buff);
                        }
                        break;
                    case DOWN:
                        if (buff.current->next != NULL) {
                            buff.current = buff.current->next;
                            info.currentLineNumber++;
                            bufferSetPrefArrPos(&buff);
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
                        view.render = RENDER_FULL;
                        info.dirty = true;
                        break;
                    case CTRL_S:
                        if (!info.hasFileName) {
                            info.mode = COMMANDLINE;
                            cmdFunction = CMD_GET_FILENAME;
                        } else {
                            if (buffWriteFile(&buff, info.fileName)) {
                                info.dirty = false;
                                notifySet(&notif, NOTIFY_SAVED, "File saved successfully!");
                            }
                        }
                        break;
                }

                break;
            case COMMANDLINE: /*----------------- COMMANDLINE MODE -------------------*/
                switch (input) {
                    case 0:
                        break;
                    case ESC:
                        info.mode = NORMAL;
                        command->buffer[0] = '\0';
                        command->arrLength = 0;
                        command->arrPos = 0;
                        cmdViewCurX = 0;
                        break;
                    case 32 ... 126: // ascii 1 byte range
                    case 128 ... 247: // multi bytes range
                        lineInsertChar(&command, input);
                        cmdViewCurX++;
                        break;
                    case BACKSPACE:
                        if (command->arrPos <= 0) break;

                        lineRemoveChar(&command);
                        cmdViewCurX--;
                        break;
                    case LEFT:
                        if (lineMoveLeft(&command)) {
                            cmdViewCurX--;
                        }
                        break;
                    case RIGHT:
                        if (lineMoveRight(&command)) {
                            cmdViewCurX++;
                        }
                    case ENTER:
                        switch (cmdFunction) {
                            case CMD_NONE:
                                break;
                            case CMD_GET_FILENAME:
                                commandSetFileName(&info, &command);

                                if (info.hasFileName && info.dirty) {
                                    buffWriteFile(&buff, info.fileName);
                                    info.dirty = false;
                                    cmdFunction = CMD_NONE;
                                    notifySet(&notif, NOTIFY_SAVED, "File saved successfully!");
                                }
                                break;
                            case CMD_GET_COMMAND:
                                break;
                        }
                        info.mode = NORMAL;
                        command->buffer[0] = '\0';
                        command->arrLength = 0;
                        command->arrPos = 0;
                        cmdViewCurX = 0;
                        break;
                }
                break;
        } /*------------ MODE END ------------*/


        char c = buff.current->buffer[buff.current->arrPos];

        if (buff.current->arrLength > 0 && c == '\0' && info.mode == NORMAL) {
            lineMoveLeft(&buff.current);
        }
        if (notif.type == NOTIFY_CLEAR) {
            view.render = RENDER_FULL;
            notif.type = NOTIFY_NONE;
        }

        viewGetTerminalSize(&view);
        viewCorrectCursor(&view, &buff);
        viewUpdate(&view, &info);

        write(STDOUT_FILENO, "\x1b[?25l", 6); // Hide cursor
        if (info.mode == NORMAL || info.mode == INSERT) {
            viewDraw(&view, &buff, &info);
            viewDrawStatusLine(&view, &buff, &info);

            // Testing
            if (notif.type != NOTIFY_NONE) {
                notify(&notif, &view);
            }

            viewPlaceCursorOnCurrent(&view);
            viewSetCursorStyle(&info);
        } else if (info.mode == COMMANDLINE) {
            write(STDOUT_FILENO, "\x1b[6 q", 5); // I cursor
            viewDrawCommandStatus(&command, cmdFunction, &view);
            viewDrawCommand(&command, cmdFunction, &view);
            viewCommandSetCursor(command, &view, cmdViewCurX);
        }

        write(STDOUT_FILENO, "\x1b[?25h", 6); // Show cursor
        fflush(stdout);

        view.oldLineNumb = view.currentLineNumb;
        if (view.render != RENDER_WELCOME)
            view.render = RENDER_LINE;
    }

    write(STDOUT_FILENO, "\x1b[2 q", 5); // Block
    write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7); // clear

    if (info.fileName != NULL) {
        free(info.fileName);
    }
    if (command->buffer) {
        free(command->buffer);
    }
    free(notif.msg);
    buffFreeAll(&buff);
    terminalDisableRaw(&term);
    return 0;
}
