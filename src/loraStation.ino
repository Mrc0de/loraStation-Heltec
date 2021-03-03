// First LoraStation Device
// platform: espressif32
// board: heltec_wifi_lora_32_V2
// framework: arduino

// First physical build: "Ziggy"
#include "heltec.h"
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
// AdaFruit_GFX = PlatformIO Library #ID 13
#include <Adafruit_ST7735.h>
#include <Minc.h> // logo, cuz im cooler than u
// Bluetooth Low Energy BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
// We dont do anything with any network devices automatically
// We just initialize them, we might not even do that until needed later on, for now we bring them up to make sure they are connected
// Initial StatusBootScreen shows the devices come up, later so will a 'device status' menu option
#include <Wire.h>

// Input
// m5stack cardkb (cheap too, dont buy them, mine)
#define CARDKB_ADDR 0x5F
#define KEYPAD_CHAR_LIMIT 256   // kb buffer, increase as needed

// Lora Networking
// Lora Band for the heltec_wifi_lora_32_V2 ( 433E6 , 868E6 or 915E6 are available)
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6

// Waveshare 1.8" LCD 128x160 ST7735S Controller (green tab)
// PIN 21 is MOSI (in code) (Display calls it DIN)
// PIN 22 is CLK on display
#define TFT_CS 2
#define TFT_DC 23
#define TFT_RST 17
// Backlight goes to any power, run it around to seperate/dim/etc if you care about that
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, 21, 22, TFT_RST);

//int counter = 0;
//int initInterval = 5000;
//int initLastSent = millis();
char c = 'Z';
// New
char keypad[KEYPAD_CHAR_LIMIT];
byte WifiMAC[6];

void setup() {
  tft.initR(INITR_GREENTAB);  //waveshare TFT 1.8inch 128x160
  delay(500);
  tft.setCursor(0,0);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLUE);
  tft.fillScreen(ST7735_WHITE);
  tft.fillScreen(ST7735_RED); // when it does it, you will see that, these colors in the library....are wrong (rigged for correct output)
  // Heltec Display, Lora and Serial
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  // Secondary I2C, just cardkb for now
  Wire1.begin(32,33); // 21 SDA (white), 22 SCL (yellow)

  // Initial Startup Printout
  tft.fillScreen(0xA514);
  tft.setTextColor(ST7735_BLACK);
  tft.println("|LoraStation v0.1");
  tft.println("|Band: " + String(BAND/1000).substring(0,String(BAND/1000).indexOf(".")));
  //only keyboard, lora, and the screens are on
  //Bring up WIFI, but dont connect or start ap
  tft.println("|Init WiFi Adapter");
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  WiFi.macAddress(WifiMAC);
  tft.println("|MAC: " + String(WifiMAC[0],HEX) + String(":") + String(WifiMAC[1],HEX) + String(":") + String(WifiMAC[2],HEX) + String(":") +
            String(WifiMAC[3],HEX) + String(":") + String(WifiMAC[4],HEX) + String(":") + String(WifiMAC[5],HEX));
  tft.println("|WiFi Adapter IDLE");
  // Bring up Bluetooth
  tft.println("|Init BLE Adapter");
  BLEDevice::init("BLEd");
  tft.println("|BLE Adapter IDLE");
  tft.println("|*-+Starting+-*|");



//  tft.println(" BLE: Ready");
//  tft.println(" GPS: SIM808");
//  tft.println(" GSM: SIM808");
//  tft.println(" Storage: MicroSD");

  // The smaller heltec screen is just for debug output, is usually covered by the casing and will probably just get in the way
  delay(1000);
  tft.fillScreen(ST7735_BLACK);
  tft.drawXBitmap(0, 0, Minc, Minc_width, Minc_height, 0xFFFF);
  delay(2000);
  Heltec.display->drawString(0, 16, "[Debug Screen] ");
  Heltec.display->display();
}

void loop() {
  // on keypress
  Wire1.requestFrom(CARDKB_ADDR,1);
  while (Wire1.available()) {
    c = Wire1.read();
    if (c!=0) {
          tft.println("Lastkey: " + String(c));
    }
  }
}

void offerInit433(){
    LoRa.beginPacket();
    // /*
    // * LoRa.setTxPower(txPower,RFOUT_pin);
    // * txPower -- 0 ~ 20
    // * RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
    // *   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
    // *   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
    // */
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print("INIT433"); //handshake offer, plain test, bring encryption later
//    LoRa.print(counter);
    LoRa.endPacket();
//    counter++;
}

// OLD CRAP ONLY FOR MY MEMORY OF SYNTAX ETC, garbaged later

//  // Broadcast Init433 once per
//  if (millis() - initLastSent > initInterval) {
//      Heltec.display->clear();
//      Heltec.display->drawString(0, 9, " [Offer Init 433] " + String(counter));
//      Heltec.display->drawString(0, 18, " [keyin: " + String(c) + " ]");
//      Heltec.display->display();
//      offerInit433();
//      initLastSent = millis();
//  }

