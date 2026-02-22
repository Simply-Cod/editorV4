#include "bufferInfo.h"
#include "line.h"
#include <string.h>


int commandSetFileName(BufferInfo *info, Line **line) {
    if ((*line)->arrLength <= 0) return 0;

    if (info->hasFileName) return 0;

    info->fileName = strdup((*line)->buffer);
    if (!info->fileName) return 0;
    info->fileName[(*line)->arrLength] = '\0';

    if (strcmp(info->fileName, (*line)->buffer) == 0) {
        info->hasFileName = true;
        return 1;
    }

    return 0;
}
