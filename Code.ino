/**********************************************************************************
  Home Automation with Alexa and NodeMCU - UPDATE

  Espalexa is a library for ESP8266-based devices and ESP32 that emulates parts of
  the SSDP protocol and the Philips hue API, just enough so it can be discovered
  and controlled by Alexa-powered devices like the Amazon Echo or the Dot.

  7 "devices" to be discovered:

   4 Simple IoT devices
    ==> Red Light    (Light1)    ==> Relay 1 ==> NodeMCU D1 (GPIO5)  - Living Room
    ==> Blue Light   (Light2)    ==> Relay 3 ==> NodeMCU D5 (GPIO14) - Bed Room
    ==> Green Light  (Outlet1)   ==> Relay 2 ==> NodeMCU D2 (GPIO4)  - Living Room
    ==> Yellow Light (Outlet2)   ==> Relay 4 ==> NodeMCU D6 (GPIO12) - Bed Room

   3 Groups of devices
    ==> All Home Devices
    ==> Living Room (Light1 and Outlet1 or Red/Green leds on testing phase)
    ==> Bed Room (Light2 and Outlet2 or Blue/Yellow leds on testing phase)

   To discovery devices for the first time, use Alexa app on your smartphoneuse or
   a direct voice commnad:  "Alexa, discovery devices" (wait around 45 seconds)

   To activate a device or a group of devices (Device functions), you should use
   voice commands, like:
   "Alexa, turn on (off) Light1" or "..., turn on (off) Red Light" on testing phase
   "Alexa, turn on Living Room" or "..., turn off Living Room"
   "Alexa, turn on All Home Devices" or "..., turn off All Home Devices"

  *********************************************************************************/

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// Header Files For Dht22 and LCD
#include <DHT.h>;
//I2C LCD:
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//Constants DHT 22 Sensor
#define DHTPIN D3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
//int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

// display configuration and sensor pin define ends here


// define the GPIO connected with Relays

#define RelayPin1 14  //D5
#define RelayPin2 12  //D6
#define RelayPin3 13  //D7
#define RelayPin4 3   //RXD0

//direct callback functions
void device1Changed(uint8_t brightness);
void device2Changed(uint8_t brightness);
void device3Changed(uint8_t brightness);
void device4Changed(uint8_t brightness);

//special callback functions
/*void devFunc1Changed(uint8_t brightness);
  void devFunc2Changed(uint8_t brightness);*/
void devFunc3Changed(uint8_t brightness);

// device names
String Device_1_Name = "Fan 1";
String Device_2_Name = "Fan 2";
String Device_3_Name = "Light 1";
String Device_4_Name = "Light 2";

// device_function names
String Dev_Func_1_Name = "ALL Fans";
String Dev_Func_2_Name = "All Lights";
String Dev_Func_3_Name = "All Room Devices";

Espalexa espalexa;

// prototypes
boolean connectWifi();

// WiFi Credentials
const char* ssid = "";
const char* password = "";

boolean wifiConnected = false;

void setup()
{

  // display Initialization
  dht.begin();
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
  lcd.setCursor(0, 0);
  lcd.print("Project BY:");
  lcd.setCursor(0, 1);
  lcd.print("Er.Arpit Soni");
  delay(1500);
  // Display initialization ends here

  Serial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
  {
    // Define your devices here.
    espalexa.addDevice(Device_1_Name, device1Changed);
    espalexa.addDevice(Device_2_Name, device2Changed);
    espalexa.addDevice(Device_3_Name, device3Changed);
    espalexa.addDevice(Device_4_Name, device4Changed);

    // Define your devices functions here.
    espalexa.addDevice(Dev_Func_1_Name, devFunc1Changed);
    espalexa.addDevice(Dev_Func_2_Name, devFunc2Changed);
    espalexa.addDevice(Dev_Func_3_Name, devFunc3Changed);

    espalexa.begin();
  }
  else
  {
    while (1)
    {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void loop()
{
  //Read data and store it to variables hum and temp
  // Reading temperature or humidity takes about 250 milliseconds!

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  // for displaying temprature
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");

  // For Displaying Humidity
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print(" %");
  delay(500); //Delay 30 sec.

  espalexa.loop();
  delay(1);

  // Displaying Status on Lcd
/*  if (RelayPin1, LOW)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 1 is ON");
    delay(1000);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 1 is OFF");
    delay(1000);
  }
  */
}

//our callback functions
void device1Changed(uint8_t brightness) {

  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin1, LOW);
    Serial.println("Fan 1 : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 1 : ON");
    delay(2000);
  }
  else //if (brightness == 0)
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Fan 1 : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 1 : OFF");
    delay(2000);
  }
/*  else
  {
    int brigh_perc = (brightness / 255.) * 100;
    analogWrite(RelayPin1, brightness);
    Serial.print("Device1 Brightness: ");
    Serial.print(brigh_perc);
    Serial.println("%");

  }
  */
}

void device2Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin2, LOW);
    Serial.println("Fan 2 : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 2 : ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin2, HIGH);
    Serial.println("Fan 2 : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fan 2 : OFF");
    delay(2000);
  }
}

void device3Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin3, LOW);
    Serial.println("Light 1 : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light 1 : ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    Serial.println("Light 1 : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light 1 : OFF");
    delay(2000);
  }
}

void device4Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin4, LOW);
    Serial.println("Light 2 : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light 2 : ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin4, HIGH);
    Serial.println("Light 2 : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light 2 : OFF");
    delay(2000);
  }
}


void devFunc1Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin1, LOW);
    digitalWrite(RelayPin2, LOW);
    Serial.println("All Fans : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" All Fans : ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    digitalWrite(RelayPin2, HIGH);
    Serial.println("All : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Fans : OFF");
    delay(2000);
  }
}

void devFunc2Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin3, LOW);
    digitalWrite(RelayPin4, LOW);
    Serial.println(" All Lights : ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Lights : ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    digitalWrite(RelayPin4, HIGH);
    Serial.println("All Lights : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Lights : OFF");
    delay(2000);
  }
}

void devFunc3Changed(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin1, LOW);
    digitalWrite(RelayPin2, LOW);
    digitalWrite(RelayPin3, LOW);
    digitalWrite(RelayPin4, LOW);
    Serial.println("All Home Devices ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Dev. are ON");
    delay(2000);
  }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    digitalWrite(RelayPin2, HIGH);
    digitalWrite(RelayPin3, HIGH);
    digitalWrite(RelayPin4, HIGH);
    Serial.println("All Home Devices OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Dev. are OFF");
    delay(2000);
  }
}

// connect to wifi  returns true if successful or false if not
boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
