#include "keyboard_hook.h"
#include <Windows.h>

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_sender) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        GstElement* valve = gst_bin_get_by_name(GST_BIN(g_sender->pipeline), "valve0");

        if (wParam == WM_KEYDOWN && kb->vkCode == cfg.ptt_key && valve && !IsPTTPushed) {
            //printf("PTT: MIC ON\n");
            IsPTTPushed = true;
            g_object_set(valve, "drop", FALSE, NULL);
        }
        if (wParam == WM_KEYUP && kb->vkCode == cfg.ptt_key && valve) {
            //printf("PTT: MIC OFF\n");
            g_object_set(valve, "drop", TRUE, NULL);
            IsPTTPushed = false;
        }
        gst_object_unref(valve);
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

unsigned __stdcall KeyLoop(void* arg) {
    MSG msg;

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        printf("Could not put a Windows hook!\n");
        return 1;
    }
    printf("Press V to talk\n");

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("ESC pressed, exiting...\n");
            PostQuitMessage(0);  // zatrzyma GetMessage
            break;
        }
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}