#include "bufferInfo.h"

#include <stdlib.h>

void infoInit(BufferInfo *info) {
    info->currentLineNumber = 0;
    info->lineCount = 0;
    info->mode = NORMAL;
    info->dirty = false;
    info->hasFileName = false;
    info->fileName = NULL;
}
