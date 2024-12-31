#include <Arduino.h>
#include "TFT_eSPI.h"
#include <SoftwareSerial.h>

SoftwareSerial e5(0, 1);

static char recv_buf[512];
static bool is_exist = false;
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft); // sprite

#define CONTROL_PIN 24

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    e5.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();

    if (p_ack == NULL)
    {
        return 0;
    }

    do
    {
        while (e5.available() > 0)
        {
            ch = e5.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }

        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }

    } while (millis() - startMillis < timeout_ms);
    return 0;
}

static int recv_prase(void)
{
    char ch;
    int index = 0;
    memset(recv_buf, 0, sizeof(recv_buf));
    while (e5.available() > 0)
    {
        ch = e5.read();
        recv_buf[index++] = ch;
        Serial.print((char)ch);
        delay(2);
    }

    if (index)
    {
        char *p_start = strstr(recv_buf, "+TEST: RX \"5345454544");
        if (p_start)
        {
            bool status = strstr(recv_buf, "01") != NULL; // Check for "01" or "00" to determine boolean value

            spr.createSprite(200, 50);
            spr.fillSprite(0x6670FF);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);

            if (status)
            {
                spr.setTextColor(TFT_RED, 0x6670FF);
                spr.drawString("Danger!", 40, 10);
                spr.pushSprite(70, 130);
                spr.deleteSprite();
            }
            else
            {
                spr.setTextColor(TFT_GREEN, 0x6670FF);
                spr.drawString("Safety", 40, 10);
                spr.pushSprite(80, 130);
                spr.deleteSprite();
            }

            Serial.println(status ? "Data: ON" : "Data: OFF");

            // Control pin based on data status
            digitalWrite(CONTROL_PIN, status ? HIGH : LOW);

            return 1;
        }
    }
    return 0;
}

static int node_recv(uint32_t timeout_ms)
{
    at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");
    int startMillis = millis();
    do
    {
        if (recv_prase())
        {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    return 0;
}

void setup(void)
{
    tft.begin();
    tft.setRotation(3);
    Serial.begin(115200);
    e5.begin(9600);
    Serial.print("Receiver\r\n");

    pinMode(CONTROL_PIN, OUTPUT);
    digitalWrite(CONTROL_PIN, LOW); // Initialize to LOW

    tft.fillScreen(0x6670FF); // Set background color to #ebffde
    tft.setFreeFont(&FreeSansBoldOblique18pt7b);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("GOAT GUARDS", 30, 25, 1); // Update text to "Goat Guards"
    tft.fillRect(20, 70, 280, 4,TFT_BLACK);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("Status:", 100, 90, 1); // Add "Status" below Goat Guards
    tft.setTextColor(TFT_GREEN);
    tft.drawString("---- Monitoring Active ----", 70, 200, 2);

    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);
    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
    }
}

void loop(void)
{
    if (is_exist)
    {
        node_recv(2000);
    }
}
