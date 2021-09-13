#pragma once
#include "La_Interface.h"

UCHAR* back_buffer = (UCHAR*)&GRAPHIC::graphicOut;

int back_lpitch = 0;

INPUT_::KEYBOARD keyboard;

FRAMECounter fpsSet;
GAMEBox gameBox;

#define DInput_Init_Keyboard()  keyboard.create()

#define DInput_Read_Keyboard()  keyboard.read()

#define keyboard_state keyboard

#define Draw_Rectangle(x1,y1, x2, y2, color) GRAPHIC::DrawRectangle(x1, y1, x2, y2, color)