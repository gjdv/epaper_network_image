/* Includes ------------------------------------------------------------------*/
//#include <ESP8266WiFi.h>// ESP8266 and WiFi classes
#include <WiFi.h>
#include <ArduinoHttpClient.h>

/* SSID and password of your WiFi net ----------------------------------------*/
const char *ssid = "ssid"; // network ssid
const char *password = "password"; // password

/* Image to retrieve Settings ------------------------------------------------*/
char host_name[] = "ipaddressorhostname"; // no http:// just the bare hostname
int port = 80;
String path = "/example.br.raw"; // path on hostname to the .br.raw image to show


IPAddress myIP;        // IP address in your local wifi net
WiFiClient wifi;
HttpClient client = HttpClient(wifi, host_name, port);
  
void network_setup() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

//  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
//    Serial.println("Configuration failed.");
//  }

  // Applying SSID and password
  WiFi.begin(ssid, password);

  // Waiting the connection to a router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connection is complete
  Serial.println("");

  Serial.println("WiFi connected");

  // Show obtained IP address in local Wifi net
  Serial.println(myIP = WiFi.localIP());
}

char readchar() {
  char c = client.read();
//  Serial.printf(" %d", c);
  return c;
}

bool retrieve_image(UBYTE *blackimage, UBYTE *redimage) {
  UBYTE* currentimage = blackimage;
  if(client.connect(host_name, port)) {
    Serial.println("Connected to server");
    client.get(path);
    //client.println("GET " + path + " HTTP/1.0");
    //client.println(); // end HTTP request header

    int statusCode = client.responseStatusCode();
    // read headers
    while (client.headerAvailable()) {
        Serial.print("  " + client.readHeaderName());
        Serial.print(": ");
        Serial.println(client.readHeaderValue());
    }
    //String response = client.responseBody(); // does not work, so only use it for reading the header and then do the rest myself
    Serial.printf("StatusCode: %d\n", statusCode);
    if (statusCode != 200) {
      Serial.println("Invalid response from server");
      return false;
    }

    int numbytes = -1;
    int bytesread = 0;
    byte c = 0;
    Serial.print("Reading: ");
    if(wifi.connected()) { // Note: not the other way arround as asking wifi.connected() while in transfer causes problems (https://www.pschatzmann.ch/home/2020/12/05/wificlient-and-http/)
      while (wifi.available()) {
        if (numbytes == -1) {
          c = (byte) readchar();
          numbytes = (c << 8) + (byte) readchar();
          Serial.printf("Image size: %d\n", numbytes);
          c = readchar();
          if (c != 255) {
            Serial.println("Error: Wrong image format; expecting 2 bytes specifying the size L followed by 1 byte \xff, then the black channel followed by the red channel (both of size L)");
            return false;
          }
          if (numbytes != 48000) {
            Serial.println("Error: Wrong size image");
            return false;
          }
        }
        else {
          c = (byte) readchar();
          currentimage[bytesread] = c;
          bytesread++;
          
          if (bytesread == numbytes) { // after reading in the black channel switch to the red image
            currentimage = redimage;
            bytesread = 0;
          }
        }
      }
    }
    Serial.println("Transfer complete");
    client.stop();
    return true;
  } 
  else {
    Serial.println("Connection failed");
    return false;
  }
}
