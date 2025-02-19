/*
GM Smart Home Device Header File

Created March 19, 2023

V 1.0
*/

#ifndef SHDevice_h
#define SHDevice_h

#include "Arduino.h"
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <TimeLib.h>
#include "secrets.h"



struct statusCode{

    // Codes for the vacuum type in home assistant
    bool idle_state;
    bool error_state;
    bool cleaning_state;
    bool docked_state;
    bool paused_state;
    bool returning_state;

    String battery;
    String error;
    String warning;
    

    String to_json(){
        
        String state = "";
        
        error_state = (!error.equals("") or !warning.equals(""));

        if(idle_state)              state = "idle";
        else if(error_state){state = "error";}
        else if(cleaning_state)     state = "cleaning";
        else if(docked_state)       state = "docked";
        else if(paused_state)       state = "paused";
        else if (returning_state)   state = "returning";

        String stateJSON = "state:"+state+",";
        String batteryJSON = "battery:"+battery+","; 
        String errorJSON = "error_code:"+error+",";
        String warningJSON = "warning_code:"+warning+",";



        return "{"+stateJSON+batteryJSON+errorJSON+warningJSON+"}";
    }
};


class SHDevice{
    public:
        // Interface Items
        SHDevice();
        bool startup(String objName, String usrnm, String pswrd,int rgbPins[3], void(*callback)(int), float colorFactor[3], int promptCount);
        void rgbControl(float red, float green, float blue);
        void rgbBlink(int ms = 500);
        void run(int timeStep = 500);
        void writeData(String topic, String data);
        void postWarningVal(bool val);
        void postErrorVal(bool val);
        void postBattery(float val);
        void (*logic)();
        int promptCount;
        String readData();
        String readControlData();

        


    private:
        float rgbVal[3];
        float colorFactor[3];
        int RED_PIN;
        int GREEN_PIN;
        int BLUE_PIN;
        void(*mqttCallback)(int);
        int timestamp;
        int threshold;
        bool error;
        int error_count;
        bool warning;
        int warning_count;
        String connect_topic;
        String control_topic;
        String status_topic;
        statusCode status;

        //MQTT Items
        String mqttPswrd;
        String mqttUsrnm;
        String mqttStatusMsg;
        String mqttPackage;
        String mqttPackageTopic;
        void mqttRead();
        bool connectMQTT();
        String getMqttStatus();
        bool mqttConnected();
        void mqttWrite(String topic, String msg);
        String getMqttPackageTopic();
        String getMqttPackage();

        //Wifi Items
        bool wifiStartup();
        String wifiStatusMessage;
        bool wifiGetStatus();
        int wifiGetCurentTime();
        String wifiGetTimeString();
        byte mac[6];
        IPAddress ip;
        String ipString;

};

#endif