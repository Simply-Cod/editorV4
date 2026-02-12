#include "buffer.h"
#include "bufferInfo.h"
#include "input.h"
#include "line.h"
#include "terminal.h"
#include "viewPort.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Terminal term;

int main() {

    Buffer buff;
    buffInit(&buff);

    BufferInfo info;
    infoInit(&info);

    ViewPort view;
    viewInit(&view);

    if (!terminalEnableRaw(&term)) exit(1);

    unsigned int input;
    bool quit = false;

    // Set up buffer either with provided arg or a new one

    buffCreateHead(&buff);
    buff.current = buff.head;

    while (!quit) {
        input = readInput();

        switch (info.mode) {


            case NORMAL: /*--------- NORMAL MODE ---------*/
                info.mode = INSERT;
                break;


            case INSERT: /*--------- INSERT MODE ---------*/

                switch (input) {
                    case 0:
                        break;
                    case CTRL_Q:
                        quit = true;
                        break;
                    case 32 ... 126: // asci 1 byte chars
                    case 128 ... 247: // multi bytes
                        lineInsertChar(&buff.current, input);
                        view.curX++;
                        break;
                }

                break;
        } /*------------ INPUT END ------------*/

        viewGetTerminalSize(&view);
        viewUpdate(&view);

        tmpViewDraw(&view, &buff);
        fflush(stdout);

    }


    buffFreeAll(&buff);
    terminalDisableRaw(&term);
    return 0;
}
