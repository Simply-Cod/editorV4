#pragma once

#include "bufferInfo.h"
#include "line.h"

typedef struct Buffer {
    Line *head;
    Line *current;
}Buffer;


void buffInit(Buffer *buff);
int buffCreateHead(Buffer *buff, BufferInfo *info);
void buffFreeAll(Buffer *buff);
int buffAddLineBelowCurrent(Buffer *buff, BufferInfo *info);
