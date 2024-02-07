#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

const char* ssid = "UW MPSK";
const char* password = "V3h_D_Ed{F"; // Replace with your network password
#define DATABASE_URL "https://power-management-c165d-default-rtdb.firebaseio.com" // Replace with your database URL
#define API_KEY "AIzaSyD0jXYBIdSh7ZIIVChgLxP4I2-XfLHSL3I" // Replace with your API key
int UPLOAD_INTERVAL = 3000;
#define STAGE_INTERVAL 12000 // 12 seconds each stage
#define SLEEP_DURATION 10000 
unsigned long lastBelowThresholdTime = 0;
const float distanceThreshold = 50.0; 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// HC-SR04 Pins
const int trigPin = 2;
const int echoPin = 3;

// Define sound speed in cm/usec
const float soundSpeed = 0.034;

// Function prototypes
float measureDistance();
void connectToWiFi();
void initFirebase();
void sendDataToFirebase(float distance, float upload_interval);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectToWiFi();

  Serial.println("Turning on Firebase and sending data every 1 second...");
  initFirebase();
  unsigned long startTime = millis();
  while (true)
  {
    float currentDistance = measureDistance();
    sendDataToFirebase(currentDistance, UPLOAD_INTERVAL);
    if(currentDistance <= distanceThreshold) {
      lastBelowThresholdTime = millis();
    }
    if (millis() - lastBelowThresholdTime >= 10000) {
      Serial.println("Distance > 50cm for 10 seconds. Going to sleep.");
      WiFi.disconnect(true);
      esp_sleep_enable_timer_wakeup(SLEEP_DURATION * 1000);
      esp_deep_sleep_start();
    }
    delay(100); // Delay between measurements
  }
}

void loop(){
  // This is not used
}

float measureDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * soundSpeed / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

void connectToWiFi()
{
  // Print the device's MAC address.
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void initFirebase()
{
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);
}

void sendDataToFirebase(float distance, float upload_interval){
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > upload_interval || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.pushFloat(&fbdo, "test/distance", distance)){
      Serial.println("PASSED");
      Serial.print("PATH: ");
      Serial.println(fbdo.dataPath());
      Serial.print("TYPE: " );
      Serial.println(fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.print("REASON: ");
      Serial.println(fbdo.errorReason());
    }
    count++;
  }
}

