#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    GstElement* pipeline;
    GMainLoop* loop;
    const char *name;
} AppData;

typedef struct {
    char rpi_ip[64];
    char windows_ip[64];
    int send_port;
    int recv_port;
} Config;


int load_config(const char *filename, Config *cfg) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Brak pliku konfiguracyjnego %s, używam domyślnych wartości.\n", filename);
        strcpy(cfg->rpi_ip, "192.168.50.12");
        strcpy(cfg->windows_ip, "192.168.50.124");
        cfg->send_port = 5000;
        cfg->recv_port = 5001;
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "rpi_ip=")) sscanf(line, "rpi_ip=%63s", cfg->rpi_ip);
        else if (strstr(line, "windows_ip=")) sscanf(line, "windows_ip=%63s", cfg->windows_ip);
        else if (strstr(line, "send_port=")) sscanf(line, "send_port=%d", &cfg->send_port);
        else if (strstr(line, "recv_port=")) sscanf(line, "recv_port=%d", &cfg->recv_port);
    }

    fclose(f);
    return 1;
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


int main(int argc, char *argv[]) {
	Config cfg;
	load_config("config.ini", &cfg);	

	gst_init(&argc, &argv);

	AppData sender = { 0 };
	AppData receiver = { 0 };
	gchar* launch_string_sender = NULL;
	gchar* launch_string_receiver = NULL;
	GMainLoop* loop = g_main_loop_new(NULL, FALSE);

	launch_string_sender = g_strdup_printf(
		"alsasrc device=hw:2,0 ! audioconvert ! audioresample ! "
    		"audio/x-raw,format=S16BE,channels=1,rate=16000 ! "
    		"rtpL16pay ! udpsink host=%s port=%d",
    		cfg.windows_ip, cfg.send_port
	);

	launch_string_receiver = g_strdup_printf(
    		"udpsrc port=%d caps=\"application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=1\" ! "
    		"rtpjitterbuffer latency=20 ! rtpL16depay ! audioconvert ! audioresample ! alsasink sync=false",
    		cfg.recv_port
	);
	
    	sender.pipeline = gst_parse_launch(launch_string_sender, NULL);
	sender.loop = loop;
	sender.name = "RPI5_SENDER";

	receiver.pipeline = gst_parse_launch(launch_string_receiver, NULL);
	receiver.loop = loop;
	receiver.name = "RPI5_RECEIVER";

    	GstBus* bus1 = gst_element_get_bus(sender.pipeline);
	gst_bus_add_watch(bus1, bus_callback, &sender);
	gst_object_unref(bus1);

	GstBus* bus2 = gst_element_get_bus(receiver.pipeline);
	gst_bus_add_watch(bus2, bus_callback, &receiver);
	gst_object_unref(bus2);

	gst_element_set_state(receiver.pipeline, GST_STATE_PLAYING);
	gst_element_set_state(sender.pipeline, GST_STATE_PLAYING);
    	g_print("Wysyłam RTP na windows(port 5000)\nodbieram RTP port 5001");

    	g_main_loop_run(loop);

    	// Sprzątanie
	g_print("Zatrzymywanie pipeline...\n");
	gst_element_set_state(receiver.pipeline, GST_STATE_NULL);
	gst_element_set_state(sender.pipeline, GST_STATE_NULL);
	gst_object_unref(receiver.pipeline);
	gst_object_unref(sender.pipeline);
	g_main_loop_unref(loop);

    	return 0;
}
