#include "notification.h"
#include "viewPort.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// ╔══════╦══════╗
// ║ col1 ║ col2 ║
// ╠══════╬══════╣
// ║ val1 ║ val2 ║
// ╚══════╩══════╝

int notifyError(ViewPort *view, char *err) {
    int boxWidth = 42;
    int startCol = view->width - boxWidth - 1;
    int row = 3;

    char buf[256];
    int n;

    // ┌ Top line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╔════════════════════════════════════╗", row, startCol);
    write(STDOUT_FILENO, buf, n);

    // │ Title line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ \x1b[31m%-34s\x1b[0m ║", row + 1, startCol, "Error!");
    write(STDOUT_FILENO, buf, n);

    // │ Message line (dynamic text)
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ %-34s ║", row + 2, startCol, err);
    write(STDOUT_FILENO, buf, n);

    // └ Bottom line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╚════════════════════════════════════╝", row + 3, startCol);
    write(STDOUT_FILENO, buf, n);

    return 0;
}

static void notifySaved(ViewPort *view, char *msg) {
    int boxWidth = 42;
    int startCol = view->width - boxWidth - 1;
    int row = 3;

    char buf[256];
    int n;

    // ┌ Top line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╔════════════════════════════════════╗", row, startCol);
    write(STDOUT_FILENO, buf, n);

    // │ Title line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ \x1b[32m%-34s\x1b[0m ║", row + 1, startCol, "Saved!");
    write(STDOUT_FILENO, buf, n);

    // │ Message line (dynamic text)
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ %-34s ║", row + 2, startCol, msg);
    write(STDOUT_FILENO, buf, n);

    // └ Bottom line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╚════════════════════════════════════╝", row + 3, startCol);
    write(STDOUT_FILENO, buf, n);

}
static void notifyWarning(ViewPort *view, char *msg) {
    int boxWidth = 42;
    int startCol = view->width - boxWidth - 1;
    int row = 3;

    char buf[256];
    int n;

    // ┌ Top line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╔════════════════════════════════════╗", row, startCol);
    write(STDOUT_FILENO, buf, n);

    // │ Title line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ \x1b[33m%-34s\x1b[0m ║", row + 1, startCol, "Warning!");
    write(STDOUT_FILENO, buf, n);

    // │ Message line (dynamic text)
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ %-34s ║", row + 2, startCol, msg);
    write(STDOUT_FILENO, buf, n);

    // └ Bottom line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╚════════════════════════════════════╝", row + 3, startCol);
    write(STDOUT_FILENO, buf, n);

}
static void notifyHint(ViewPort *view, char *msg) {
    int boxWidth = 42;
    int startCol = view->width - boxWidth - 1;
    int row = 3;

    char buf[256];
    int n;

    // ┌ Top line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╔════════════════════════════════════╗", row, startCol);
    write(STDOUT_FILENO, buf, n);

    // │ Title line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ \x1b[34m%-34s\x1b[0m ║", row + 1, startCol, "Hint!");
    write(STDOUT_FILENO, buf, n);

    // │ Message line (dynamic text)
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ %-34s ║", row + 2, startCol, msg);
    write(STDOUT_FILENO, buf, n);

    // └ Bottom line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╚════════════════════════════════════╝", row + 3, startCol);
    write(STDOUT_FILENO, buf, n);

}

void notify(Notification *notif, ViewPort *view) {

    if (notif->type == NOTIFY_NONE) return;
    if (!notif->started) {
        time(&notif->startTime);
        notif->started = true;
    }

    time(&notif->currentTime);
    notif->diff = difftime(notif->currentTime, notif->startTime);

    if (notif->diff > 3.0) {
        notif->type = NOTIFY_NONE;
        notif->started = false;
        notif->type = NOTIFY_CLEAR;
        return;
    }

    switch (notif->type) {
        case NOTIFY_CLEAR:
        case NOTIFY_NONE:
            return;
        case NOTIFY_ERROR:
            notifyError(view, notif->msg);
            break;
        case NOTIFY_HINT:
            notifyHint(view, notif->msg);
            break;
        case NOTIFY_WARNING:
            notifyWarning(view, notif->msg);
            break;
        case NOTIFY_SAVED:
            notifySaved(view, notif->msg);
            break;
    }
}

void notifySet(Notification *notif, enum NotifyType type, char *msg) {
    notif->type = type;
    strncpy(notif->msg, msg, 32);
}
