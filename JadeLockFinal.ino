#include <WiFi.h>
#include <FirebaseESP32.h>
#define WIFI_SSID "JADELOCK"          //WIFI NAME CHANGE
#define WIFI_PASSWORD "Asdfghjkl54321"      //WIFI PASSWORD CHANGE

#define FIREBASE_HOST "https://projectdoor-9a3f5-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "FRUdwuJPtG55ZPtpkhZBtk6AoBtydNaqLoe0lw6b"
#define VIBRATION_SENSOR_PIN 34
#define BUZZER_PIN 26            
#define VIBRATION_THRESHOLD 4000 
#define RELAY_PIN 25 

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected to Wi-Fi");
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Firebase.setString(firebaseData, "/alert", "No update");
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

void loop() {
    intrudersensor();
}

void intrudersensor() {
    int sensorValue = analogRead(VIBRATION_SENSOR_PIN); 
    if (sensorValue > VIBRATION_THRESHOLD) {
        Serial.println("Intruder detected!");
        digitalWrite(BUZZER_PIN, HIGH);  
        delay(5000);  
        digitalWrite(BUZZER_PIN, LOW);   
        Firebase.setString(firebaseData, "/alert", "Intruder Detected");
    }
    delay(500);  
    lock();
}

void lock() {
    if (Firebase.getInt(firebaseData, "/door")) {
        int doorStatus = firebaseData.intData();
        if (doorStatus == 1) {
            Serial.println("Locking solenoid...");
            digitalWrite(RELAY_PIN, LOW); 
        } else if (doorStatus == 0) {
            Serial.println("Unlocking solenoid...");
            digitalWrite(RELAY_PIN, HIGH);  
        }
    } else {
        Serial.print("Failed to get door status: ");
        Serial.println(firebaseData.errorReason());
    }
}
