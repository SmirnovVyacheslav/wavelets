#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "wavelet.h"
#include "open_gl.h"

#define ID_TRANSFORM_NAME 100
#define ID_OPEN	101
#define ID_SAVE 102
#define ID_DIRECT 103
#define ID_REVERSE 104
#define ID_CANCEL 105
#define ID_FILTER 106
#define ID_FULL 107
#define ID_WPACK 108
#define ID_INFO 109
#define ID_STEP 110
#define ID_INC_STEP 111
#define ID_DEC_STEP 112

using namespace std;

HDC view_hdc, plot_hdc;
RECT rect = { 0 };
HWND Main_Window, Transformation, Info, Step, Open, Save, Direct, Reverse, Cancel, Filter, Full, WPack, View, Plot, Inc_Step, Dec_Step;

wchar_t file[1024];
OPENFILENAMEW file_handler;

unsigned int time_req;

int main_cx = 800, main_cy = 600;
int view_x = 120, view_y = 60, view_cx = (main_cx - 160) / 2, view_cy = main_cy - 100;
int plot_x = view_x + view_cx + 20, plot_y = view_y, plot_cx = view_cx, plot_cy = view_cy;
int	pX = GetDeviceCaps(GetDC(NULL), HORZRES) / 2 - main_cx / 2,
	pY = GetDeviceCaps(GetDC(NULL), VERTRES) / 2 - main_cy / 2;

unique_ptr<Open_GL> gl;
unique_ptr<Wavelet> wave;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ChildProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK ChildProc_2(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam);