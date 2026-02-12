#pragma once

#include "line.h"

typedef struct Buffer {
    Line *head;
    Line *current;
}Buffer;


void buffInit(Buffer *buff);
int buffCreateHead(Buffer *buff);
void buffFreeAll(Buffer *buff);
