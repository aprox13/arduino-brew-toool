
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "presenter/oled_screen.h"
#include "presenter/screen.h"
#include "controller/brew_controller.h"
#include "model/arduino.h"
#include "model/arduino_proxy.h"
#include "sensor/flow_sensor.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

#define FLOW_SENSOR_PIN 2
#define RESET_BNT_PIN ((uint8_t)5)
#define FLOW_SPEED_CALC_PERIOD 1000

#define DELAY_PERIOD_MS 100


brew_controller *controller;
oled_screen *screen;

long prev_ticks = 0;

void setup()
{
   Serial.begin(9600);

   Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

   if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
   {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
         ;
   }

   pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
   pinMode(RESET_BNT_PIN, INPUT_PULLUP);

   flow_sensor::attach(digitalPinToInterrupt(FLOW_SENSOR_PIN));

   display.clearDisplay();

   arduino *arduino = new arduino_proxy();
   screen = new oled_screen(display);
   button_controller btn(RESET_BNT_PIN, *arduino, LOW);
   flow_controller fc(*arduino, FLOW_SPEED_CALC_PERIOD);
}

void loop()
{
   long now = millis();
   
   if (now - prev_ticks > DELAY_PERIOD_MS) {
      prev_ticks = now;

      screen->draw_flow_screen(flow_sensor::liters(), 0, flow_sensor::get_ticks());
   }

}
