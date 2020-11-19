#include <Adafruit_GFX.h>    // Core graphics library for TFT Dispaly
#include <Adafruit_ST7735.h> // Hardware-specific library for TFT Display
#include <SPI.h> //The Serial Peripheral Interface is a synchronous serial communication interface specification used for short-distance communication, primarily in embedded systems. 
#include <MQ2.h>//MQ-2 Gas sensor library
#include <ESP8266WiFi.h>//ESP8266 Wi-Fi library for connecting the ESP8266 to the internet
#include <FirebaseArduino.h>//Libary for sending the data gathered by ESP8266 to Firebase Realtime Database
#include <DHT.h>//DHT-11 Temperature and Humidity sensor

/*Pin declaration of the 1.8" TFT display*/
#define TFT_CS     D1 //TFT Display CS Pin connected to D1 of the NodeMCU
#define TFT_RST    D3 //TFT Display Reset Pin connected to D3 of the NodeMCU
#define TFT_DC     D5 //TFT Display A0 Pin connected to D5 of the NodeMCU
#define TFT_SCLK D0  //TFT Display SCK Pin connected to D0 of the NodeMCU 
#define TFT_MOSI D2  //TFT Display SDS Pin connected to D2 of the NodeMCU
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

/*Declaration of the WIFI-SSID and Password*/
#define WIFI_SSID "Xperia XA1 Ultra_2634" //WIFI SSID hidden due to security reason
#define WIFI_PASSWORD "fb9085061cda" //WIFI Password hidden due to security reason
/*Declaration of the Firebase Host and authentication code*/
#define FIREBASE_HOST "testfirebase-a025c.firebaseio.com" //Firebase Host hidden due to security reason
#define FIREBASE_AUTH "2RBVeAtAtfBKjOvUmouO7BXPTdKX0ByK92xYWN3S" // Firebase Authentication Code hidden due to security reason


//Declaration of the sensors pin
const int flamePin = D7;//Flame senor's D0 pin connected to D7
int Flame = HIGH;//Flame sensor was set to HIGH mode
DHT dht;//The DHT term for the DHT 11 sensor will be used as "dht"
int Analog_Input = A0; // Analog Pin was declared as A0
int lpg, co, smoke;//Declaration of the integer for LPG, Smoke, CarbonMonoxide value
MQ2 mq2(Analog_Input);//The MQ2 term for the MQ-2 gas sensor will be used as "mq2" with A0 pin connected with the development board
const int buzzer = D8;// Declaration of the +V buzzer pin connected to pin no D8 of the Arduino


void setup(void) {
              Serial.begin(9600);
              Serial.print("Hello");
              // Use this initializer if you're using a 1.8" TFT
              tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
              Serial.println("Initialized");
              //Used to calculate the intilisation time
              uint16_t time = millis();
              tft.fillScreen(ST7735_WHITE);//Sets the backgrund colur as white
              time = millis() - time;
              Serial.println(time, DEC);//Print the intilisation time of the TFT Display
              delay(500); 
              tft.setRotation(1);//Rotate screen only 1 time
              tft.setCursor(50, 50);//Sets the cursor at x=50 and y=50
              tft.setTextColor(ST7735_BLACK);//Sets the text colour as black
              tft.setTextSize(2.5);//Sets the text size to 2.5
              tft.println("HELLO");//Prints HELLO
              tft.setTextSize(2.5);//Sets the text size to 2.5
              tft.print("   ALEKHAYA");//Prints ALEKHAYA with spaces
              delay(1000);//delays by 1000ms
              tft.fillScreen(ST7735_WHITE);//Sets the backgrund colur as white
              tft.setCursor(40, 50);//Sets the cursor at x=40 and y=50
              tft.setTextColor(ST7735_BLACK);//Sets the text colour as black
              tft.setTextSize(2.5);//Sets the text size to 2.5
              tft.print("LOADING");//Prints out LOADING
              delay(500);//delays by 500ms
             
              pinMode(buzzer, OUTPUT); //Specifying buzzer pin as output
              pinMode(flamePin, INPUT);//Specifying the flame digital pin as input
              tone(buzzer, 6000);//The buzzer produces a beeping noise at 6000Hz
              delay(1000);//Delays by 1000ms
              noTone(buzzer); //stops the beeping of the buzzer
            
              dht.setup(D6);//DHT11 initialization
              delay(2000);//delays by 2000ms
              Serial.println('\n');
              mq2.begin();//MQ-2 Gas Sensor initialization
              wifiConnect(); //User defined function
              Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);//Firebase initialization
              delay(10);
}



