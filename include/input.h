#pragma once
// ALT + numb = Escnumb eks: alt-1 = 27-1
// pageUp = esc[5~  pageDown = esc[6~
// home = esc[1~    end = esc[4~
// ins = esc[2~     del = esc[3~

// default keybinds
#define CTRL_A 1
#define CTRL_B 2
#define CTRL_C 3
#define CTRL_D 4
#define CTRL_E 5
#define CTRL_F 6
#define CTRL_G 7
#define CTRL_H 8
#define CTRL_I 9
#define CTRL_J 10
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_M 13
#define CTRL_N 14
#define CTRL_O 15
#define CTRL_P 16
#define CTRL_Q 17
#define CTRL_R 18
#define CTRL_S 19
#define CTRL_T 20
#define CTRL_U 21
#define CTRL_V 22
#define CTRL_W 23
#define CTRL_X 24
#define CTRL_Y 25
#define CTRL_Z 26


#define TAB 9
#define ESC 27
#define ENTER 13
#define BACKSPACE 127

// Custom key binds
#define UP      1000
#define DOWN    1001
#define RIGHT   1002
#define LEFT    1003

// Alt + <key> = esc+key
#define ALT_J 1500
#define ALT_K 1501

// Command signals
#define CMD_QUIT 2000
#define CMD_WRITE 2001


unsigned int readInput();
