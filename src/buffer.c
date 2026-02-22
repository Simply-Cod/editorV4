#include "buffer.h"
#include "bufferInfo.h"
#include "line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buffInit(Buffer *buff) {
    buff->head = NULL;
    buff->current = NULL;
    buff->prefArrPos = 0;
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

int bufferAddLineAboveCurrent(Buffer *buff, BufferInfo *info) {

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

    new->next = buff->current;
    new->previous = buff->current->previous;

    if (buff->current->previous != NULL) {
        buff->current->previous->next = new;
    } else {
        buff->head = new;
    }
    buff->current->previous = new;

    info->lineCount++;
    info->dirty = true;

    buff->current = buff->current->previous;
    return 1;
}

void bufferDeleteLine(Buffer *buff, BufferInfo *info, Line **toDelete) {

    Line *l = *toDelete;

    if (buff->head->next == NULL) return;

    if (l->previous == NULL) {
        buff->head = l->next;
    } else {
        l->previous->next = l->next;
    }

    if (l->next != NULL) {
        l->next->previous = l->previous;
    }

    if (l->buffer != NULL)
        free(l->buffer);

    free(l);

    info->lineCount--;
    info->dirty = true;
}

int buffLoadFromFile(Buffer *buff, BufferInfo *info) {

    FILE *file;
    file = fopen(info->fileName, "r");

    if (file == NULL) return 0;

    buff->current = buff->head;
    info->lineCount = 1;
    char buf[1024];


    while (fgets(buf, sizeof(buf), file)) {
        size_t len = strlen(buf);

        if (len == 0) continue;

        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
            len--;
        }

        buff->current->buffer = malloc(len + 1);

        if (!buff->current->buffer) {
            fclose(file);
            buffFreeAll(buff);
            info->lineCount = 0;
            return 0;
        }

        strcpy(buff->current->buffer, buf);

        buff->current->capacity = len + 1;
        buff->current->arrLength = len;
        buff->current->arrPos = 0;

        info->lineCount++;

        if (!feof(file)) {

            buffAddLineBelowCurrent(buff, info);

            if (buff->current->next) {
                buff->current = buff->current->next;
            }
        }
    }
    fclose(file);

    if (buff->current && buff->current->arrLength == 0) {
        buff->current = buff->current->previous;

        bufferDeleteLine(buff, info, &buff->current->next);
        buff->current->next = NULL;
    }


    return 1;
}

void bufferSetPrefArrPos(Buffer *buff) {

    if (buff->prefArrPos > buff->current->arrLength) {
        buff->current->arrPos = buff->current->arrLength;
    } else {
        buff->current->arrPos = buff->prefArrPos;
    }

}
