#ifndef config_h
#define config_h
#include <Arduino.h>

const uint8_t analog_input = A0;
const uint8_t digital_input = D6;

const char* ssid     = "WiFi SSID";
const char* password = "WiFi password";

const char* host = "hooks.slack.com";

const char fingerprint[] PROGMEM = "C1 0D 53 49 D2 3E E5 2B A2 61 D5 9E 6F 99 0D 3D FD 8B B2 B3";

const uint8_t sound_detection_period_in_milliseconds = 200;
const uint8_t https_client_max_retries = 30;

#endif config_h