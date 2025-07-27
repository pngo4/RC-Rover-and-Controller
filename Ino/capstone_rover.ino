#include <WiFiConfig.h>

RemoteData data; //Initialize RemoteData to store values

// Pin assignments
const int EN12 = 5;  // Enable for 1,2
const int EN34 = 10; // Enable for 3,4
const int LEFT_IN1 = 3; // 1A ("forward" HIGH)
const int LEFT_IN2 = 4; // 2A

const int RIGHT_IN1 = 1; // 3A ("forward" HIGH);
const int RIGHT_IN2 = 0; // 4A

int CENTER_X, CENTER_Y;
int CAL_SAMPLES = 100;

int deadzone = 20;

//pins for the ultrasonic sensor
const int echo = 8;
const int trig = 9;


//joystick input calibration and normalization function

//normalize readings to -1, 0, 1
int normalizeAxis(uint8_t val, uint8_t center) {
  if (val < center - deadzone) return -1;
  else if (val > center + deadzone) return 1;
  else return 0;
}
//calibrates it to new center eveerytime based on readings 
void calibrateJoystick() {
  long sx = 0, sy = 0;
  for (int i = 0; i < CAL_SAMPLES; ++i) {
    delay(20);
    sx += data.vx;
    sy += data.vy;           
  }
  CENTER_X = sx / CAL_SAMPLES;
  CENTER_Y = sy / CAL_SAMPLES;
}

int ultrasonicSensorData() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);

  long distance = duration / 58.0 //formula: uS / 58 = centimeters

  return distance;
}
////////////////////////////////////////////////////////

//FUNCTIONS FOR DRIVING THE CAR 
void stopMotors() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
}

void driveForward() {
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
}

void driveBackward() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
}

void turnLeft() {
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
}

void turnRight() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
}

void topRight() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
}

void topLeft() {
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
}

void backRight() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
}

void backLeft() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
}
//////////////////////////////////////////////


void setup() {

  Serial.begin(115200);
  
  //put ESP32 in WiFi mode
  wifi_setup();

  //configure this ESP32 to receive RemoteData
  set_data_receiver(&data);

  //set pin modes
  pinMode(EN12, OUTPUT);
  pinMode(EN34, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  // Enable motors
  digitalWrite(EN12, HIGH);
  digitalWrite(EN34, HIGH);
  delay(300);
  calibrateJoystick();
}


void loop() {

  int nx = normalizeAxis(data.vx, CENTER_X);
  int ny = normalizeAxis(data.vy, CENTER_Y);

  Serial.print("X Value: ");
  Serial.println(nx);
  Serial.print("Y Value: ");
  Serial.println(ny);

  
// 3x3 direction mapping
if (nx == 0 && ny == 0) {
  stopMotors();
} else if (nx == 0 && ny == 1) {
  driveForward();
} else if (nx == 0 && ny == -1) {
  driveBackward();
} else if (nx == -1 && ny == 0) {
  turnLeft();
} else if (nx == 1 && ny == 0) {
  turnRight();
} else if (nx == -1 && ny == 1) {
  topLeft();
} else if (nx == 1 && ny == 1) {
  topRight();
} else if (nx == -1 && ny == -1) {
  backLeft();
} else if (nx == 1 && ny == -1) {
  backRight();
}

  delay(70);

}


