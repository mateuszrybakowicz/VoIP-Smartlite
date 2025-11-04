#pragma once

#include <stdio.h>

typedef struct {
    char rpi_ip[64];
    char pc_ip[64];
    int send_port;
    int recv_port;
    int ptt_key;
    int modulated_female_ptt_key;
    int modulated_male_ptt_key;
    char gst_debug[64];
} Config;

int load_config(const char* filename, Config* cfg);
int parse_vk_code(const char* key);