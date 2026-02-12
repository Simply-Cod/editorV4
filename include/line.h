#pragma once

#define LINE_CAP_32 32
#define LINE_CAP_64 64
#define LINE_CAP_128 128

typedef struct Line {
    int arrLength;
    int capacity;
    int arrPos;
    char *buffer;
    struct Line *next;
    struct Line *previous;
}Line;


int lineInsertChar(Line **line, unsigned char ch);
int lineRemoveChar(Line **line);
