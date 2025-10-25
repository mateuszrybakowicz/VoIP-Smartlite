#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "keyboard_hook.h"
#include "gstreamer_handler.h"

AppData *g_sender = NULL;
HHOOK hHook;
boolean IsPTTPushed = false;
Config cfg;

int main(int argc, char* argv[]) {
    load_config("config.ini", &cfg);
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, KeyLoop, NULL, 0, NULL);

    gstreamer_create_and_run(&cfg);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    return 0;
}
