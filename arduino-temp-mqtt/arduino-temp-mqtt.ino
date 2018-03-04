/**
* Connects to a WiFi network, 
* reads a temperature from a
* DHT-22 device and publishes
* and subscribes to a MQTT 
* broker.
* 
* Requirements:
* - ESP8266 MCU
* - DHT-22 sensor
* - Correct Libraries
*/



#include <DHT.h>;
#include <ESP8266WiFiMulti.h>
#include <MQTTClient.h>



/* Configuration Constants, change before use */
#define BAUD 9600            //! baud rate for comms
#define DHTPIN 2            //! PIN
#define DHTTYPE DHT22       //! DHT 22  (AM2302)
#define SIZE 20             //! size of buffer for storing temp
#define BROKER "broker.shiftr.io"
#define CLIENTID  "arduino-ad"
#define CLIENT_USER_NAME "try"
#define CLIENT_PASS "try"
#define TOPIC_PUBLISH "/arduino-temp-test101"
#define TOPIC_SUBSCRIBE "/arduino-temp-test101" /* For debugging subscribe to 
                                                 topic that we publish on */
#define WIFIAP "AP"
#define WIFIKEY "KEY"


// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
 
/* globals */
float hum;  /* Stores humidity value    */
float temp; /* Stores temperature value */
unsigned long lastMillis = 0;
MQTTClient client; /* Mttq client object */
WiFiClient net;    /* WIFI client object */


/**
 *  Inital setup procedure, connects to wifi, 
 *  and sets up MQTT broker
 */
void setup()
{
  /* setup serial comms and dht */
  Serial.begin(BAUD);
  dht.begin();
  /* Connect to wifi */
  connect();

  /* Setup MQTT Broker */
  client.begin(BROKER, net);
  
  /* fucntion to call on message received */
  client.onMessage(messageReceived);

  /* Connect to broker */
  while (!client.connect(CLIENTID, CLIENT_USER_NAME, CLIENT_PASS)) {
    Serial.print(".");
    delay(1000);
  }

  /*subscribe to a topic, for debugging it should
     be the topic you publish to */
  client.subscribe(TOPIC_SUBSCRIBE);

}

/**
 * Main Loop with a 2 seconds delay
 * Reads temperature and publishes it.
 * Also checks for any new messages in 
 * subscribed topic.
 */
void loop()
{

  /* check for messages from scbscribed topoic */
  client.loop();

  /* see git page TODO: add link */
  delay(10);  // <- fixes some issues with WiFi stability

  
  if (!client.connected())
  {
    /* warn user something isnt working */
    Serial.println("MQTT service was not able to connect ");;
  }


  /* Read temp */
  temp = dht.readTemperature();
  
  /* Print temp and humidity values to serial monitor */
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  /* flush serial output */
  Serial.flush();

  
  char str[SIZE];
  //snprintf(str, SIZE, "%f", temp);// %f format not supported in arduino libraries
  /* copy float to a char array TODO: how many significant figures? */
  dtostrf(temp, 4, 2, str);
  
  /* publish a message roughly every  5 seconds */
  if (millis() - lastMillis > 5000)
  {
    lastMillis = millis();
    client.publish(TOPIC_PUBLISH, str);
  }

  delay(2000); //Delay 2 sec.
}

/* connect to wifi 
*
*/
void connect()
{
  

  Serial.print("Connecting");
  WiFi.begin(WIFIAP, WIFIKEY);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());



}

/**
 *  Print to serial comms message received from broker for
 *  the subscribed topic
 */
void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}






