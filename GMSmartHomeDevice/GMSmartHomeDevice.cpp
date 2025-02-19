//V 1.0

#include "GMSmartHomeDevice.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

SHDevice::SHDevice(){
  SHDevice::timestamp = 0;
  SHDevice::error_count = 0;
  SHDevice::warning_count = 0;
}

bool SHDevice::startup(String objName, String usrnm, String pswrd, int rgbPins[3], void(*callback)(int), float colorFactor[3], int promptCount = 60){
    // Initialize the known pins
    SHDevice::RED_PIN = rgbPins[0];
    SHDevice::GREEN_PIN = rgbPins[1];
    SHDevice::BLUE_PIN = rgbPins[2];
    SHDevice::mqttCallback = callback;
    SHDevice::connect_topic = objName;
    SHDevice::control_topic = objName + "/control";
    SHDevice::status_topic = objName + "/status";
    SHDevice::mqttPswrd = pswrd;
    SHDevice::mqttUsrnm = usrnm;
    SHDevice::promptCount = promptCount;

    pinMode(SHDevice::RED_PIN,OUTPUT);
    pinMode(SHDevice::GREEN_PIN, OUTPUT);
    pinMode(SHDevice::BLUE_PIN, OUTPUT);

    // Blink each color to ensure proper operation of RGB lED
    rgbControl(1,0,0);
    delay(500);
    rgbControl(0,1,0);
    delay(500);
    rgbControl(0,0,1);
    delay(500);

    Serial.println("Connecting to Wifi, could take up to 1 minute ...");
    rgbControl(1,0,0);
    SHDevice::wifiStartup();
    Serial.println(SHDevice::wifiStatusMessage);
    if (SHDevice::wifiGetStatus()){
        rgbControl(1,1,0);

        SHDevice::connectMQTT();
        Serial.println(SHDevice::mqttStatusMsg);
        if (mqttClient.connected()){
            rgbControl(1,1,1);
            return true;
        } 
        return false;
    }
    return false;
}

void SHDevice::rgbControl(float r, float g, float b){
  SHDevice::rgbVal[0] = r * SHDevice::colorFactor[0];
  SHDevice::rgbVal[1] = g * SHDevice::colorFactor[1];
  SHDevice::rgbVal[2] = b * SHDevice::colorFactor[2];

  int red = 4095 - (4095 * r);
  int green = 4095 - (4095 * g);
  int blue = 4095 - (4095 * b);
  analogWrite(SHDevice::RED_PIN, red);
  analogWrite(SHDevice::GREEN_PIN, green);
  analogWrite(SHDevice::BLUE_PIN, blue);
}

void SHDevice::rgbBlink(int ms){
    float rgbTemp[] = {rgbVal[0], rgbVal[1], rgbVal[2]};
    rgbControl(0,0,0);
    delay(ms);
    rgbControl(rgbTemp[0], rgbTemp[1], rgbTemp[2]);
    delay(ms);
}

void SHDevice::run(int timeStep){
  SHDevice::timestamp = millis() / 1000;
  if(!SHDevice::wifiGetStatus()){
    rgbControl(1,0,0);
    Serial.print("Uptime (s): ");
    Serial.print(SHDevice::timestamp);
    Serial.println(" - Failed to connect to Wifi...Retrying");
    SHDevice::wifiStartup();
    Serial.println(SHDevice::wifiStatusMessage);
    delay(1000);
  }

  else if(!mqttClient.connected()){
    rgbControl(1,1,0);
    Serial.print("Uptime (s): ");
    Serial.print(SHDevice::timestamp);
    Serial.println(SHDevice::wifiGetTimeString() + " - FAILED to connect to MQTT Broker...Retrying");
    SHDevice::connectMQTT();
    Serial.println(SHDevice::getMqttStatus());
  }

  else{
    mqttClient.poll();
    SHDevice::mqttRead();
    SHDevice::logic();

    // If an error has been posted, check to see if it should be published
    if (SHDevice::error){
      SHDevice::warning_count = 0;
      SHDevice::status.warning = "0";
      SHDevice::status.error_state = false;
      if(SHDevice::error_count < SHDevice::promptCount){
        SHDevice::error_count ++;
        SHDevice::status.error = "0";
        SHDevice::status.error_state = false;
      } else{
        SHDevice::status.error = "1";
        SHDevice::status.error_state = true;
        rgbControl(1,0,1);
      }
    }

    // If a warning has been posted, check to see if it should be published
    else if(SHDevice::warning){
      SHDevice::error_count = 0;
      SHDevice::status.error = "0";
      SHDevice::status.error_state = false;
      if(SHDevice::warning_count < SHDevice::promptCount){
        SHDevice::warning_count ++;
        SHDevice::status.warning = "0";
        SHDevice::status.error_state = false;
      } else{
        SHDevice::status.warning = "1";
        SHDevice::status.error_state = true;
        rgbControl(0,1,1);
      }
    }

    //If no error or warning has been published, keep the LED in normal mode
    else{
      rgbControl(1,1,1);
      SHDevice::error_count = 0;
      SHDevice::warning_count = 0;
      SHDevice::status.error_state = false;
      SHDevice::status.warning = "0";
      SHDevice::status.error = "0";
    }

    SHDevice::mqttWrite(connect_topic, "1");
    SHDevice::mqttWrite(SHDevice::status_topic, SHDevice::status.to_json());
    //SHDevice::mqttWrite(SHDevice::warning_topic, String(int(SHDevice::warning)));
    //SHDevice::mqttWrite(SHDevice::error_topic, String(int(SHDevice::error)));
    delay(timeStep);
  }
}

