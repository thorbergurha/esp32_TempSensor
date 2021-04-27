#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>


// Data wire is plugged TO GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time tempC was updated

// constants won't change:
const long interval = 600000;           // interval at which to take tempC (milliseconds)

//Set const for relay
const int relay = 26;

const char* ssid     = "SSID";
const char* password = "WIFIPASSWORD";

const char* serverName = "DATABASE POST LINK";

String apiKeyValue = "DATABASEAPIKEY";


// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Number of temperature devices found
int numberOfDevices;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress;

void setup()
{
    // start serial port
    Serial.begin(115200);

    // Start up the library
    sensors.begin();

    pinMode(relay, OUTPUT);


    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());




  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();




    // Grab a count of devices on the wire
    numberOfDevices = sensors.getDeviceCount();

    // locate devices on the bus
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(numberOfDevices, DEC);
    Serial.println(" devices.");

    // Loop through each device, print out address
    for (int i = 0; i < numberOfDevices; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(tempDeviceAddress, i))
        {
            Serial.print("Found device ");
            Serial.print(i, DEC);
            Serial.print(" with address: ");
            printAddress(tempDeviceAddress);
            Serial.println();
        }
        else
        {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.print(" but could not detect address. Check power and cabling");
        }
    }
}

void loop()
{

    ArduinoOTA.handle();

    sensors.requestTemperatures(); // Send the command to get temperatures

    //Get sensors temps
    float temperatureC = sensors.getTempCByIndex(0);

    // Loop through each device, print out temperature data
    for (int i = 0; i < numberOfDevices; i++)
    {
        // Search the wire for address
        if (sensors.getAddress(tempDeviceAddress, i))
        {
            // Output the device ID
            Serial.print("Temperature for device: ");
            Serial.println(i, DEC);
            // Print the data
            float tempC = sensors.getTempC(tempDeviceAddress);
            Serial.print("Temp C: ");
            Serial.print(tempC);
            Serial.println();
        }
    }

    if (temperatureC >= 20)
    {
        digitalWrite(relay, LOW);
        Serial.println("Current Flowing");
    }
    else if (temperatureC <= 25)
    {
        digitalWrite(relay, HIGH);
        Serial.println("Current not Flowing");
    }
    else
    {
        digitalWrite(relay, HIGH);
        Serial.println("Current not Flowing");
    }


    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        
        previousMillis = currentMillis;
        
        if(WiFi.status()== WL_CONNECTED){
            HTTPClient http;
            
            // Your Domain name with URL path or IP address with path
            http.begin(serverName);
            
            // Specify content-type header
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            
            // Prepare your HTTP POST request data
            String httpRequestData = "api_key=" + apiKeyValue
                                + "&sensor=" + String("Hiti") //Hér kemur hvaða hitamælir þetta kemur frá, úr gerjunaríláti eða skáp?
                                + "&value1=" + String(temperatureC)
                                + "&value2=" + String("Test") //Hér kemur value um relay
                                + "";
            Serial.print("httpRequestData: ");
            Serial.println(httpRequestData);
            
            // You can comment the httpRequestData variable above
            // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
            //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

            // Send HTTP POST request
            int httpResponseCode = http.POST(httpRequestData);
            
            // If you need an HTTP request with a content type: text/plain
            //http.addHeader("Content-Type", "text/plain");
            //int httpResponseCode = http.POST("Hello, World!");
            
            // If you need an HTTP request with a content type: application/json, use the following:
            //http.addHeader("Content-Type", "application/json");
            //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
                
            if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            }
            else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
            }
            // Free resources
            http.end();
        }
        else {
            Serial.println("WiFi Disconnected");
        }
    }

    Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (deviceAddress[i] < 16)
            Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}
