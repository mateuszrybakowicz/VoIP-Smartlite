#pragma once
#include <gst/gst.h>
#include "config.h"

typedef struct {
    GstElement* pipeline;
    GMainLoop* loop;
    const char* name;
    GstElement* pitch_element;
    GstElement* equalizer_element;
} AppData;

void toggle_voice_effect_advanced(AppData* app, int voice_mode);
void gstreamer_create_and_run(Config* cfg);