#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>  // do _beginthreadex
#include <stdbool.h>

typedef struct {
    GstElement* pipeline;
    GMainLoop* loop;
    const char* name;
} AppData;

AppData *g_sender = NULL;
HHOOK hHook;

boolean IsPTTPushed = false;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_sender) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        GstElement* valve = gst_bin_get_by_name(GST_BIN(g_sender->pipeline), "valve0");

        if (wParam == WM_KEYDOWN && kb->vkCode == 'V' && valve && !IsPTTPushed) {
            printf("PTT: Mikrofon ON\n");
            IsPTTPushed = true;
            g_object_set(valve, "drop", FALSE, NULL);
        }
        if (wParam == WM_KEYUP && kb->vkCode == 'V' && valve) {
            printf("PTT: Mikrofon OFF\n");
            g_object_set(valve, "drop", TRUE, NULL);
            IsPTTPushed = false;
        }
        gst_object_unref(valve);
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


static gboolean bus_callback(GstBus* bus, GstMessage* msg, gpointer user_data) {
    AppData* data = (AppData*)user_data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError* err = NULL;
        gchar* dbg_info = NULL;
        gst_message_parse_error(msg, &err, &dbg_info);
        g_printerr("[%s] ERROR: %s\n", data->name, err->message);
        if (dbg_info) {
            g_printerr("[%s] Debug info: %s\n",data->name, dbg_info);
        }
        g_error_free(err);
        g_free(dbg_info);

        g_main_loop_quit(data->loop);
        break;
    }

    case GST_MESSAGE_EOS:
        g_print("[%s] EOS (end of stream)\n", data->name);
        g_main_loop_quit(data->loop);
        break;

    case GST_MESSAGE_STATE_CHANGED: {
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->pipeline)) {
            GstState old_state, new_state, pending;
            gst_message_parse_state_changed(msg, &old_state, &new_state, &pending);
            g_print("[%s] Pipeline state changed from %s to %s\n",
                data->name,
                gst_element_state_get_name(old_state),
                gst_element_state_get_name(new_state));

            GstPad* pad = gst_element_get_static_pad(data->pipeline, "sink");
            if (pad) {
                GstCaps* caps = gst_pad_get_current_caps(pad);
                if (caps) {
                    gchar* caps_str = gst_caps_to_string(caps);
                    g_print("Negotiated caps: %s\n", caps_str);
                    g_free(caps_str);
                    gst_caps_unref(caps);
                }
                gst_object_unref(pad);
            }
        }
        break;
    }
    default:
        //g_print("[%s] Other message: %s\n", data->name, GST_MESSAGE_TYPE_NAME(msg));
        break;
    }
    return TRUE;
}

unsigned __stdcall KeyLoop(void* arg) {
    MSG msg;

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        printf("Nie uda³o siê za³o¿yæ hooka!\n");
        return 1;
    }
    printf("Trzymaj V, aby mówiæ (ESC = wyjœcie)\n");

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


int main(int argc, char* argv[]) {
    MSG msg;
    
 

    
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, KeyLoop, NULL, 0, NULL);


    g_setenv("GST_DEBUG", "*:3", TRUE);
    gst_init(&argc, &argv);

    AppData sender = { 0 };
    g_sender = &sender;
    AppData receiver = { 0 };
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);


    sender.pipeline = gst_parse_launch(
        "wasapisrc ! valve drop=TRUE name=valve0 !audioconvert !audioresample !"
        "audio/x-raw,format=S16BE,channels=1,rate=16000 ! "
        "rtpL16pay ! udpsink host=192.168.50.12 port=5001",
        NULL);
    sender.loop = loop;
    sender.name = "WINDOWS_SENDER";
    

    receiver.pipeline = gst_parse_launch(
        "udpsrc port=5000 caps=\"application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=1\" ! "
        "rtpL16depay ! audioconvert ! audioresample ! directsoundsink",
        NULL);
    receiver.loop = loop;
    receiver.name = "WINDOWS_RECEIVER";

    GstBus* bus1 = gst_element_get_bus(sender.pipeline);
    gst_bus_add_watch(bus1, bus_callback, &sender);
    gst_object_unref(bus1);

    GstBus* bus2 = gst_element_get_bus(receiver.pipeline);
    gst_bus_add_watch(bus2, bus_callback, &receiver);
    gst_object_unref(bus2);
    
    gst_element_set_state(receiver.pipeline, GST_STATE_PLAYING);
    gst_element_set_state(sender.pipeline, GST_STATE_PLAYING);
    g_print("zostala otwarta komunikacja dwukierunkowa, odbieram na portcie 5000, nadaje na 5001\n");


    g_main_loop_run(loop);

    // Sprz¹tanie
    g_print("Zatrzymywanie pipeline...\n");
    gst_element_set_state(receiver.pipeline, GST_STATE_NULL);
	gst_element_set_state(sender.pipeline, GST_STATE_NULL);
    gst_object_unref(receiver.pipeline);
	gst_object_unref(sender.pipeline);
    g_main_loop_unref(loop);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    return 0;
}
