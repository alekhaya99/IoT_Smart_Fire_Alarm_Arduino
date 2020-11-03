#include <MQ2.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>

#define WIFI_SSID "Xperia XA1 Ultra_2634" 
#define WIFI_PASSWORD "fb9085061cda"
#define FIREBASE_HOST "testfirebase-a025c.firebaseio.com"
#define FIREBASE_AUTH "2RBVeAtAtfBKjOvUmouO7BXPTdKX0ByK92xYWN3S"
#define LED 8


const int flamePin = D4;
int Flame = HIGH;
DHT dht;

int Analog_Input = A0;
int lpg, co, smoke;

MQ2 mq2(Analog_Input);

void setup()
{
pinMode(flamePin, INPUT);
Serial.begin(115200);


dht.setup(D6);

  delay(2000);
  Serial.println('\n');
  mq2.begin();
  wifiConnect();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString("LED1","h");
  Firebase.setInt("DHT/Json/ht",1);
 

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

  
 Flame = digitalRead(flamePin);
  if (Flame== LOW)
  {
   Firebase.setFloat("FIRE",1);
  }
  else
  {
    Firebase.setFloat("FIRE",0);
  }
delay(10);

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
  Serial.print(WIFI_SSID); Serial.println(" ...");

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
