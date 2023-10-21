/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"

#include "network.h"

int REFRESH_INTERVAL = 10*60; // refresh every 10 minutes

/* Entry point ----------------------------------------------------------------*/
void setup()
{
  Serial.println("Dev module init");
  DEV_Module_Init();
  delay(10);
  
  Serial.println("Network setup");
  network_setup();

  Serial.println("e-Paper Init...");
  EPD_7IN5B_V2_Init();

  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Not woken by timer, clear screen...");
    EPD_7IN5B_V2_Clear();
    DEV_Delay_ms(500);
  }

  esp_sleep_enable_timer_wakeup(REFRESH_INTERVAL * 1000000);

  UBYTE *BlackImage, *RYImage;
  UWORD Imagesize = ((EPD_7IN5B_V2_WIDTH % 8 == 0) ? (EPD_7IN5B_V2_WIDTH / 8 ) : (EPD_7IN5B_V2_WIDTH / 8 + 1)) * EPD_7IN5B_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Serial.println("ERROR: Failed to apply for black memory...");
    while(1);
  }
  if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Serial.println("ERROR: Failed to apply for red memory...");
    while(1);
  }
  
  Serial.println("Retrieving image");
  bool res = retrieve_image(BlackImage, RYImage);
  if (res) {
    Serial.println("EPD_Display");
    EPD_7IN5B_V2_Display(BlackImage, RYImage);
    DEV_Delay_ms(2000);
  }

  Serial.println("Goto Sleep...");
  EPD_7IN5B_V2_Sleep();
  
  free(BlackImage);
  free(RYImage);
  BlackImage = NULL;
  RYImage = NULL;
  
  esp_deep_sleep_start();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{ // unused when using deep sleep
}
