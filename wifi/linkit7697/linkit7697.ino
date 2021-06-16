/*
  Web client

  This sketch connects to a website (http://download.labs.mediatek.com)
  using LinkIt 7697

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:
  * LinkIt 7697

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe
  modified Jan 2017
  by MediaTek Labs
*/


#include <LWiFi.h>

char ssid[] = "Palace of Joe Tsai";      //  your network SSID (name)
char pass[] = "0918273645";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(117,185,24,248);
//char server[] = "download.labs.mediatek.com";   // http://download.labs.mediatek.com/linkit_7697_ascii.txt
IPAddress server(140,112,30,36);
int port = 6887;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

char *FLAG = "HW5{Jo3_Tsa1-7he_M4st3r_0F_Tra1niNg}";

void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
//
//    // attempt to connect to Wifi network:
//    while (status != WL_CONNECTED) {
//        Serial.print("Attempting to connect to SSID: ");
//        Serial.println(ssid);
//        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
//        status = WiFi.begin(ssid, pass);
//    }
//    Serial.println("Connected to wifi");
//    printWifiStatus();
}

void loop() {

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
    }
    Serial.println("Connected to wifi");
    printWifiStatus();
  
    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    if (client.connect(server, port)) {
        Serial.println("connected to server (GET)");
        // Make a HTTP request:
        client.println(FLAG);
        Serial.println("flag sent");
        delay(1000);
    }
  
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
        char c = client.read();
        Serial.write(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();

    }

    // disconnect
    status = WiFi.disconnect();
    Serial.println(status);
    Serial.println("disconnected from WIFI");

//    do nothing forevermore:
//    while (true);
    delay(1000);
}


void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
