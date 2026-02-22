#pragma once

#include "viewPort.h"
#include <time.h>
#include <stdbool.h>

enum NotifyType {
    NOTIFY_NONE,
    NOTIFY_CLEAR,
    NOTIFY_ERROR,
    NOTIFY_HINT,
    NOTIFY_WARNING,
    NOTIFY_SAVED,
};
typedef struct Notification {
    enum NotifyType type;
    char *msg;
    time_t startTime;
    time_t currentTime;
    double diff;
    double delay;
    bool started;
}Notification;

int notifyError(ViewPort *view, char *err);
void notify(Notification *notif, ViewPort *view);
void notifySet(Notification *notif, enum NotifyType type, char *msg);
