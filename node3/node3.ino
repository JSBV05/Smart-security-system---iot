const int smokeSensorPin = A0;
const int buzzerPin = 7;
const int smokeThreshold = 50;

int lastSmokeStatus = 0;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int smokeValue = analogRead(smokeSensorPin);
  bool smokeDetected = smokeValue > smokeThreshold;

  // Only print when status changes
  if (smokeDetected != lastSmokeStatus) {
    Serial.print("SMOKE:");
    Serial.print(smokeDetected ? "YES" : "NO");
    Serial.print(",VALUE:");
    Serial.println(smokeValue);

    lastSmokeStatus = smokeDetected;
  }

  // Buzzer control
  digitalWrite(buzzerPin, smokeDetected ? LOW : HIGH);

  delay(200);  // Short delay for responsive update
}



