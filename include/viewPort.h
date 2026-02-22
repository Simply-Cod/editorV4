#pragma once

#include "buffer.h"
#include "bufferInfo.h"

enum RenderType {
    RENDER_WELCOME,
    RENDER_FULL,
    RENDER_LINE,
    RENDER_FROM_CURSOR,
};

typedef struct ViewPort {
    int height;
    int width;
    int topLine;
    int oldTopLine;
    int curX;
    int curY;
    int oldLineNumb;
    int currentLineNumb;
    enum RenderType render;
}ViewPort;


void viewInit(ViewPort *view);
int viewGetTerminalSize(ViewPort *view);

void viewUpdate(ViewPort *view, BufferInfo *info);

void tmpViewDraw(ViewPort *view, Buffer *buff);
void viewSetCursorStyle(BufferInfo *info);
void viewCorrectCursor(ViewPort *view, Buffer *buff);


int viewDraw(ViewPort *view, Buffer *buff, BufferInfo *info);
void viewDrawWelcome(ViewPort *view);
void viewDrawFull(ViewPort *view, Buffer *buff, BufferInfo *info);
void viewDrawLine(ViewPort *view, Buffer *buff, BufferInfo *info);

void viewDrawStatusLine(ViewPort *view, Buffer *buff, BufferInfo *info);

void viewPlaceCursorOnCurrent(ViewPort *view);

void viewPrintLine(Line *line);
int viewMoveCurOnY(Line *oldLine, Line *newLine);


void viewDrawCommandStatus(Line **line, enum CommandFunction cmdFunc, ViewPort *view);
void viewDrawCommand(Line **line, enum CommandFunction cmdFunc, ViewPort *view);
void viewCommandSetCursor(Line *line, ViewPort *view, int curX);
