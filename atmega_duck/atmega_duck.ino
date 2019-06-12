/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/WiFiDuck
 */

// ===== Settings ===== //
#define I2C_ADDR 0x31 // I2C address this (slave) device will listen to

#define DEBUG         // Enable serial debugging output

#define BUFFER_SIZE 512

#define RESPONSE_OK 0x00
#define RESPONSE_PROCESSING 0x01

// ===== Libraries ===== //firefox0
#include <Wire.h>        // I2C0
#include "DuckyParser.h" // Ducky Script language Interpreter

// ===== Types ===== //
typedef struct buffer_t {
    char   data[BUFFER_SIZE];
    size_t len;
} buffer_t;

// ===== Global Variables ===== //
DuckyParser ducky;

bool processing = false;

buffer_t mainBuffer;

// ===== Global Functions ===== //
// I2C Request
void requestEvent() {
    if (processing) {
        Wire.write(RESPONSE_PROCESSING);
    } else {
        processing = mainBuffer.len > 0;
        Wire.write(processing ? RESPONSE_PROCESSING : RESPONSE_OK);
    }
}

// I2C Receive
void receiveEvent(int len) {
    if (mainBuffer.len + (unsigned int)len <= BUFFER_SIZE) {
        Wire.readBytes(&mainBuffer.data[mainBuffer.len], len);
        mainBuffer.len += len;
    } else {
        // Serial.println("Buffer is full!");
    }
}

// ===== SETUP ====== //
void setup() {
    /*
       Serial.begin(115200);
       while (!Serial);
       Serial.println("Started!");
     */

    Wire.begin(I2C_ADDR); // Start I2C

    // Set I2C events
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);

    Keyboard.begin();
}

// ===== LOOOP ===== //
void loop() {
    if (processing) {
        ducky.parse(mainBuffer.data, mainBuffer.len);
        mainBuffer.len = 0;
        processing     = false;
    }
}