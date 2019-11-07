#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

void ICACHE_RAM_ATTR sound_detected_interrupt();

volatile bool sound_detected_since_last_check = false;
uint8_t subsequent_sound_frames_count = 0;

WiFiClientSecure httpsClient;

void setup(){
    Serial.begin(9600);
    delay(10);
    connectToWifi();

    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000);

    pinMode(analog_input, INPUT);
    pinMode(digital_input, INPUT);
    attachInterrupt(digitalPinToInterrupt(digital_input), sound_detected_interrupt, RISING);
}

void connectToWifi() {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {

    if(sound_detected_since_last_check) {
        subsequent_sound_frames_count += 1;
    } else {
        if(subsequent_sound_frames_count >= subsequent_sound_frames_trigger_count) {
            trigger_event(subsequent_sound_frames_count * sound_detection_frame_duration_ms);
        }
        subsequent_sound_frames_count = 0;
    }
    sound_detected_since_last_check = false;

    delay(sound_detection_frame_duration_ms);
}

void sound_detected_interrupt() {
    sound_detected_since_last_check = true;
}

void trigger_event(int milliseconds_continuous_sound) {
    Serial.print("Alerting slack after ");
    Serial.print((float) milliseconds_continuous_sound / (float) 1000, 1);
    Serial.println(" seconds of subsequent sound frames");

    delay(slack_alert_delay_ms);

    alert_slack("Kaffekverna ble kjørt. Forhåpentligvis nytraktet kaffe å få om kort tid!");
}

void alert_slack(char* alert_text) {
    Serial.print("Connecting to " + String(slack_host) + ":" + slack_port);

    int retries = 0;
    while((!httpsClient.connect(slack_host, slack_port)) && (retries < https_client_max_retries)){
        Serial.print(".");
        retries++;
        delay(100);
    }

    Serial.println();

    if(retries == https_client_max_retries) {
        Serial.println("Connection failed");
    } else {
        Serial.println("Connected to " + String(slack_host));
        Serial.println();

        String body = String("{\"text\": \"") + alert_text + "\"}";
        String payload = String("POST ") + slack_webhook_path + " HTTP/1.1\r\n" +
            "Host: " + slack_host + "\r\n" +
            "Connection: close" + "\r\n" +
            "Accept: */*" + "\r\n" +
            "Content-Type: application/json" + "\r\n" +
            "Content-Length: " + body.length() + "\r\n" +
            "\r\n" +
            body + "\r\n" +

        Serial.println("Payload:");
        Serial.println(payload);
        Serial.println();

        httpsClient.print(payload);

        Serial.println("POST request sent");

        Serial.println();

        Serial.println("Reponse headers:");
        String header_line = "";
        while (httpsClient.connected() && header_line != "\r") {
            header_line = httpsClient.readStringUntil('\n');
            Serial.println(header_line);
        }
        Serial.println();

        Serial.println("Reponse body:");
        while (httpsClient.available()) {
            String response_line = httpsClient.readStringUntil('\n');
            Serial.println(response_line);
        }

        Serial.println();

        Serial.println("Closing connection");
    }
}
