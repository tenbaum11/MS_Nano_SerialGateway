#include <SPI.h>
#include <MySensor.h>  
#include <Time.h> 

#define CID_HUM 0
#define CID_TEMP 1
#define CID_LIGHT 2
#define CID_RGB 3
#define LIGHT_SENSOR_ANALOG_PIN 0

#define RED   3  // Arduino PWM pin for Red
#define GREEN 6 // Arduino PWM pin for Green
#define BLUE  5 // Arduino PWM pin for Blue
const byte RGB_PINS[3] = { RED, GREEN, BLUE };

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

MySensor gw; 

MyMessage msgHUM(CID_HUM, V_HUM);
MyMessage msgTEMP(CID_TEMP, V_TEMP);
MyMessage msgLIGHT(CID_LIGHT, V_LIGHT_LEVEL);
MyMessage msgRGB(CID_RGB, V_VAR1);
//MyMessage on(CHILD_ID, V_SCENE_ON);
//MyMessage off(CHILD_ID, V_SCENE_OFF)
boolean metric = false; 
int lastLightLevel;
int lightLevel;
float humidity=0.0;
float temperature=0.0;

boolean timeReceived = false;
unsigned long lastUpdate=0, lastRequest=0;
//String RGBstr = "964f0";
String RGBstr = "7b4ef";
//123, 4, 239 7b4ef
// 220;3;1;0;24;7b4ef


int red, green, blue;

void setup()  
{ 
  
  for (int thisPin=0; thisPin < 3; thisPin++) { 
      pinMode(RGB_PINS[thisPin], OUTPUT); 
      Serial.print("PIN: "); Serial.println(RGB_PINS[thisPin]); 
      analogWrite(RGB_PINS[thisPin], 254); delay(1000);
      analogWrite(RGB_PINS[thisPin], 0);   delay(300);
  }
  
  
  gw.begin(incomingMessage, 220, false);
  gw.sendSketchInfo("UNO BB Light Sensor", "1.0");
  gw.present(CID_LIGHT, S_LIGHT_LEVEL);
  gw.present(CID_HUM, V_HUM);
  gw.present(CID_TEMP, S_TEMP);
  gw.present(CID_RGB, S_CUSTOM);
  //gw.present(CHILD_ID, S_SCENE_CONTROLLER);
  
  //HEXtoRGB(RGBstr);
  //analogWrite(BLUE, 254);
  //metric = gw.getConfig().isMetric;
}


void receiveTime(unsigned long time) {
  Serial.println("TIME");
  Serial.println(time);
  setTime((unsigned long)(time)); 
  timeReceived = true;
}

void loop()      
{     
  unsigned long now = millis();
  int sensorValue = 500;
  float batteryV  = sensorValue * 0.003866392;
  int batteryPcnt = sensorValue / 10;

  if ((!timeReceived && now-lastRequest > 5*1000)
    || (timeReceived && now-lastRequest > 60*1000*6)) { 
    Serial.println("requesting time");
    gw.requestTime(receiveTime);  
    lastRequest = now;
  }
  //int lightLevel=66;
  lightLevel++;
  humidity = humidity + 0.3;
  temperature = temperature + 0.2;
  //int lightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23; 

  // Print time every second
  if (timeReceived && now-lastUpdate > 3000) {
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(month());
    Serial.print(" ");
    Serial.print(year()); 
    Serial.println(); 
    lastUpdate = now;
  }



  if (lightLevel != lastLightLevel) {
    //Serial.println(lightLevel);
    gw.send(msgHUM.set(humidity, 1));
    gw.send(msgTEMP.set(temperature, 1));
    gw.send(msgLIGHT.set(lightLevel));
    gw.sendBatteryLevel(batteryPcnt);
    lastLightLevel = lightLevel;
  }

  gw.wait(3000);
  //gw.sleep(500000);

}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void HEXtoRGB(String RGBstr)
{
  unsigned long number = (unsigned long) strtol(&RGBstr[0], NULL, 16);
  Serial.println(number);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  int RGB_values[] = {r, g, b};
  Serial.print(F("r:")); Serial.println(r);
  Serial.print(F("g:")); Serial.println(g);
  Serial.print(F("b:")); Serial.println(b);
  
  for (int i = 0; i<3; i++) {
      analogWrite(RGB_PINS[i],RGB_values[i]);
  }
  
}


void incomingMessage(const MyMessage &message)
{

  int barrotemp = atoi(message.data);
  Serial.println("Rcvd from UNO");
  Serial.print("Sender: "); 
  Serial.println(message.sender);
  Serial.print("Last: "); 
  Serial.println(message.last);
  Serial.print("Sensor: "); 
  Serial.println(message.sensor);

  Serial.print("Type: "); 
  Serial.println(message.type);
  Serial.print("Data: "); 
  Serial.println(message.getFloat(), 1);

  if (message.type==V_VAR1) {
      String hexstring = message.getString();
      HEXtoRGB(hexstring);
  }

}




