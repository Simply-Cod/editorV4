#pragma once

#include "viewPort.h"
#include <time.h>
#include <stdbool.h>

// Buffer errors
#define ERR_BUFF_CREATE_HEAD_NOT_NULL 1000
#define ERR_BUFF_CREATE_HEAD_NOT_NULL_MSG "buffCreateHead: Head is not null"
#define ERR_BUFF_CREATE_HEAD_NULL 1001
#define ERR_BUFF_CREATE_HEAD_NULL_MSG "buffCreateHead: Head is null"
#define ERR_BUFF_CREATE_HEAD_BUFFER_NULL 1002
#define ERR_BUFF_CREATE_HEAD_BUFFER_NULL_MSG "buffCreateHead: Head->buffer is null"

#define ERR_BUFF_ADD_LINE_NEW_NULL 1010
#define ERR_BUFF_ADD_LINE_NEW_NULL_MSG "buffAddLine: Line is null"
#define ERR_BUFF_ADD_LINE_NEW_NULL_BUFFER 1011
#define ERR_BUFF_ADD_LINE_NEW_NULL_BUFFER_MSG "buffAddLine: Line->buffer is null"

#define ERR_BUFF_LOAD_FILE_FILE_NULL 1020
#define ERR_BUFF_LOAD_FILE_FILE_NULL_MSG "buffLoadFromFile: file is null"
#define ERR_BUFF_LOAD_FILE_CURRENT_BUFFER_NULL 1021
#define ERR_BUFF_LOAD_FILE_CURRENT_BUFFER_NULL_MSG "buffLoadFromFile: current buffer is null"

#define ERR_BUFF_WRITE_FILE_NO_NAME 1030
#define ERR_BUFF_WRITE_FILE_NO_NAME_MSG "buffWriteFile: missing name"
#define ERR_BUFF_WRITE_FILE_NULL 1031
#define ERR_BUFF_WRITE_FILE_NULL_MSG "buffWriteFile: file is null"

// Buffer info
#define ERR_INFO_HANDLE_ARGS_MEM_FAIL 1500
#define ERR_INFO_HANDLE_ARGS_MEM_FAIL_MSG "handleArgs: failed to allocate memory for name"
#define ERR_INFO_HANDLE_ARGS_CHDIR_FAIL 1501
#define ERR_INFO_HANDLE_ARGS_CHDIR_FAIL_MSG "handleArgs: failed to change dir"
#define ERR_INFO_HANDLE_ARGS_UNKNOWN 1502
#define ERR_INFO_HANDLE_ARGS_UNKNOWN_MSG "handleArgs: unknown fileType"

enum NotifyType {
    NOTIFY_NONE,
    NOTIFY_ERROR,
    NOTIFY_HINT,
    NOTIFY_WARNING,
    NOTIFY_SAVED,
};

typedef struct Notif_Node {
    enum NotifyType type;
    char *msg;
    struct Notif_Node *next;
}Notif_Node;

typedef struct Notification {
    int count;
    time_t startTime;
    time_t currentTime;
    double diff;
    double delay;
    bool started;
    bool clear;
    Notif_Node *head;
}Notification;

void notify(Notification *notif, ViewPort *view);
void notifyInit(Notification *notif);
void notifySet(Notification *notif, enum NotifyType type, char *msg);
void notifyFree(Notification *notif);
void notifySetError(Notification *notif, int *errorCode);
void notifyUpdate(Notification *notif);
