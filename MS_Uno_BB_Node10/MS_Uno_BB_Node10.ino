#include <SPI.h>
#include <Wire.h>
#include <MySensor.h>  
#include <Time.h> 

//#define DEBUG 1
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.print (F(x))
 #define DEBUG_PRINTDEC(x)    Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
#endif
//DEBUG_PRINT ("I think I'm here");",`


#include <SFE_BMP180.h>
SFE_BMP180 pressure;
#define ALTITUDEmeter 100.0  //meters

#define CID_HUM     0
#define CID_TEMP    1
#define CID_LIGHT   2
#define CID_RGB     3
#define CID_BTEMP   4
#define CID_BBARO   5
#define CID_VOLT    6
#define LIGHT_SENSOR_ANALOG_PIN 0

#define BAT_PIN A0
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

MyMessage msgBTEMP(CID_BTEMP, V_TEMP);
MyMessage msgBBARO(CID_BBARO, V_PRESSURE);
MyMessage msgBFORC(CID_BBARO, V_FORECAST);
MyMessage msgVOLT(CID_VOLT, V_VAR2);

//MyMessage on(CHILD_ID, V_SCENE_ON);
//MyMessage off(CHILD_ID, V_SCENE_OFF)
boolean metric = false; 
int lastLightLevel;
int lightLevel;
float humidity=0.0;
float temperature=0.0;

boolean timeReceived = false;
unsigned long lastUpdate=0, lastRequest=0;
String RGBstr = "7b4ef";

int red, green, blue;
int degF, absHg, adjHg, errorFlag=9; //BMP180 vars
//double degInF, baroInHg; 
 //float max_battery_voltage = 4100L;

void setup()  
{ 

  
  //pinMode(A2, INPUT);

  gw.begin(incomingMessage, 220, false);
  gw.sendSketchInfo("UNO BB Light Sensor", "1.0");
  gw.present(CID_LIGHT, S_LIGHT_LEVEL);
  gw.present(CID_HUM, V_HUM);
  gw.present(CID_TEMP, S_TEMP);
  gw.present(CID_BBARO, S_BARO);
  gw.present(CID_BTEMP, S_TEMP);
  //gw.present(CID_VOLT, S_CUSTOM);
  //gw.present(CID_RGB, S_CUSTOM);
  //gw.present(CHILD_ID, S_SCENE_CONTROLLER);
  
  //HEXtoRGB(RGBstr);
  //analogWrite(BLUE, 254);
  //metric = gw.getConfig().isMetric;
  
   if (pressure.begin())
    Serial.println(F("BMP180 init success"));
  else{
    Serial.println(F("BMP180 init fail\n\n"));
    while(1); // Pause forever.
  }
  
  //DEBUG_PRINT ("SEtup complete \n");
  //Serial.println("SEtup complete");
  
    for (int thisPin=0; thisPin < 3; thisPin++) { 
      pinMode(RGB_PINS[thisPin], OUTPUT); 
      //Serial.print("PIN: "); Serial.println(RGB_PINS[thisPin]); 
      analogWrite(RGB_PINS[thisPin], 254); delay(500);
      analogWrite(RGB_PINS[thisPin], 0);   delay(300);
  }
  
  
  
}


void receiveTime(unsigned long time) {
  DEBUG_PRINT("TIME");
  DEBUG_PRINTDEC(time);
  setTime((unsigned long)(time)); 
  timeReceived = true;
}

void loop()      
{     
  unsigned long now = millis();
  //((1200000+470000)/470000*1.1)/1023=0.00382063601
  int vcc_val = 33; //analogRead(A2); // 6666; //readVcc();
  //float batteryV  = vcc_val * 0.00382;  //0.003866392;
  //int batteryPcnt = (int)batteryV/10;
  //Serial.print("battery"); Serial.println(batteryPcnt, DEC);
  //Serial.print("battery222"); Serial.println(max_battery_voltage, DEC);
  //DEBUG_PRINT ("BATTERY"); DEBUG_PRINTDEC(max_battery_voltage);

  if ((!timeReceived && now-lastRequest > 5*1000)
    || (timeReceived && now-lastRequest > 60*1000*6)) { 
    DEBUG_PRINT ("requesting time \n");
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

    DEBUG_PRINT (" ");
    Serial.print(month());
    DEBUG_PRINT (" ");
    Serial.print(day());
    DEBUG_PRINT (" ");
    Serial.print(year());
    DEBUG_PRINT (" ");
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    DEBUG_PRINT ("/n");
    lastUpdate = now;
    readBMP180(ALTITUDEmeter);
    //gw.send(msgVOLT.set(batteryPcnt));
    
  }



  if (lightLevel != lastLightLevel) {
    gw.send(msgHUM.set(humidity, 1));
    gw.send(msgTEMP.set(temperature, 1));
    gw.send(msgLIGHT.set(lightLevel));
    //gw.sendBatteryLevel(batteryPcnt);
    lastLightLevel = lightLevel;
  }

  gw.wait(4500);
  
  //gw.wait(1500);

  DEBUG_PRINT("TEMP: "); DEBUG_PRINTDEC(degInF);
  DEBUG_PRINT("\nHUM: "); DEBUG_PRINTDEC(baroInHg);
  //gw.wait(2000);
  //gw.sleep(15000);

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
  DEBUG_PRINT("r:"); DEBUG_PRINTDEC(r);
  DEBUG_PRINT("g:"); DEBUG_PRINTDEC(g);
  DEBUG_PRINT("b:"); DEBUG_PRINTDEC(b);
  
  for (int i = 0; i<3; i++) {
      analogWrite(RGB_PINS[i],RGB_values[i]);
  }
  
}


void incomingMessage(const MyMessage &message)
{

  int barrotemp = atoi(message.data);
  
  DEBUG_PRINT("Rcvd from UNO\n");
  DEBUG_PRINT("Sender: "); 
  DEBUG_PRINTDEC(message.sender);
  DEBUG_PRINT("Last: "); 
  DEBUG_PRINTDEC(message.last);
  DEBUG_PRINT("Sensor: "); 
  DEBUG_PRINTDEC(message.sensor);

  DEBUG_PRINT("Type: "); 
  DEBUG_PRINTDEC(message.type);
  DEBUG_PRINT("Data: "); 
  //DEBUG_PRINTDEC(message.data);

  if (message.type==V_VAR1) {
    
      String hexstring = message.getString();
      DEBUG_PRINT("HEX STRING: "); DEBUG_PRINTDEC(hexstring);
      HEXtoRGB(hexstring);
  }

}




