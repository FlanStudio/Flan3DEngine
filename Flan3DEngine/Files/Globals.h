#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)
#define MIN(a, b) (a < b) ? (a) : (b)
#define MAX(a, b) (a > b) ? (a) : (b)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float SCREEN_SIZE;
extern bool WIN_FULLSCREEN;
extern bool WIN_RESIZABLE;
extern bool WIN_BORDERLESS;
extern bool WIN_FULLSCREEN_DESKTOP;
extern bool VSYNC;
extern float BRIGHTNESS;
#endif