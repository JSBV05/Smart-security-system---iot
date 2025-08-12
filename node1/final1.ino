#include <Servo.h>
#include <Keypad.h>

// --- Security System Variables ---
bool securitySystemON = false;
String enteredPassword = "";
const String correctPassword = "12345";

// --- Servo Setup ---
Servo doorServo;
int lockPosition = 0;
int unlockPosition = 90;

// --- Keypad Setup ---
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {8, 9, 10, 11};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Sensor and Buzzer Pins ---
const int pirPin = 12;           // PIR sensor input
const int slotSensorPin = 2;    // Door slot sensor input
const int buzzerPin = 13;       // Buzzer output

// --- State Tracking ---
bool lastMotion = -1;
bool lastDoor = -1;
bool lastSecurity = -1;

void setup() {
  Serial.begin(9600);

  // Setup sensors and output
  pinMode(pirPin, INPUT);
  pinMode(slotSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH); // OFF initially

  // Setup servo
  doorServo.attach(3);
  doorServo.write(unlockPosition);

  Serial.println("READY");
}

void loop() {
  checkSerialCommands();
  checkKeypad();
  checkSensors();
}

// --- Serial Command Handling ---
void checkSerialCommands() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "ARM") {
      armSystem();
      Serial.println("System Armed from Web");
    } else if (cmd == "DISARM") {
      disarmSystem();
      Serial.println("System Disarmed from Web");
    } else if (cmd == "STATUS") {
      Serial.println(securitySystemON ? "STATUS:ON" : "STATUS:OFF");
    } else {
      Serial.println("Unknown Command");
    }
  }
}

// --- Keypad Handling ---
void checkKeypad() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("KEY:");
    Serial.println(key);

    if (key == '#') {
      if (enteredPassword == correctPassword) {
        toggleSystem();
      } else {
        Serial.println("ALERT:Wrong Password");
      }
      enteredPassword = "";
    } else if (key == '*') {
      enteredPassword = "";
    } else {
      enteredPassword += key;
    }
  }
}

void toggleSystem() {
  if (securitySystemON) {
    disarmSystem();
  } else {
    armSystem();
  }
}

void armSystem() {
  doorServo.write(lockPosition);
  securitySystemON = true;
  Serial.println("STATUS:ON");
}

void disarmSystem() {
  doorServo.write(unlockPosition);
  securitySystemON = false;
  digitalWrite(buzzerPin, HIGH); // Turn off buzzer
  Serial.println("STATUS:OFF");
}

// --- Sensor and Buzzer Handling ---
void checkSensors() {
  int motionDetected = digitalRead(pirPin);
  int doorOpen = digitalRead(slotSensorPin);

  // Only print when there's a change
  if (securitySystemON != lastSecurity || motionDetected != lastMotion || doorOpen != lastDoor) {
    Serial.print("SECURITY:");
    Serial.print(securitySystemON ? "ON" : "OFF");
    Serial.print(",MOTION:");
    Serial.print(motionDetected == HIGH ? "YES" : "NO");
    Serial.print(",DOOR:");
    Serial.println(doorOpen == HIGH ? "OPEN" : "CLOSED");

    lastMotion = motionDetected;
    lastDoor = doorOpen;
    lastSecurity = securitySystemON;
  }

  // Buzzer logic
  if (securitySystemON) {
    if (doorOpen == HIGH) {
      digitalWrite(buzzerPin, LOW); // Turn ON buzzer
    } else {
      digitalWrite(buzzerPin, HIGH); // Turn OFF buzzer
    }
  } else {
    digitalWrite(buzzerPin, HIGH); // OFF when disarmed
  }
}

