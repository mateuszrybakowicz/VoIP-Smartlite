#include "config.h" 
#include <string.h>
#include <windows.h>

static int parse_vk_code(const char* key) {
    if (_stricmp(key, "V") == 0) return 'V';
    if (_stricmp(key, "F2") == 0) return VK_F2;
    if (_stricmp(key, "F3") == 0) return VK_F3;
    if (_stricmp(key, "F4") == 0) return VK_F4;
    if (_stricmp(key, "F5") == 0) return VK_F5;
    return 'V';
}

int load_config(const char* filename, Config* cfg) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("there is no such a file \"%s\", using deafult values.\n", filename);
        strcpy(cfg->rpi_ip, "0.0.0.0");
        strcpy(cfg->pc_ip, "0.0.0.0");
        cfg->send_port = 5001;
        cfg->recv_port = 5000;
        cfg->ptt_key = 'V';
        strcpy(cfg->gst_debug, "*:3");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "rpi_ip=")) sscanf(line, "rpi_ip=%63s", cfg->rpi_ip);
        else if (strstr(line, "pc_ip=")) sscanf(line, "pc_ip=%63s", cfg->pc_ip);
        else if (strstr(line, "send_port=")) sscanf(line, "send_port=%d", &cfg->send_port);
        else if (strstr(line, "recv_port=")) sscanf(line, "recv_port=%d", &cfg->recv_port);
        else if (strstr(line, "key=")) {
            char key[16];
            sscanf(line, "key=%15s", key);
            cfg->ptt_key = parse_vk_code(key);
        }
        else if (strstr(line, "gst_debug=")) sscanf(line, "gst_debug=%63s", cfg->gst_debug);
    }

    fclose(f);
    return 1;
}