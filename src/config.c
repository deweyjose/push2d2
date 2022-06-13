//
// Created by Dewey Jose on 5/12/22.
//

#include <string.h>
#include <confuse.h>
#include <config.h>
#include <malloc.h>

// remapping for readability and reduce bugs/mistypes
#define cf(name) CFG_FLOAT(name, 0, CFGF_NONE)
#define ci(name) CFG_INT(name, 0, CFGF_NONE)
#define cs(name) CFG_STR(name, 0, CFGF_NONE)
#define cend() CFG_END()
#define dcgi(prop) config->display_config.prop = cfg_getint(display_cfg, #prop)
#define rcgi(prop) rconf->prop = cfg_getint(rotary, #prop)
#define log(format, ...) printf("CONFIG: " format "\n" , ##__VA_ARGS__)

int config_load(config_ptr config, char *filename) {
    log("loading config file %s", filename);

    cfg_opt_t serial_opts[] = {
            cs("device"),
            ci("baud_rate"),
            cend()
    };

    cfg_opt_t coordinates_opts[] = {
            cf("latitude"),
            cf("longitude"),
            cend()
    };

    cfg_opt_t display_opts[] = {
            ci("rows"),
            ci("columns"),
            ci("bit_mode"),
            ci("register_select_pin"),
            ci("enable_pin"),
            ci("d0_pin"),
            ci("d1_pin"),
            ci("d2_pin"),
            ci("d3_pin"),
            ci("d4_pin"),
            ci("d5_pin"),
            ci("d6_pin"),
            ci("d7_pin"),
            cend()
    };

    cfg_opt_t rotary_opts[] = {
            ci("phase_a_pin"),
            ci("phase_b_pin"),
            ci("cpr"),
            ci("gear_ratio"),
            cend()
    };

    cfg_opt_t secs_opts[] = {
            CFG_SEC("serial_config", serial_opts, CFGF_NONE),
            CFG_SEC("coordinates_config", coordinates_opts, CFGF_NONE),
            CFG_SEC("display", display_opts, CFGF_NONE),
            CFG_SEC("rotary", rotary_opts, CFGF_TITLE | CFGF_MULTI)
    };

    // load the options file
    cfg_t *cfg;
    cfg = cfg_init(secs_opts, CFGF_NONE);
    if (cfg_parse(cfg, filename) == CFG_PARSE_ERROR) {
        log("failed to parse %s", filename);
        return 0;
    }

    cfg_t *serial_cfg = cfg_getsec(cfg, "serial_config");

    char *tmpDevice = cfg_getstr(serial_cfg, "device");
    unsigned int str_len = strlen(tmpDevice) + 1;
    config->serial.device = (char *) malloc(str_len);
    strcpy(config->serial.device, tmpDevice);

    config->serial.baud_rate = (short)cfg_getint(serial_cfg, "baud_rate");

    // map the options into the configuration struct

    cfg_t *coordinates_cfg = cfg_getsec(cfg, "coordinates_config");
    config->coordinates.longitude = cfg_getfloat(coordinates_cfg, "longitude");
    config->coordinates.latitude = cfg_getfloat(coordinates_cfg, "latitude");

    cfg_t *display_cfg = cfg_getsec(cfg, "display");
    dcgi(rows);
    dcgi(columns);
    dcgi(bit_mode);
    dcgi(register_select_pin);
    dcgi(enable_pin);
    dcgi(d0_pin);
    dcgi(d1_pin);
    dcgi(d2_pin);
    dcgi(d3_pin);
    dcgi(d4_pin);
    dcgi(d5_pin);
    dcgi(d6_pin);
    dcgi(d7_pin);

    unsigned int rotarySections = cfg_size(cfg, "rotary");
    if (rotarySections != 2) {
        log("found %d rotary sections 2 required", rotarySections);
        return 0;
    }

    for (int i = 0; i < rotarySections; ++i) {
        cfg_t *rotary = cfg_getnsec(cfg, "rotary", i);

        struct rotary_config *rconf = (strcmp(cfg_title(rotary), "azimuth") == 0) ?
                                      &config->azimuth_config :
                                      &config->altitude_config;

        rcgi(phase_a_pin);
        rcgi(phase_b_pin);
        rcgi(cpr);
        rcgi(gear_ratio);
    }

    cfg_free(cfg);
    return 1;
}

void config_dump(config_ptr config) {
    log("CONFIG dump:");
    log("coordinates_config:");
    log("  latitude: %LF", config->coordinates.latitude);
    log("  longitude: %LF", config->coordinates.longitude);
    log("display:");
    log("  rows: %d", config->display_config.rows);
    log("  columns: %d", config->display_config.columns);
    log("  bit_mode: %d", config->display_config.bit_mode);
    log("  register_select_pin: %d", config->display_config.register_select_pin);
    log("  enable_pin: %d", config->display_config.enable_pin);
    log("  d0_pin: %d", config->display_config.d0_pin);
    log("  d1_pin: %d", config->display_config.d1_pin);
    log("  d2_pin: %d", config->display_config.d2_pin);
    log("  d3_pin: %d", config->display_config.d3_pin);
    log("  d4_pin: %d", config->display_config.d4_pin);
    log("  d5_pin: %d", config->display_config.d5_pin);
    log("  d6_pin: %d", config->display_config.d6_pin);
    log("  d7_pin: %d", config->display_config.d7_pin);
    log("rotary azimuth:");
    log("  phase_a_pin: %d", config->azimuth_config.phase_a_pin);
    log("  phase_b_pin: %d", config->azimuth_config.phase_b_pin);
    log("  cpr: %d", config->azimuth_config.cpr);
    log("  gear_ratio: %d", config->azimuth_config.gear_ratio);
    log("rotary altitude:");
    log("  phase_a_pin: %d", config->altitude_config.phase_a_pin);
    log("  phase_b_pin: %d", config->altitude_config.phase_b_pin);
    log("  cpr: %d", config->altitude_config.cpr);
    log("  gear_ratio: %d", config->altitude_config.gear_ratio);
}
