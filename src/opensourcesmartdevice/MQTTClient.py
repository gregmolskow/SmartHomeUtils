"""
MQTTClient.py
This module provides a basic MQTT client implementation using the Paho MQTT library.
"""

import paho.mqtt.client as mqtt
from paho.mqtt.client import MQTTMessageInfo

class MQTTClient:
    """
    A simple MQTT client that connects to a broker, subscribes to topics, and publishes messages.
    This class should be subclassed to implement the on_message callback."""
    def __init__(self, broker, port):
        self.broker = broker
        self.port = port
        self.client = mqtt.Client()

        # Assign the on_message callback function
        self.client.on_message = self.on_message

    def on_message(self, client, userdata, msg:mqtt.MQTTMessage):
        """
        Callback function to handle incoming messages.
        This method should be overridden in a subclass to process messages.
        """
        raise NotImplementedError("on_message method must be implemented by subclass")

    def connect(self):
        """
        Connect to the MQTT broker.
        """
        self.client.connect(self.broker, self.port, 60)

    def subscribe(self, topic:str):
        """Subscribe to a topic.
        This method allows the client to listen for messages on a specified topic.

        Args:
            topic (str): The topic to subscribe to.
        """
        self.client.subscribe(topic)

    def publish(self,topic:str,  message:str) -> MQTTMessageInfo:
        """Publish a message to a topic.
        This method allows the client to send messages to a specified topic.
        Args:
            topic (str): The topic to publish the message to.
            message (str): The message to be published.
        Returns:
            MQTTMessageInfo: Information about the published message.
        """
        return self.client.publish(topic, message)

    def start(self):
        """
        Start the MQTT client loop.
        """
        self.client.loop_start()

    def stop(self):
        """
        Stop the MQTT client loop."""
        self.client.loop_stop()

# Example usage
if __name__ == "__main__":
    broker = 'your_broker_address'
    port = 1883
    topic_subscribe = 'your/subscribe/topic'
    topic_publish = 'your/publish/topic'
    message = 'Hello MQTT'

    mqtt_client = MQTTClient(broker, port)
    mqtt_client.connect()
    mqtt_client.subscribe(topic_subscribe)
    mqtt_client.publish(topic_publish, message)
    mqtt_client.start()

    # some logic to run until done

    mqtt_client.stop()

