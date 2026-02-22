#pragma once

#include "bufferInfo.h"
#include "line.h"

typedef struct Buffer {
    int prefArrPos;
    Line *head;
    Line *current;
}Buffer;


void buffInit(Buffer *buff);
int buffCreateHead(Buffer *buff, BufferInfo *info);
void buffFreeAll(Buffer *buff);
void bufferDeleteLine(Buffer *buff, BufferInfo *info, Line **toDelete);

int buffAddLineBelowCurrent(Buffer *buff, BufferInfo *info);
int bufferAddLineAboveCurrent(Buffer *buff, BufferInfo *info);


int buffLoadFromFile(Buffer *buff, BufferInfo *info);
void bufferSetPrefArrPos(Buffer *buff);
