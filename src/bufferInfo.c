#include "bufferInfo.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void infoInit(BufferInfo *info) {
    info->currentLineNumber = 0;
    info->lineCount = 0;
    info->mode = NORMAL;
    info->dirty = false;
    info->hasFileName = false;
    info->loadFile = false;
    info->fileName = NULL;
}

int handleArgs(BufferInfo *info, int argc, char *argv[]) {

    if (argc <= 1) {
        return 1;
    } else if (argc == 2) {

        struct stat st;

        if (stat(argv[1], &st) == 0) {
            int len = strlen(argv[1]);

            if (S_ISREG(st.st_mode)) { // is a file

                info->fileName = strndup(argv[1], len);
                info->fileName[len] = '\0';

                if (!info->fileName) return 0;

                info->hasFileName = true;
                info->loadFile = true;
                return 1;
            } else if (S_ISDIR(st.st_mode)) { // is a directory

                if (chdir(argv[1]) != 0)
                    return 0;
                else
                    return 1;
            } else { // Unknown

                return 0;
            }
        } else { // set as file name

            int len = strlen(argv[1]);
            info->fileName = strndup(argv[1], strlen(argv[1]));
            if (!info->fileName) return 0;

            info->fileName[len] = '\0';
            info->hasFileName = true;
            return 1;
        }
    }
    return 1;
}

int infoCheckExtension(BufferInfo *info) {

    if (!info->hasFileName) {
        info->extension = EXT_NONE;
        return 1;
    }
    if (info->fileName == NULL) {
        info->extension = EXT_NONE;
        return 1;
    }

    const char *ext = strrchr(info->fileName, '.');

    if (ext && ext != info->fileName) {
        int exCount = 3;
        char *extensions[] = {
            ".txt",
            ".c",
            ".h"
        };

        for (int i = 0; i < exCount; i++) {
            if (strcmp(ext, extensions[i])) {
                info->extension = i;
                return 1;
            }
        }

    } else {
        info->extension = EXT_NONE;
        return 0;
    }

    return 1;
}
