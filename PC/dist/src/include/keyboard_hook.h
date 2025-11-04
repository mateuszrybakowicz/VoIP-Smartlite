#pragma once
#include <windows.h>
#include <gst/gst.h>
#include "config.h"
#include <stdbool.h>
#include "gstreamer_handler.h"

extern AppData* g_sender;
extern Config cfg;
extern boolean IsPTTPushed;
extern HHOOK hHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
unsigned __stdcall KeyLoop(void* arg);
