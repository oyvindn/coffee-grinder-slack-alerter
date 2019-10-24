#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

void ICACHE_RAM_ATTR sound_interrupt();

volatile bool sound_detected_since_last_check = false;
uint8_t continuous_sound_count = 0;

WiFiClientSecure httpsClient;

void setup(){
    Serial.begin(9600);
    delay(10);
    connectToWifi();

    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000);

    pinMode(analog_input, INPUT);
    pinMode(digital_input, INPUT);
    attachInterrupt(digitalPinToInterrupt(digital_input), sound_interrupt, RISING);
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
    delay(sound_detection_period_in_milliseconds);
    
    if(sound_detected_since_last_check) {
        continuous_sound_count += 1;
    } else {
        if(continuous_sound_count > 6) {
            sound_trigger(continuous_sound_count * sound_detection_period_in_milliseconds);
        }
        continuous_sound_count = 0;
    }
    sound_detected_since_last_check = false;
}

void sound_interrupt() {
  sound_detected_since_last_check = true;
}

void sound_trigger(int milliseconds_continuous_sound) {
    Serial.print("Alerting slack after ");
    Serial.print((float) milliseconds_continuous_sound / (float) 1000, 1);
    Serial.println(" seconds continuous sound");
    alert_slack();
}

void alert_slack() {
    IPAddress remote_addr;
    WiFi.hostByName(host, remote_addr);
    Serial.print("HTTPS Connecting " + remote_addr.toString());
    int r=0; //retry counter
    while((!httpsClient.connect(host, 443)) && (r < https_client_max_retries)){
        delay(100);
        Serial.print(".");
        r++;
    }
    Serial.println();
    if(r==https_client_max_retries) {
        Serial.println("Connection failed");
    } else {
        Serial.println("Connected to slack");
    }
}
