#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

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
static int SCREEN_WIDTH = 1280;
static int SCREEN_HEIGHT = 1024;
static float SCREEN_SIZE = 1;
static bool WIN_FULLSCREEN = false;
static bool WIN_RESIZABLE = false;
static bool WIN_BORDERLESS = false;
static bool WIN_FULLSCREEN_DESKTOP = false;
static bool VSYNC = false;
static float BRIGHTNESS = 1.0f;
