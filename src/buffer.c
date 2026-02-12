#include "buffer.h"
#include "line.h"
#include <stdlib.h>

void buffInit(Buffer *buff) {
    buff->head = NULL;
    buff->current = NULL;
}

int buffCreateHead(Buffer *buff) {

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
