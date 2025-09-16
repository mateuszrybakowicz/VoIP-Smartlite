#pragma once
#include <windows.h>
#include <gst/gst.h>
#include "config.h"
#include <stdbool.h>

typedef struct {
    GstElement* pipeline;
    GMainLoop* loop;
    const char* name;
} AppData;

extern AppData* g_sender;
extern Config cfg;
extern boolean IsPTTPushed;
extern HHOOK hHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
unsigned __stdcall KeyLoop(void* arg);
