#include <Arduino.h>
#include <Wire.h>

// Buffer to receive data
static char recv_buf[512];
static bool is_exist = false;

// Variables for data to send
const int inputPin = 1;
int data1 = 0;

// Function to send AT commands and check for expected response within a timeout
static int at_send_check_response(char *expected_ack, int timeout_ms, char *command_format, ...) {
    int character;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, command_format);
    Serial1.printf(command_format, args);
    Serial.printf(command_format, args);
    va_end(args);
    delay(200);
    startMillis = millis();

    if (expected_ack == NULL) {
        return 0;
    }

    do {
        while (Serial1.available() > 0) {
            character = Serial1.read();
            recv_buf[index++] = character;
            Serial.print((char)character);
            delay(2);
        }

        if (strstr(recv_buf, expected_ack) != NULL) {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    return 0;
}

// Function to send data over LoRa
static int node_send(uint32_t timeout) {
    int ret = 0;
    char data[32];
    char cmd[128];

    memset(data, 0, sizeof(data));
    sprintf(data, "%04X", data1);
    sprintf(cmd, "AT+TEST=TXLRPKT,\"5345454544%s\"\r\n", data);
    ret = at_send_check_response("TX DONE", timeout, cmd);

    if (ret == 1) {
        Serial.println("Sent successfully!");
    } else {
        Serial.println("Send failed!");
    }
    return ret;
}

void setup() {
    Serial.begin(115200); // Initialize serial communication
    pinMode(inputPin, INPUT);
    Serial1.begin(9600, SERIAL_8N1, 44, 43);
    Wire.begin();

    Serial.println("Initializing LoRa module...");

    // Basic AT command to check module presence
    if (at_send_check_response("+AT: OK", 100, "AT\r\n")) {
        is_exist = true;

        // Set LoRa module to TEST mode and configure RF settings
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);

    } else {
        is_exist = false;
        Serial.println("No E5 module found.");
    }
}

void loop() {
    if (is_exist) {
        // Update data dynamically
        data1 = digitalRead(inputPin);

        // Send data
        node_send(2000);
        delay(3000);
    } else {
        delay(1000); // Wait and retry if the module isn't available
    }
}
