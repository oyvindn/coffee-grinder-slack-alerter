#ifndef config_h
#define config_h
#include <Arduino.h>

const uint8_t analog_input = A0;
const uint8_t digital_input = D6;

const char* ssid     = "WiFi SSID";
const char* password = "WiFi password";

const char* slack_host = "hooks.slack.com";
const uint16_t slack_port = 443;
const char* slack_webhook_path = "/services/T00000000/B00000000/XXXXXXXXXXXXXXXXXXXXXXXX";

const char fingerprint[] PROGMEM = "C1 0D 53 49 D2 3E E5 2B A2 61 D5 9E 6F 99 0D 3D FD 8B B2 B3";

const uint8_t sound_detection_frame_duration_ms = 200;
const uint8_t subsequent_sound_frames_trigger_count = 12;
const uint8_t slack_alert_delay_ms = 6 * 60 * 1000;

const uint8_t https_client_max_retries = 30;

#endif config_h