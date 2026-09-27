// ArduinoClassExample.ino
// Controls an LED and a Servo motor based on ultrasonic distance measurements.
// - As an object gets closer, the LED blinks faster.
// - The servo sweeps like a dial gauge reflecting proximity/distance.

#include <Servo.h>

// --- Pin Definitions ---
constexpr uint8_t TRIG_PIN  = 2;
constexpr uint8_t ECHO_PIN  = 3;
constexpr uint8_t SERVO_PIN = 9;
constexpr uint8_t LED_PIN   = 13;

// --- Operational Parameters ---
constexpr float MIN_DISTANCE_CM = 5.0f;    // Minimum proximity limit
constexpr float MAX_DISTANCE_CM = 100.0f;  // Maximum active detection range

constexpr unsigned long MIN_BLINK_INTERVAL_MS = 60;   // Very fast blink when close
constexpr unsigned long MAX_BLINK_INTERVAL_MS = 1000; // Slow blink when far

constexpr unsigned long SENSOR_READ_INTERVAL_MS = 60;  // Sensor sampling interval
constexpr unsigned long SERIAL_LOG_INTERVAL_MS  = 300; // Serial monitor print interval

// --- Global Objects and State ---
Servo dialGauge;

unsigned long lastSensorReadMillis = 0;
unsigned long lastBlinkMillis = 0;
unsigned long lastLogMillis = 0;

float currentDistanceCm = MAX_DISTANCE_CM;
unsigned long currentBlinkIntervalMs = MAX_BLINK_INTERVAL_MS;
int currentServoAngle = 0;
bool ledState = LOW;

// Measure distance using HC-SR04 ultrasonic sensor
float readDistanceCm() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond HIGH pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse duration (timeout after ~30ms, approx. 5 meters)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    // Timeout or out of range: default to max distance
    return MAX_DISTANCE_CM;
  }

  // Speed of sound: 343 m/s = 0.0343 cm/microsecond (divided by 2 for round-trip)
  float distance = (duration * 0.0343f) / 2.0f;
  return constrain(distance, MIN_DISTANCE_CM, MAX_DISTANCE_CM);
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  dialGauge.attach(SERVO_PIN);
  dialGauge.write(0); // Start gauge at 0 degrees

  Serial.println(F("=================================================="));
  Serial.println(F("Arduino Proximity Gauge & Dynamic Blinker Started"));
  Serial.println(F("Pins: Trig=2, Echo=3, Servo=9, LED=13"));
  Serial.println(F("=================================================="));
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Periodic ultrasonic distance sampling
  if (currentMillis - lastSensorReadMillis >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadMillis = currentMillis;

    currentDistanceCm = readDistanceCm();

    // Map distance to blink interval: closer = faster blink (smaller interval)
    currentBlinkIntervalMs = map(
      (long)currentDistanceCm,
      (long)MIN_DISTANCE_CM,
      (long)MAX_DISTANCE_CM,
      MIN_BLINK_INTERVAL_MS,
      MAX_BLINK_INTERVAL_MS
    );

    // Map distance to servo angle: closer = higher dial angle (0 to 180 deg)
    currentServoAngle = map(
      (long)currentDistanceCm,
      (long)MIN_DISTANCE_CM,
      (long)MAX_DISTANCE_CM,
      180,
      0
    );

    dialGauge.write(currentServoAngle);
  }

  // 2. Non-blocking LED blink timer
  if (currentMillis - lastBlinkMillis >= currentBlinkIntervalMs) {
    lastBlinkMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // 3. Informative Serial logging
  if (currentMillis - lastLogMillis >= SERIAL_LOG_INTERVAL_MS) {
    lastLogMillis = currentMillis;

    Serial.print(F("Distance: "));
    Serial.print(currentDistanceCm, 1);
    Serial.print(F(" cm | Blink Interval: "));
    Serial.print(currentBlinkIntervalMs);
    Serial.print(F(" ms | Gauge Angle: "));
    Serial.print(currentServoAngle);
    Serial.println(F(" deg"));
  }
}
