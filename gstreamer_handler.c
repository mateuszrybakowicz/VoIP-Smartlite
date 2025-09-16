#include "gstreamer_handler.h"
#include "keyboard_hook.h"
#include <string.h>
#include "config.h"

static gboolean bus_callback(GstBus* bus, GstMessage* msg, gpointer user_data) {
    AppData* data = (AppData*)user_data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError* err = NULL;
        gchar* dbg_info = NULL;
        gst_message_parse_error(msg, &err, &dbg_info);
        g_printerr("[%s] ERROR: %s\n", data->name, err->message);
        if (dbg_info) {
            g_printerr("[%s] Debug info: %s\n", data->name, dbg_info);
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

void gstreamer_create_and_run(Config* cfg) {
    g_setenv("GST_DEBUG", cfg->gst_debug, TRUE);
    gst_init(NULL, NULL);

    AppData sender = { 0 };
    g_sender = &sender;
    AppData receiver = { 0 };
    gchar* launch_string_sender = NULL;
    gchar* launch_string_receiver = NULL;
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    launch_string_sender = g_strdup_printf(
        "wasapisrc ! valve drop=TRUE name=valve0 !audioconvert !audioresample !"
        "audio/x-raw,format=S16BE,channels=1,rate=16000 ! "
        "rtpL16pay ! udpsink host=%s port=%d",
        cfg->rpi_ip, cfg->send_port
    );
    launch_string_receiver = g_strdup_printf(
        "udpsrc port=%d caps=\"application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=1\" ! "
        "rtpL16depay ! audioconvert ! audioresample ! directsoundsink",
        cfg->recv_port
    );

    sender.pipeline = gst_parse_launch(launch_string_sender, NULL);
    sender.loop = loop;
    sender.name = "WINDOWS_SENDER";

    receiver.pipeline = gst_parse_launch(launch_string_receiver, NULL);
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
    g_print("full-duplex communication has been established\nPC->RPI at port 5001\nRPI->PC at port 5000");

    g_main_loop_run(loop);

    // Sprz¹tanie
    g_print("stoping pipelines, clearing everything...\n");
    gst_element_set_state(receiver.pipeline, GST_STATE_NULL);
    gst_element_set_state(sender.pipeline, GST_STATE_NULL);
    gst_object_unref(receiver.pipeline);
    gst_object_unref(sender.pipeline);
    g_main_loop_unref(loop);
}