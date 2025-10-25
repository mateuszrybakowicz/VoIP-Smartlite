#include "config.h" 
#include <string.h>
#include <windows.h>

static int parse_vk_code(const char* key) {
    if (_stricmp(key, "V") == 0) return 'V';
    if (_stricmp(key, "B") == 0) return 'B';
    if (_stricmp(key, "N") == 0) return 'N';
    if (_stricmp(key, "F1") == 0) return VK_F1;
    if (_stricmp(key, "F2") == 0) return VK_F2;
    if (_stricmp(key, "F3") == 0) return VK_F3;
    if (_stricmp(key, "F4") == 0) return VK_F4;
    if (_stricmp(key, "F5") == 0) return VK_F5;
    return 'V';
}

int load_config(const char* filename, Config* cfg) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("there is no such a file \"%s\"\n", filename);
        return 0;
    }
    strcpy(cfg->rpi_ip, "0.0.0.0");
    strcpy(cfg->pc_ip, "0.0.0.0");
    cfg->send_port = 5001;
    cfg->recv_port = 5000;
    cfg->ptt_key = 'V';
	cfg->modulated_male_ptt_key = 'B';
	cfg->modulated_female_ptt_key = 'N';
    strcpy(cfg->gst_debug, "*:3");

    char line[256];
    char name[64];
    char value[64];

    while (fgets(line, sizeof(line), f)) {
        // usuñ koñcowe \r\n
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        // Parsuj "name=value"
        if (sscanf(line, " %63[^=]=%63s", name, value) != 2) {
            continue;
        }

        if (_stricmp(name, "rpi_ip") == 0) {
            strncpy(cfg->rpi_ip, value, sizeof(cfg->rpi_ip) - 1);
            cfg->rpi_ip[sizeof(cfg->rpi_ip) - 1] = '\0';
        }
        else if (_stricmp(name, "pc_ip") == 0) {
            strncpy(cfg->pc_ip, value, sizeof(cfg->pc_ip) - 1);
            cfg->pc_ip[sizeof(cfg->pc_ip) - 1] = '\0';
        }
        else if (_stricmp(name, "send_port") == 0) {
            cfg->send_port = atoi(value);
        }
        else if (_stricmp(name, "recv_port") == 0) {
            cfg->recv_port = atoi(value);
        }
        else if (_stricmp(name, "normal_key") == 0) {
            cfg->ptt_key = parse_vk_code(value);
        }
        else if (_stricmp(name, "modulated_male_key") == 0) {
            cfg->modulated_male_ptt_key = parse_vk_code(value);
        }
        else if (_stricmp(name, "modulated_female_key") == 0) {
            cfg->modulated_female_ptt_key = parse_vk_code(value);
        }
        else if (_stricmp(name, "gst_debug") == 0) {
            strncpy(cfg->gst_debug, value, sizeof(cfg->gst_debug) - 1);
            cfg->gst_debug[sizeof(cfg->gst_debug) - 1] = '\0';
        }
    }

    fclose(f);
    return 1;
}