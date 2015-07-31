
#include <SPI.h>  
#include <MySensor.h>  
#include <MyGateway.h>  
#include <stdarg.h>
// 220;255;3;0;1;1438233491
//220;255;3;0;1;1496700
#define INCLUSION_MODE_TIME 0 // Number of minutes inclusion mode is enabled
#define INCLUSION_MODE_PIN 88 // Digital pin used for inclusion mode button
#define RADIO_ERROR_LED_PIN 56  // Error led pin
#define RADIO_RX_LED_PIN    54  // Receive led pin
#define RADIO_TX_LED_PIN    53  // the PCB, on board LED
#define M_PRESENTATION      0   // message-type presentation
#define M_SET_VARIABLE      1   // message-type presentation


//MyGateway gw(9, 10, INCLUSION_MODE_TIME);
MyGateway gw(8, 9, INCLUSION_MODE_TIME, INCLUSION_MODE_PIN, RADIO_RX_LED_PIN, RADIO_TX_LED_PIN, RADIO_ERROR_LED_PIN);
//MyMessage msgGET(5, V_DIMMER);

//string hexstring = "FF3Fa0";
//int number = (int) strtol( &hexstring, NULL, 16);
//int r = number >> 16;
//int g = number >> 8 & 0xFF;
//int b = number & 0xFF;


//s=child-sensor-id,
//c=message-type
//t=sub-type
//pt=payload-type (see MyMessage.h)
//l=length


char inputString[MAX_RECEIVE_LENGTH] = "";    // A string to hold incoming commands from serial/ethernet interface
int inputPos = 0;
boolean commandComplete = true;  // whether the string is complete

//200;5;1;0;3;22
//200;5;1;0;3;11
//202;5;1;0;3;77
//202;1;1;0;24;2
// 202;3;1;0;32;551520375

unsigned long previousMillis = 0; 
unsigned long previousMillis2 = 0;
const long interval = 50000; 
const long interval2 = 70000; 




void setup()  
{ 
  //gw.begin(RF24_PA_LEVEL_GW);
  delay(500);
  Serial.print("setup complete"); 
  delay(500);
  gw.begin();  
  delay(500);
  //gw.begin(RF24_PA_LEVEL_GW, RF24_CHANNEL, RF24_DATARATE, writeEthernet);

  // pinMode(4, INPUT); 
  // pinMode(5, INPUT); 
  // pinMode(6, INPUT); 
  //Serial.print("Node ID: ");
  //Serial.println(gw.getNodeId());
  Serial.println("setup complete");
  //Serial.print("0;6;0;0;3;My Light\n");
  //pinMode(5, OUTPUT); digitalWrite(5, LOW);
  
//  Serial.print("0;0;");
//  Serial.print(M_PRESENTATION);
//  Serial.print(";");
//  Serial.print(S_HUM) ;
//  Serial.print(";");
//  Serial.println(LIBRARY_VERSION) ;

    
}

//205;18;1;0;3;99
//205;16;1;0;2;1
//
// 1437026353
// 205;255;3;0;1;1437026353

void loop()  
{ 
  //gw.process();
  gw.processRadioMessage();   
  if (commandComplete) {
    // A command wass issued from serial interface
    // We will now try to send it to the actuator
    gw.parseAndSend(inputString);
    commandComplete = false;  
    inputPos = 0;
  }
  
//  unsigned long currentMillis = millis();
//  if(currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis; 
    // 12;6;0;0;3;My Light\n
    //Serial.print("200;5;1;0;3;22\n");
//    Serial.print("0;0;");
//    Serial.print(M_SET_VARIABLE);
//    Serial.print(";");
//    Serial.print(V_HUM) ;
//    Serial.print(";");
//    Serial.println(34) ;
//  }
  
  
  //unsigned long currentMillis = millis();
  //if(currentMillis - previousMillis2 >= interval2) {
    //previousMillis2 = currentMillis; 
    // 12;6;0;0;3;My Light\n
    //Serial.print("200;5;1;0;2;22\n");
    
  //}
  
  

  //gw.request(CHILD_ID_GET, S_DIMMER);  delay(3000);

  //13;7;1;0;2;1\n
}


//void incomingMessage(const MyMessage &message) {
//  if (message.type == V_DIMMER && message.sensor==5 ){
//    int testrxval = atoi(message.data);
//    gw.send(msgGET.set(testrxval)); //delay(1000);
//  }
//  Serial.println("xxx");
//}

//200;4;1;1;3;55
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputPos<MAX_RECEIVE_LENGTH-1 && !commandComplete) { 
      if (inChar == '\n') {
        inputString[inputPos] = 0;
        commandComplete = true;
      } 
      else {
        // add it to the inputString:
        inputString[inputPos] = inChar;
        inputPos++;
      }
    } 
    else {
      // Incoming message too long. Throw away 
      inputPos = 0;
    }
  }
}




