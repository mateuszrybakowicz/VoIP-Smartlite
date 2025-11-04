#include "gstreamer_handler.h"
#include "keyboard_hook.h"
#include <string.h>
#include "config.h"


void toggle_voice_effect_advanced(AppData* app, int voice_mode) {
    if (app->pitch_element && app->equalizer_element) {

        switch (voice_mode) {
        case 1:
            g_object_set(app->pitch_element, "pitch", 1.0, NULL);
            g_object_set(app->equalizer_element,
                "band0", 0.0, "band1", 0.0, "band2", 0.0, NULL);
            break;

        case 3:
            g_object_set(app->pitch_element, "pitch", 1.2, NULL);
            g_object_set(app->equalizer_element,
                "band0", -6.0,
                "band1", 3.0,
                "band2", 2.0,
                NULL);
            break;

        case 2:
            g_object_set(app->pitch_element, "pitch", 0.7, NULL);
            g_object_set(app->equalizer_element,
                "band0", 0.0, "band1", 0.0, "band2", 0.0, NULL);
            break;
        }
    }
    else {
        g_print("Warning: Effect elements not available\n");
    }
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

    const gchar* voice_effect = "";


    launch_string_sender = g_strdup_printf(
        "wasapisrc low-latency=true provide-clock=false ! valve drop=TRUE name=valve0 ! "
        //"identity single-segment=true ! "  // Dodaj do pomiaru
        "queue max-size-time=100000 ! "
        "audioconvert dithering=0 ! audioresample quality=1 ! "
        "pitch name=voicepitch pitch=1.0 ! "
        "equalizer-3bands name=voiceeq ! "
        "audioconvert ! "
        "audio/x-raw,format=S16BE,channels=1,rate=16000 ! "
        "rtpL16pay pt=96 ! udpsink host=%s port=%d sync=false async=false buffer-size=65536",
        cfg->rpi_ip, cfg->send_port
    );

    launch_string_receiver = g_strdup_printf(
        "udpsrc port=%d buffer-size=131072 ! "
        "application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=1,payload=96 ! "
        "rtpjitterbuffer latency=100 do-lost=true ! "  
        "rtpL16depay ! "
        "queue max-size-time=0 ! "
        "audioconvert ! "
        "audioresample ! "
        "directsoundsink buffer-time=100000",
        cfg->recv_port
    );

    sender.pipeline = gst_parse_launch(launch_string_sender, NULL);
    sender.loop = loop;
    sender.name = "WINDOWS_SENDER";
	sender.equalizer_element = gst_bin_get_by_name(GST_BIN(sender.pipeline), "voiceeq");
	sender.pitch_element = gst_bin_get_by_name(GST_BIN(sender.pipeline), "voicepitch");

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
    g_print("full-duplex communication has been established\nPC->RPI at port 5001\nRPI->PC at port 5000\n");

    gst_debug_set_default_threshold(GST_LEVEL_LOG);
    g_setenv("GST_DEBUG", "latency:4,queue:4,rtpjitterbuffer:4", TRUE);


    g_main_loop_run(loop);

    // Sprz¹tanie
    g_print("stoping pipelines, clearing everything...\n");
    gst_element_set_state(receiver.pipeline, GST_STATE_NULL);
    gst_element_set_state(sender.pipeline, GST_STATE_NULL);
    gst_object_unref(receiver.pipeline);
    gst_object_unref(sender.pipeline);
    g_main_loop_unref(loop);
}