void loop()
{     
                 /* DHT11 sensor*/
                  delay(dht.getMinimumSamplingPeriod()); //Delay of amount equal to sampling period 
                  float humidity = dht.getHumidity();// Get humidity value measured by the DHT 11 sensor in percentage
                  float temperature = dht.getTemperature();//Get temperature value measured by the DHT 11 sensor in celsius 
                  float temperature_F=temperature*9/5+32;//Convert the temperature value measured by the DHT 11 sensor in celsius to Fahrenheit 
                  Firebase.setFloat("DHT/Json/temp",temperature); //Create and update the Celsius value on Firebase
                  Firebase.setFloat("DHT/Json/temp_F",temperature_F);//Create and update the Fahrenheit value on Firebase
                  Firebase.setFloat("DHT/Json/hum",humidity);//Create and update the Humidity value on Firebase
                         
                  /*MQ-2 Gas sensor*/
                  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
                  lpg = mq2.readLPG();
                  co = mq2.readCO();
                  smoke = mq2.readSmoke();
                  Firebase.setFloat("SmokeSensor/Json/smoke",smoke);
                  Firebase.setFloat("SmokeSensor/Json/co",co);
                  Firebase.setFloat("SmokeSensor/Json/lpg",lpg);

                /*TFT Display print*/
                  tft.setTextWrap(false);// If there are any extra characters, then the cursor will go to the next line and print the remaning characters
                  tft.setCursor(15, 5);
                  tft.setTextSize(2);
                  tft.fillScreen(ST7735_GREEN);
                  tft.setTextColor(ST7735_RED);
                  
                  tft.println("TEMPERATURE");
                  tft.println(".................................");
                  tft.setTextColor(ST7735_BLACK);
                  tft.println("  TEMP in C:");
                  tft.print("   ");
                  tft.println(temperature);
                  tft.println(".................................");
                  
                  tft.println("  TEMP in F:");
                  tft.print("   ");
                  tft.println( temperature_F);
                  delay(2000);
                
                  
                  tft.setCursor(20, 5);
                  tft.setTextColor(ST7735_RED);
                  tft.setTextSize(2);
                  tft.fillScreen(ST7735_YELLOW );
                  tft.println(" HUMIDITY");
                  tft.println(".................................");
                  tft.setTextColor(ST7735_BLACK);
                  tft.println("  HUMIDITY:");
                  tft.print("   ");
                  tft.println(humidity);
                  tft.println(".................................");
                  delay(2000);

                  tft.setCursor(15, 5);
                  tft.setTextColor(ST7735_BLUE);
                  tft.setTextSize(2);
                  tft.fillScreen(ST7735_CYAN );
                  tft.println("  GAS VALUE");
                  tft.println(".................................");
                  tft.setTextColor(ST7735_BLACK);
                  tft.println("     CO:");
                  tft.print("     ");
                  tft.println(co);
                  tft.println(".................................");
                  tft.println("    LPG:");
                  tft.print("     ");
                  tft.println(lpg);
                  tft.println(".................................");
                  delay(2000);
                  
                  tft.setCursor(15, 5);
                  tft.setTextColor(ST7735_WHITE);
                  tft.setTextSize(2);
                  tft.fillScreen(ST7735_MAGENTA);
                  tft.println(" GAS VALUE");
                  tft.println(".................................");
                  tft.setTextColor(ST7735_BLACK);
                  tft.println("    SMOKE");
                  tft.print("      ");
                  tft.println(smoke);
                  tft.println(".................................");
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
                        tone(buzzer, 2000);
                        delay(1000);                               
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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //initialize the Wi-Fi connection between Wi-Fi source and ESP8266
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); //Print the number of times Wi-Fi connection was tried to established
    Serial.print(' ');
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
