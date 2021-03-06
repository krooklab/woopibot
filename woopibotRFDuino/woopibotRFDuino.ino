#include <RFduinoBLE.h>

#include "RunningMedian.h"

int led = 3;
unsigned long lastRssiTime;
unsigned long timeSinceLastRssi;

bool isConnected = false;

char bytes[1024];
int bytesIndex = 0;

// uitmiddelen volgens matthias variablen:
float RSSI_alpha = 0.2;
int RSSI_old = 0;
bool RSSI_firstRead = true;




RunningMedian rssiSamples = RunningMedian(50);
long count = 0;


void setup() {
  pinMode(led, OUTPUT);
  
  

  
//  RFduinoBLE.txPowerLevel = -20;
  
  RFduinoBLE.deviceName = "woopibot";
  RFduinoBLE.advertisementData = "";
  
  Serial.begin(9600);
  
  RFduinoBLE.begin();
}


void loop() {
//  RFduinoBLE.send(0);

  readSerial();
  
  RFduino_ULPDelay(SECONDS(0.040));
  
  // rssi nog eens zenden als het te lang geleden is dat we hem nog gezonden hebben:
  timeSinceLastRssi = millis() - lastRssiTime;
  if(timeSinceLastRssi > 50) {
    int rssiMedian = (int)rssiSamples.getMedian();
    sendRssi(rssiMedian);
    lastRssiTime = millis();
  }
}

void readSerial() {
  if(!Serial.available()) return;
  
  while(Serial.available()) {
    char val = Serial.read();
    bytes[bytesIndex++] = val;
  }
  
  RFduinoBLE.send(bytes, bytesIndex);
  bytesIndex = 0;
}


void RFduinoBLE_onConnect()
{
  digitalWrite(led, HIGH);
  isConnected = true;
}

void RFduinoBLE_onDisconnect()
{
  digitalWrite(led, LOW);
  isConnected = false;
}

void RFduinoBLE_onReceive(char *data, int len)
{
  Serial.print(data);
}

// returns the dBm signal strength indicated by the receiver
// received signal strength indication (-0dBm to -127dBm)
void RFduinoBLE_onRSSI(int rssi) {
  lastRssiTime = millis();
  
  if(!isConnected) return;
  
  // uitmiddelen volgens matthias:
//  if(RSSI_firstRead) {
//    RSSI_firstRead = false;
//    RSSI_old = rssi;
//  }else{
//    rssi = RSSI_alpha*RSSI_old + (1-RSSI_alpha)*rssi;
//    RSSI_old = rssi;
//  }


  // uitmiddelen over een window van 50 waarden:
  rssiSamples.add(rssi);
  int rssiMedian = (int)rssiSamples.getMedian();

  sendRssi(rssiMedian);
}

void sendRssi(int rssi) {
  // convert int to string and string to char array:
  String rssiString = String(rssi);
  int rssiStringLen = rssiString.length()+1; 
  char rssiCharArray[rssiStringLen];
  rssiString.toCharArray(rssiCharArray, rssiStringLen);
  
  
  // prepare BLE data:
  char data[rssiStringLen+3];
  data[0] = 0x02;
  data[1] = 'R';
  for(int i=0; i < rssiStringLen-1; i++) {
    data[i+2] = rssiCharArray[i];
  }
  data[rssiStringLen+1] = 0x03;
  
  RFduinoBLE.send(data, rssiStringLen+2);  
}
