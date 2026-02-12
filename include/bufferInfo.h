#pragma once

#include <stdbool.h>

enum EditorMode {
    NORMAL,
    INSERT,
};

typedef struct BufferInfo {
    int lineCount;
    int currentLineNumber;
    enum EditorMode mode;
    bool dirty;
    bool hasFileName;
    char *fileName;
}BufferInfo;


void infoInit(BufferInfo *info);
