Smart Home Device
=================

The Smart Home Device module provides a set of classes and functions to interact with MQTT to behvaeas a wifi smart home device. This device is intended to be used with an MQTT Broker and Home Assistant. 

This class expects a 'secrets.h' file to be present in the same directory as the main project file. This file should contain the following information:

.. code-block:: cpp

    #define WIFI_SSID // The SSID of the wifi network
    #define WIFI_PASS // The password of the wifi network
    #define SOFTWARE_VERSION 0.0.1
    #define CST 72000 // The timezone offset in seconds
    #define BROKER // The IP address of the MQTT Broker
    #define PORT 1883 // The port of the MQTT Broker
    #define DEVICE_NAME // The name of the device
    #define USRNM // The username for the MQTT Broker
    #define PSWRD // The password for the MQTT Broker


.. doxygenFile:: GMSmartHomeDevice.h