void SHDevice::writeData(String topic, String data){SHDevice::mqttWrite(topic, data);}

String SHDevice::readData(){return SHDevice::getMqttPackage();}

String SHDevice::readControlData(){
  SHDevice::readData();
  Serial.println(SHDevice::getMqttPackageTopic());
  if (SHDevice::getMqttPackageTopic() == SHDevice::control_topic) return SHDevice::getMqttPackage();
  return (String)NULL;
}

void SHDevice::postWarningVal(bool val){SHDevice::warning = val;}

void SHDevice::postErrorVal(bool val){SHDevice::error = val;}

void SHDevice::postBattery(float val){SHDevice::status.battery = String(val);}

bool SHDevice::connectMQTT(){
  int connectCount = 3;
  
  mqttClient.setUsernamePassword(SHDevice::mqttUsrnm, SHDevice::mqttPswrd);
  while ((connectCount > 0) && (!mqttClient.connect(BROKER, PORT))) {
    connectCount = connectCount -1 ;
  }
    
  if (!mqttClient.connected()) {
    SHDevice::mqttStatusMsg = (String)millis();
    SHDevice::mqttStatusMsg.concat(" - MQTT connection FAILED! Error code = ");
    SHDevice::mqttStatusMsg.concat(mqttClient.connectError());
    return false;
  }

  mqttClient.onMessage(SHDevice::mqttCallback);
  mqttClient.subscribe(SHDevice::control_topic);

  SHDevice::mqttStatusMsg = "Successfully Connected to MQTT Broker!";
  return true;
}

String SHDevice::getMqttStatus() {return SHDevice::mqttStatusMsg;}

bool SHDevice::mqttConnected() {return mqttClient.connected();}

void SHDevice::mqttRead(){
  String dataIn = "";
  SHDevice::mqttPackageTopic = (String)mqttClient.messageTopic();
  while(mqttClient.available()){
    dataIn.concat((char)mqttClient.read());
  }

  if (SHDevice::mqttPackageTopic == SHDevice::control_topic) SHDevice::mqttPackage = dataIn;
}

void SHDevice::mqttWrite(String topic, String msg){
  mqttClient.beginMessage(topic);
  mqttClient.print(msg);
  mqttClient.endMessage();
}

String SHDevice::getMqttPackageTopic() {return SHDevice::mqttPackageTopic;}

String SHDevice::getMqttPackage() {return SHDevice::mqttPackage;}

bool SHDevice::wifiStartup(){ 
  int connectCount = 3;   //Number of times allowed to try and connect to wifi

  char ssid[] = WIFI_SSID;
  char pass[] = WIFI_PASS;

  //Attempt to connect to network
  SHDevice::wifiStatusMessage = "Attempting to Connect to ";
  SHDevice::wifiStatusMessage.concat(ssid);
  SHDevice::wifiStatusMessage.concat(". Attempts Left: ");
  SHDevice::wifiStatusMessage.concat(connectCount);
  while ((connectCount > 0) && (WiFi.begin(ssid, pass) != WL_CONNECTED)) {
    // failed, retry after waiting a second
    connectCount = connectCount - 1;
    SHDevice::wifiStatusMessage = "Attempting to Connect to ";
    SHDevice::wifiStatusMessage.concat(ssid);
    SHDevice::wifiStatusMessage.concat(". Attempts Left: ");
    SHDevice::wifiStatusMessage.concat(connectCount);
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED){
    SHDevice::wifiStatusMessage = "FAILED to Connect to ";
    SHDevice::wifiStatusMessage.concat(ssid);
  } else {
    delay(10000);
    //Save IP Address
    SHDevice::ip = WiFi.localIP();
    SHDevice::ipString = SHDevice::ip[0];
    SHDevice::ipString.concat(".");
    SHDevice::ipString.concat(SHDevice::ip[1]);
    SHDevice::ipString.concat(".");
    SHDevice::ipString.concat(SHDevice::ip[2]);
    SHDevice::ipString.concat(".");
    SHDevice::ipString.concat(SHDevice::ip[3]);
    
    
    //Save Success Message with Timestamp
    SHDevice::wifiStatusMessage = SHDevice::wifiGetTimeString();
    SHDevice::wifiStatusMessage.concat(" - Successfully Connected to ");
    SHDevice::wifiStatusMessage.concat(ssid);
    SHDevice::wifiStatusMessage.concat(" IP: ");
    SHDevice::wifiStatusMessage.concat(SHDevice::ipString);
    
    WiFi.macAddress(SHDevice::mac);
    
  }
  return WiFi.status() == WL_CONNECTED;
}

bool SHDevice::wifiGetStatus(){return WiFi.status() == WL_CONNECTED;}

String SHDevice::wifiGetTimeString(){
    String output = "(YMDHMS)";
    setTime(CST);
    output.concat(year());
    output.concat(":");
    output.concat(month());
    output.concat(":");
    output.concat(day());
    output.concat(":");
    output.concat(hour());
    output.concat(":");
    output.concat(minute());
    output.concat(":");
    output.concat(second());
    return String(millis());
}

int SHDevice::wifiGetCurentTime(){return millis();}
