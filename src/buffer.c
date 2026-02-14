#include "buffer.h"
#include "bufferInfo.h"
#include "line.h"
#include <stdlib.h>

void buffInit(Buffer *buff) {
    buff->head = NULL;
    buff->current = NULL;
}

int buffCreateHead(Buffer *buff, BufferInfo *info) {

    if (buff->head != NULL) return 0;

    buff->head = malloc(sizeof(Line));

    if (!buff->head) return 0;

    buff->head->buffer = malloc(sizeof(char) * LINE_CAP_32);
    if (!buff->head->buffer) {
        free(buff->head);
        return 0;
    }

    buff->head->buffer[0] = '\0';
    buff->head->arrPos = 0;
    buff->head->arrLength = 0;
    buff->head->capacity = LINE_CAP_32;
    buff->head->next = NULL;
    buff->head->previous = NULL;

    info->lineCount = 1;
    info->currentLineNumber = 1;

    return 1;
}

void buffFreeAll(Buffer *buff) {
    if (buff->head == NULL) return;

    Line *current = buff->head;
    Line *next;

    while (current != NULL) {
        next = current->next;

        if (current->buffer != NULL) {
            free(current->buffer);
        }
        free(current);

        current = next;
    }
}

int buffAddLineBelowCurrent(Buffer *buff, BufferInfo *info) {

    Line *new = malloc(sizeof(Line));
    if (!new) return 0;

    new->buffer = malloc(sizeof(char) * LINE_CAP_32);
    if (!new->buffer) {
        free(new);
        return 0;
    }
    new->arrPos = 0;
    new->arrLength = 0;
    new->capacity = LINE_CAP_32;
    new->buffer[0] = '\0';
    new->previous = buff->current;

    if (buff->current == NULL)
        new->next = NULL;
    else
        new->next = buff->current->next;

    if (buff->current->next != NULL)
        buff->current->next->previous = new;

    buff->current->next = new;

    info->lineCount++;
    info->currentLineNumber++;
    info->dirty = true;

    buff->current = buff->current->next;

    return 1;
}
