
#include "WiFiEsp.h"
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// Trying these libraries for http client
// #include <ArduinoJson.h>
// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// Trying those libraries for http client

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#define ESP_BAUDRATE  19200
#else
#define ESP_BAUDRATE  115200
#endif

char server[] = "arduino.tips";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

// Initialize the Ethernet client object
WiFiEspClient client;

// // Weather station channel details
// unsigned long weatherStationChannelNumber = SECRET_CH_ID_WEATHER_STATION;
// unsigned int temperatureFieldNumber = 4;

// // Counting channel details
// unsigned long counterChannelNumber = SECRET_CH_ID_COUNTER;
// const char * myCounterReadAPIKey = SECRET_READ_APIKEY_COUNTER;
// unsigned int counterFieldNumber = 1; 

void setup() {
 //Initialize serial and wait for port to open
  Serial.begin(115200);  
  while(!Serial){
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // initialize serial for ESP module  
  setEspBaudRate(ESP_BAUDRATE);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  Serial.print("Searching for ESP8266..."); 
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  Serial.println("found it!");

  // int statusCode = 0;
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    }
  
    printWifiStatus();
    Serial.println("\nConnected");
  }

  // ThingSpeak.begin(client);  // Initialize ThingSpeak
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.tips");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {

  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }


  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
  // // Read in field 4 of the public channel recording the temperature
  // float temperatureInF = ThingSpeak.readFloatField(weatherStationChannelNumber, temperatureFieldNumber);  

  // // Check the status of the read operation to see if it was successful
  // statusCode = ThingSpeak.getLastReadStatus();
  // if(statusCode == 200){
  //   Serial.println("Temperature at MathWorks HQ: " + String(temperatureInF) + " deg F");
  // }
  // else{
  //   Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  // }
  
  // delay(15000); // No need to read the temperature too often.

  // // Read in field 1 of the private channel which is a counter  
  // long count = ThingSpeak.readLongField(counterChannelNumber, counterFieldNumber, myCounterReadAPIKey);  

  //  // Check the status of the read operation to see if it was successful
  // statusCode = ThingSpeak.getLastReadStatus();
  // if(statusCode == 200){
  //   Serial.println("Counter: " + String(count));
  // }
  // else{
  //   Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  // }
  
  // delay(15000); // No need to read the counter too often.
  
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    
    // send the HTTP PUT request
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.tips");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

// This function attempts to set the ESP8266 baudrate. Boards with additional hardware serial ports
// can use 115200, otherwise software serial is limited to 19200.
void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");

  for(int i = 0; i < 6; i++){
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial1.print(",8,1,0,0\r\n");
    delay(100);  
  }
    
  Serial1.begin(baudrate);
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
