#pragma once

#include <stdbool.h>

enum EditorMode {
    NORMAL,
    INSERT,
    COMMANDLINE,
};
enum FileExtension {
    EXT_TXT = 0,
    EXT_C = 1,
    EXT_H = 2,
    EXT_NONE = 3,
    EXT_UNKNOWN = 4,
};
typedef struct BufferInfo {
    int lineCount;
    int currentLineNumber;
    enum EditorMode mode;
    enum FileExtension extension;
    bool dirty;
    bool hasFileName;
    bool loadFile;
    char *fileName;
}BufferInfo;


void infoInit(BufferInfo *info);
int handleArgs(BufferInfo *info, int argc, char *argv[]);
int infoCheckExtension(BufferInfo *info);
