#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

typedef struct {
    GstElement* pipeline;
    GMainLoop* loop;
    const char *name;
} AppData;


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
	gst_init(&argc, &argv);

	AppData sender = { 0 };
	AppData receiver = { 0 };
	GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    	sender.pipeline = gst_parse_launch(
        	"alsasrc device=hw:2,0 ! audioconvert ! audioresample ! "
        	"audio/x-raw,format=S16BE,channels=1,rate=16000 ! "
        	"rtpL16pay ! udpsink host=192.168.50.124 port=5000",
        	NULL);
	sender.loop = loop;
	sender.name = "RPI5_SENDER";

	receiver.pipeline = gst_parse_launch(
    		"udpsrc port=5001 caps=\"application/x-rtp,media=audio,clock-rate=16000,encoding-name=L16,channels=1\" ! "
    		" rtpjitterbuffer latency=20 ! rtpL16depay ! audioconvert ! audioresample ! alsasink sync=false",
    		NULL);
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
