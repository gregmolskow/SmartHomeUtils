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

/**
 * @brief Structure to hold the status codes for the smart home device.
 */
struct statusCode{
    bool idle_state;      ///< Indicates if the device is in idle state.
    bool error_state;     ///< Indicates if the device is in error state.
    bool cleaning_state;  ///< Indicates if the device is in cleaning state.
    bool docked_state;    ///< Indicates if the device is docked.
    bool paused_state;    ///< Indicates if the device is paused.
    bool returning_state; ///< Indicates if the device is returning to dock.

    String battery;       ///< Battery status of the device.
    String error;         ///< Error message of the device.
    String warning;       ///< Warning message of the device.

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

/**
 * 
 * @brief Class representing a smart home device to interface over MQTT. This class is intended to be used with HomeAssistant.
 */
class SHDevice{
    public:
        /**
         * @brief Constructor for SHDevice class.
         */
        SHDevice();

        /**
         * @brief Initializes the smart home device.
         * 
         * @param objName The name of the object.
         * @param usrnm The username for MQTT connection.
         * @param pswrd The password for MQTT connection.
         * @param rgbPins An array containing the RGB pin numbers.
         * @param callback A callback function for MQTT messages.
         * @param colorFactor An array containing color factors.
         * @param promptCount The number of message prompts before pusshing the message to HA.
         * @return true if startup is successful, false otherwise.
         */
        bool startup(String objName, String usrnm, String pswrd,int rgbPins[3], void(*callback)(int), float colorFactor[3], int promptCount);

        /**
         * @brief Controls the RGB LED.
         * 
         * @param red The red color intensity.
         * @param green The green color intensity.
         * @param blue The blue color intensity.
         */
        void rgbControl(float red, float green, float blue);

        /**
         * @brief Blinks the RGB LED with the current LED color.
         * 
         * @param ms The duration of each blink in milliseconds (default is 500).
         */
        void rgbBlink(int ms = 500);

        /**
         * @brief Runs the main logic of the device. This function checks the status of the device and posts the status to the MQTT broker.
         * 
         * @param timeStep The time step for each run cycle in milliseconds (default is 500).
         */
        void run(int timeStep = 500);

        /**
         * @brief Writes data to a specified MQTT topic.
         * 
         * @param topic The MQTT topic.
         * @param data The data to be written.
         */
        void writeData(String topic, String data);

        /**
         * @brief Posts a warning value to the device.
         * 
         * @param val The warning value.
         */
        void postWarningVal(bool val);

        /**
         * @brief Posts an error value to the device.
         * 
         * @param val The error value.
         */
        void postErrorVal(bool val);

        /**
         * @brief Posts the battery status.
         * 
         * @param val The battery status value.
         */
        void postBattery(float val);

        /**
         * @brief A pointer to the micro-controller logic function. This would be the execution of commands or the sensor reading logic.
         */
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