#include "bufferInfo.h"
#include "line.h"
#include <stdlib.h>
#include <string.h>


int commandSetFileName(BufferInfo *info, Line *line) {
    if (line->arrLength <= 0) return 0;

    if (info->hasFileName) return 0;

    info->fileName = malloc(sizeof(char) * line->arrLength + 1);

    if (!info->fileName) return 0;

    strncpy(info->fileName, line->buffer, strlen(line->buffer) + 1);

    if (strcmp(info->fileName, line->buffer) == 0) {
        info->hasFileName = true;
        return 1;
    }

    return 0;
}
