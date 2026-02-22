#include "line.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int lineInsertChar(Line **line, unsigned char ch) {
    Line *tmp = *line;


    if (tmp->arrLength + 5 > tmp->capacity) {
        char *new = realloc(tmp->buffer, tmp->capacity + LINE_CAP_32);

        if (!new) return 0;
        tmp->buffer = new;
        tmp->capacity += LINE_CAP_32;
        tmp->buffer[tmp->arrLength] = '\0';
    }

    switch (ch) {
        case '\t':
        case 32 ... 126: // 1 byte
            memmove(&tmp->buffer[tmp->arrPos + 1], &tmp->buffer[tmp->arrPos], tmp->arrLength - tmp->arrPos);

            tmp->buffer[tmp->arrPos] = ch;
            tmp->arrPos++;
            tmp->arrLength++;
            break;

        case 192 ... 223: // 2 byte's
            unsigned char ch2;
            if (read(STDIN_FILENO, &ch2, 1) == 0) return 0;

            if (ch2 < 128 || ch2 > 191) return 0;
            memmove(&tmp->buffer[tmp->arrPos + 2], &tmp->buffer[tmp->arrPos], tmp->arrLength - tmp->arrPos);

            tmp->buffer[tmp->arrPos] = ch;
            tmp->buffer[tmp->arrPos + 1] = ch2;

            tmp->arrLength += 2;
            tmp->arrPos += 2;
            break;
        case 224 ... 239: // 3 byte's
            break;
        case 240 ... 247: // 4 byte's
            break;
    }
    tmp->buffer[tmp->arrLength] = '\0';
    *line = tmp;
    return 1;
}

int lineRemoveChar(Line **line) {

    Line *tmp = *line;

    if (tmp->arrPos <= 0 || tmp->arrLength <= 0) return 0;

    int rm = 1;
    int n = tmp->arrPos - 1;

    while ((unsigned char)tmp->buffer[n] >= 128 && (unsigned char)tmp->buffer[n] <= 191 && tmp->arrPos > 0) {
        rm++;
        n--;
    }

    memmove(&tmp->buffer[tmp->arrPos - rm], &tmp->buffer[tmp->arrPos], tmp->arrLength - tmp->arrPos + 1);
    tmp->arrLength -= rm;
    tmp->arrPos -= rm;

    tmp->buffer[tmp->arrLength] = '\0';

    if (tmp->capacity > tmp->arrLength + (LINE_CAP_32 * 2)) {
        char *new = realloc(tmp->buffer, tmp->capacity - LINE_CAP_32);

        if (!new) return 0;
        tmp->buffer = new;
        tmp->capacity -= LINE_CAP_32;
        tmp->buffer[tmp->arrLength] = '\0';
    }

    *line = tmp;

    return 1;
}

int lineMoveLeft(Line **line) {
    Line *tmp = *line;

    if (tmp->arrPos <= 0) return 0;

    tmp->arrPos--;

    for (;;) {
        if (tmp->arrPos <= 0) break;

        if ((unsigned char)tmp->buffer[tmp->arrPos] >= 128 && (unsigned char)tmp->buffer[tmp->arrPos] <= 191) {
            tmp->arrPos--;
        } else {
            break;
        }
    }
    *line = tmp;
    return 1;
}

int lineMoveRight(Line **line) {
    Line *tmp = *line;

    if (tmp->buffer[tmp->arrPos] == '\0') return 0;

    tmp->arrPos++;

    for (;;) {
        if (tmp->buffer[tmp->arrPos] == '\0') break;

        if ((unsigned char)tmp->buffer[tmp->arrPos] >= 128 && (unsigned char)tmp->buffer[tmp->arrPos] <= 191) {
            tmp->arrPos++;
        } else {
            break;
        }
    }
    *line = tmp;
    return 1;
}

int lineMoveBuff(Line **src, Line **dest, int count) {
    Line *current = *src;
    Line *next = *dest;

    if (next == NULL) return 0;

    if (next->arrLength + count > next->capacity - 2) {
        int cap = next->capacity;

        while (cap < (next->capacity + count) + 5) {
            cap += LINE_CAP_32;
        }

        char *tmp = realloc(next->buffer, cap);
        if (!tmp) return 0;

        next->buffer = tmp;
        next->buffer[next->arrLength] = '\0';
        next->capacity = cap;
    }

    strncpy(&next->buffer[next->arrLength], &current->buffer[current->arrPos], count);

    current->arrLength -= count;
    current->buffer[current->arrLength] = '\0';

    next->arrLength += count;
    next->buffer[next->arrLength] = '\0';
    return 1;
}
