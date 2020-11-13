#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <MQ2.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#define TFT_CS     D1
#define TFT_RST    D3  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     D5

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK D0   // set these to be whatever pins you like!
#define TFT_MOSI D2   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

#define WIFI_SSID "Xperia XA1 Ultra_2634" 
#define WIFI_PASSWORD "fb9085061cda"
#define FIREBASE_HOST "testfirebase-a025c.firebaseio.com"
#define FIREBASE_AUTH "2RBVeAtAtfBKjOvUmouO7BXPTdKX0ByK92xYWN3S"



const int flamePin = D7;
int Flame = HIGH;
DHT dht;

int Analog_Input = A0;
int lpg, co, smoke;
MQ2 mq2(Analog_Input);
const int buzzer = D8;


void setup(void) {
 Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(ST7735_WHITE);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  //Rotate screen
  tft.setRotation(1);
  tft.setCursor(50, 50);
tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(2);
  tft.print("LOADING");
  delay(500);
  // tft print function!
//  tftPrintTest();
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(flamePin, INPUT);


dht.setup(D6);

  delay(2000);
  Serial.println('\n');
  mq2.begin();
  wifiConnect();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  

  delay(10);


}



void loop()
{  
delay(dht.getMinimumSamplingPeriod()); /* Delay of amount equal to sampling period */
  float humidity = dht.getHumidity();/* Get humidity value */
  float temperature = dht.getTemperature();/* Get temperature value */
  float temperature_F=temperature*9/5+32;
  Firebase.setFloat("DHT/Json/temp",temperature);
  Firebase.setFloat("DHT/Json/temp_F",temperature_F);
  Firebase.setFloat("DHT/Json/hum",humidity);
  Serial.print(dht.getStatusString());/* Print status of communication */
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);

  
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke();
  Firebase.setFloat("SmokeSensor/Json/smoke",smoke);
  Firebase.setFloat("SmokeSensor/Json/co",co);
  Firebase.setFloat("SmokeSensor/Json/lpg",lpg);

    tft.setTextWrap(false);
  //tft.fillScreen(ST7735_BLACK);
  tft.setCursor(15, 5);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(2);
  tft.fillScreen(ST7735_GREEN);
  tft.setTextColor(ST7735_BLACK);
  tft.println("TEMPERATURE");
  tft.println(".................................");
  tft.println("  TEMP in C:");
  tft.print("   ");
  tft.println(temperature);
  tft.println(".................................");
  
  tft.println("  TEMP in F:");
  tft.print("   ");
  tft.println( temperature_F);
  delay(2000);
  

  Flame = digitalRead(flamePin);
  if (Flame== LOW)
  {
   Firebase.setFloat("FIRE",1);
   tone(buzzer, 1000);
  delay(1000); 
  tone(buzzer, 2000);
  delay(1000); 
  tone(buzzer, 3000);
  delay(1000); 
  tone(buzzer, 4000);
  delay(1000); 
  tone(buzzer, 5000);
  delay(1000); 
  tone(buzzer, 6000);// Send 1KHz sound signal...
  delay(1000);   

  tone(buzzer, 6000);
  delay(1000); 
  tone(buzzer, 5000);
  delay(1000); 
  tone(buzzer, 3000);
  delay(1000); 
  tone(buzzer, 2000);
  delay(1000); 
  tone(buzzer, 1000);
  delay(1000);// Send 1KHz sound signal...
 
   
  }
  else 
  {
    Firebase.setFloat("FIRE",0);
    noTone(buzzer);
  }




if(WiFi.status() != WL_CONNECTED)
{
  wifiConnect();
}
delay(10);

}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
//void tftPrintTest() {
//  tft.setRotation(1);
//  String txt = "Hi Iam Alek";
//  tft.setTextWrap(false);
//  //tft.fillScreen(ST7735_BLACK);
//  tft.setCursor(0, 0);
//  tft.setTextColor(ST7735_RED);
//  tft.setTextSize(2);
//  a=Firebase.getFloat("SmokeSensor/Json/smoke",smoke);
//  tft.println(a);
//  delay(500);
//  tft.setTextColor(ST7735_WHITE);
//  tft.println(txt);
//  delay(500);
//  tft.setTextColor(ST7735_YELLOW);
//  tft.println(txt);
//  delay(500);
//  tft.setTextColor(ST7735_BLUE);
//  tft.println(txt);
//  delay(500);
//  tft.setTextColor(ST7735_GREEN);
//  tft.println(txt);
//  delay(500);
//  tft.setTextColor(ST7735_WHITE);
//  tft.println("");
//  tft.println("   DOT COM");
//  delay(2000);  
//}
