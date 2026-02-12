#pragma once

#include "buffer.h"

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
    enum RenderType render;
}ViewPort;


void viewInit(ViewPort *view);
int viewGetTerminalSize(ViewPort *view);

void viewUpdate(ViewPort *view);

void tmpViewDraw(ViewPort *view, Buffer *buff);
