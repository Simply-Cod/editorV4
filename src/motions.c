#include "buffer.h"
#include "viewPort.h"
#include "bufferInfo.h"
#include <unistd.h>

void motion_scrollHalfPageDown(ViewPort *view, Buffer *buff, BufferInfo *info) {

    if (view->height <= 1) return;

    for (int i = 0; i < view->height / 2; i++) {
        if (buff->current->next == NULL) break;

        buff->current = buff->current->next;
        info->currentLineNumber++;
    }
    if (info->currentLineNumber > view->height / 2)
        view->topLine = info->currentLineNumber - (view->height / 2);
}

void motion_scrollHalfPageUp(ViewPort *view, Buffer *buff, BufferInfo *info) {

    if (view->height <= 1) return;

    for (int i = 0; i < view->height / 2; i++) {
        if (buff->current->previous == NULL) break;

        buff->current = buff->current->previous;
        info->currentLineNumber--;
    }
    if (info->currentLineNumber > view->height / 2)
        view->topLine = info->currentLineNumber - (view->height / 2);
}
