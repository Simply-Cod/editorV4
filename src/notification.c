#include "notification.h"
#include "viewPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// ╔══════╦══════╗
// ║ col1 ║ col2 ║
// ╠══════╬══════╣
// ║ val1 ║ val2 ║
// ╚══════╩══════╝

void notifyInit(Notification *notif) {
    notif->count = 0;
    notif->delay = 3.0;
    notif->started = false;
    notif->clear = false;
    notif->head = NULL;
}

static void notifyError(ViewPort *view, char *err) {
    int boxWidth = 64;
    int startCol = view->width - boxWidth - 1;
    int row = 3;

    char buf[256];
    int n;

    // ┌ Top line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╔════════════════════════════════════════════════════╗", row, startCol);
    write(STDOUT_FILENO, buf, n);

    // │ Title line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ \x1b[31m%-50s\x1b[0m ║", row + 1, startCol, "Error!");
    write(STDOUT_FILENO, buf, n);

    // │ Message line (dynamic text)
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH║ %-50s ║", row + 2, startCol, err);
    write(STDOUT_FILENO, buf, n);

    // └ Bottom line
    n = snprintf(buf, sizeof(buf), "\x1b[%d;%dH╚════════════════════════════════════════════════════╝", row + 3, startCol);
    write(STDOUT_FILENO, buf, n);

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

void notifyUpdate(Notification *notif) {
    if (notif->started || notif->head == NULL) return;

    if (notif->head->type == NOTIFY_NONE) {
        Notif_Node *newHead = notif->head->next;

        if (notif->head->msg != NULL)
            free(notif->head->msg);
        free(notif->head);

        if (newHead != NULL) {
            notif->head = newHead;
        }
        else {
            notif->head = NULL;
        }
    }
    notif->started = false;
    notif->clear = true;
    notif->count--;
}

void notify(Notification *notif, ViewPort *view) {

    if (notif->head == NULL) return;
    Notif_Node *current = notif->head;
    if (!notif->started) {
        time(&notif->startTime);
        notif->started = true;
    }

    time(&notif->currentTime);
    notif->diff = difftime(notif->currentTime, notif->startTime);

    if (notif->diff > notif->delay) {
        current->type = NOTIFY_NONE;
        notif->started = false;
        return;
    }

    switch (current->type) {
        case NOTIFY_NONE:
            return;
        case NOTIFY_ERROR:
            notifyError(view, current->msg);
            break;
        case NOTIFY_HINT:
            notifyHint(view, current->msg);
            break;
        case NOTIFY_WARNING:
            notifyWarning(view, current->msg);
            break;
        case NOTIFY_SAVED:
            notifySaved(view, current->msg);
            break;
    }
}

void notifySet(Notification *notif, enum NotifyType type, char *msg) {
    Notif_Node *new = malloc(sizeof(Notif_Node));
    if (!new) return;

    new->msg = strdup(msg);
    if (!new->msg) {
        free(new);
        return;
    }
    new->type = type;

    if (notif->head == NULL) {
        notif->head = new;
    } else {
        Notif_Node *current = notif->head;

        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new;
    }
    notif->count++;
}

void notifySetError(Notification *notif, int *errorCode) {
    if (*errorCode < 900) return;
    Notif_Node *new = malloc(sizeof(Notif_Node));
    if (!new) return;
    new->type = NOTIFY_ERROR;
    new->next = NULL;

    switch (*errorCode) {
        // Buffer errors
        case ERR_BUFF_CREATE_HEAD_NOT_NULL:
            new->msg = strdup(ERR_BUFF_CREATE_HEAD_NOT_NULL_MSG);
            break;
        case ERR_BUFF_CREATE_HEAD_NULL:
            new->msg = strdup(ERR_BUFF_CREATE_HEAD_NULL_MSG);
            break;
        case ERR_BUFF_CREATE_HEAD_BUFFER_NULL:
            new->msg = strdup(ERR_BUFF_CREATE_HEAD_BUFFER_NULL_MSG);
            break;
        case ERR_BUFF_ADD_LINE_NEW_NULL:
            new->msg = strdup(ERR_BUFF_ADD_LINE_NEW_NULL_MSG);
            break;
        case ERR_BUFF_ADD_LINE_NEW_NULL_BUFFER:
            new->msg = strdup(ERR_BUFF_ADD_LINE_NEW_NULL_BUFFER_MSG);
            break;
        case ERR_BUFF_LOAD_FILE_FILE_NULL:
            new->msg = strdup(ERR_BUFF_LOAD_FILE_FILE_NULL_MSG);
            break;
        case ERR_BUFF_LOAD_FILE_CURRENT_BUFFER_NULL:
            new->msg = strdup(ERR_BUFF_LOAD_FILE_CURRENT_BUFFER_NULL_MSG);
            break;
        case ERR_BUFF_WRITE_FILE_NO_NAME:
            new->msg = strdup(ERR_BUFF_WRITE_FILE_NO_NAME_MSG);
            break;
        case ERR_BUFF_WRITE_FILE_NULL:
            new->msg = strdup(ERR_BUFF_WRITE_FILE_NULL_MSG);
            break;

            // info errors
        case ERR_INFO_HANDLE_ARGS_MEM_FAIL:
            new->msg = strdup(ERR_INFO_HANDLE_ARGS_MEM_FAIL_MSG);
            break;
        case ERR_INFO_HANDLE_ARGS_CHDIR_FAIL:
            new->msg = strdup(ERR_INFO_HANDLE_ARGS_CHDIR_FAIL_MSG);
            break;
        case ERR_INFO_HANDLE_ARGS_UNKNOWN:
            new->msg = strdup(ERR_INFO_HANDLE_ARGS_UNKNOWN_MSG);
            break;
        default:
            free(new);
            return;
    }
    if (!new->msg) {
        free(new);
        return;
    }
    if (notif->head == NULL) {
        notif->head = new;

    } else {

        Notif_Node *current = notif->head;

        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new;
    }
    notif->count++;
    *errorCode = 0;

}

void notifyFree(Notification *notif) {
    if (notif->head == NULL) return;

    Notif_Node *current = notif->head;
    Notif_Node *next;

    while (current != NULL) {
        next = current->next;

        if (current->msg != NULL) {
            free(current->msg);
        }
        free(current);

        current = next;
    }
    notif->head = NULL;
    notif->count = 0;
    notif->started = false;
}
