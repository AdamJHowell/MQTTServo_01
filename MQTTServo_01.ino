/*
    MQTTServo_01
    MQTT to servo position
    
    Create 2020-07-28
    by Adam Howell
    https://github.com/AdamJHowell
*/

#include <Servo.h>
#include <ESP8266WiFi.h>      // Network Client for the WiFi chipset.
#include <PubSubClient.h>     // PubSub is the MQTT API.

// WiFi and MQTT constants.
const char* ssid = "Red";
const char* password = "8012254722";
const char* mqttServer = "192.168.55.200";
const int mqttPort = 2112;
const char* mqttTopic = "mqttServo";

WiFiClient espClient;
PubSubClient mqttClient( espClient );
Servo onlyServo;  // Create servo object to control a servo.
// Twelve servo objects can be created on most boards using GPIO pins.


void callback( char* topic, byte* payload, unsigned int length )
{
  Serial.print( "Message arrived [" );
  Serial.print( topic );
  Serial.print( "] " );
  for ( int i = 0; i < length; i++ )
  {
    char receivedChar = ( char )payload[i];
    Serial.print( receivedChar );
    // Note that some boards consider 'HIGH' to be off.
    if ( receivedChar == '0' )
      digitalWrite( LED_BUILTIN, HIGH );    // Turn the LED off.
    if ( receivedChar == '1' )
      digitalWrite( LED_BUILTIN, LOW );     // Turn the LED on.
    if ( receivedChar == 'a' )
    {
      Serial.println();
      moveServo( 15, 7 );
      Serial.print( "µs for 15°  : " );
      Serial.println( onlyServo.readMicroseconds() );
    }
    if ( receivedChar == 'b' )
    {
      Serial.println();
      moveServo( 165, 7 );
      Serial.print( "µs for 165° : " );
      Serial.println( onlyServo.readMicroseconds() );
    }
    if ( receivedChar == 'c' )
    {
      Serial.println();
      moveServo( 90, 7 );
      Serial.print( "µs for 90°  : " );
      Serial.println( onlyServo.readMicroseconds() );
    }
  }
}


void reconnect()
{
  // Loop until we're reconnected.
  while ( !mqttClient.connected() )
  {
    Serial.print( "Attempting MQTT connection..." );
    if ( mqttClient.connect( "ESP8266 Client" ) )         // Attempt to connect using the designated clientID.
    {
      Serial.println( "connected" );
      mqttClient.subscribe( mqttTopic );                // Subscribe to the designated MQTT topic.
    }
    else
    {
      Serial.print( " failed, return code: " );
      Serial.print( mqttClient.state() );
      Serial.println( " try again in 5 seconds" );
      // Wait 5 seconds before retrying.
      delay( 5000 );
    }
  }
}


void setup()
{
  onlyServo.attach( 2 );      // Attaches the servo on GIO2 to the servo object.
  onlyServo.write( 90 );      // Move the servo to its center position.
  Serial.begin( 115200 );   // Start the Serial communication to send messages to the computer.
  delay( 10 );
  Serial.println( '\n' );

  mqttClient.setServer( mqttServer, mqttPort );   // Set the MQTT client parameters.
  mqttClient.setCallback( callback );             // Assign the callback() function to handle MQTT callbacks.

  pinMode( LED_BUILTIN, OUTPUT );             // Initialize digital pin LED_BUILTIN as an output.

  WiFi.begin( ssid, password );               // Connect to the WiFi network.
  Serial.print( "WiFi connecting to " );
  Serial.println( ssid );

  int i = 0;
  /*
     WiFi.status() return values:
     0 : WL_IDLE_STATUS when WiFi is in process of changing between statuses
     1 : WL_NO_SSID_AVAIL in case configured SSID cannot be reached
     3 : WL_CONNECTED after successful connection is established
     4 : WL_CONNECT_FAILED if password is incorrect
     6 : WL_DISCONNECTED if module is not configured in station mode
  */
  while ( WiFi.status() != WL_CONNECTED )     // Wait for the WiFi to connect.
  {
    delay( 1000 );
    Serial.println( "Waiting for a connection..." );
    Serial.print( "WiFi status: " );
    Serial.println( WiFi.status() );
    Serial.print( ++i );
    Serial.println( " seconds" );
  }

  Serial.println( '\n' );
  Serial.println( "Connection established!" );
  Serial.print( "IP address:\t" );
  Serial.println( WiFi.localIP() );           // Send the IP address of the ESP8266 to the computer.
}


void moveServo( int position, int stepTime )
{
  if ( position < 0 || position > 180 )
  {
    Serial.print( "moveServo() was given an invalid servo position: " );
    Serial.println( position );
  }
  else
  {
    int currentPosition = onlyServo.read();
    if ( currentPosition <= position )
    {
      for ( ; currentPosition <= position; currentPosition += 1 ) // Range is from 0 degrees to 180 degrees.
      {
        onlyServo.write( currentPosition );               // Move the servo to the position in variable 'pos'.
        delay( stepTime );                    // Wait for the servo to reach the position.
      }
    }
    else
    {
      for ( ; currentPosition > position; currentPosition -= 1 ) // Range is from 0 degrees to 180 degrees.
      {
        onlyServo.write( currentPosition );               // Move the servo to the position in variable 'pos'.
        delay( stepTime );                    // Wait for the servo to reach the position.
      }
    }
  }
}


void loop()
{
  int pos;    // A position variable for the servo.
  // My servos are fully counter-clockwise when 0 is written to them.

  if ( !mqttClient.connected() )
  {
    reconnect();
  }
  mqttClient.loop();
}
