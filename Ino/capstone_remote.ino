#include <WiFiConfig.h>
//Find the mac address of your receiver ESP32 first, then enter it in here:
uint8_t receiverAddress[] = { 0x34, 0xB7, 0xDA, 0xF6, 0x3C, 0xF4 };

RemoteData data;  // global initialization of RemoteData

int deadzone = 25;
int CENTER_X, CENTER_Y;
const int CAL_SAMPLES = 100;

const int vrx = 0;
const int vry = 1;
const int sw = 3;

const int green_led = 10;
const int red_led = 8;

//normalize the axis to -1, 0, 1
int normalizeAxis(uint8_t val, uint8_t center) {
  if (val < center - deadzone) return -1;
  else if (val > center + deadzone) return 1;
  else return 0;
}

//Function to get a more stable reading (reading from joystick was weird, this helped it)
uint16_t readStable(int pin) {
  analogRead(pin);
  return analogRead(pin); // real value
}

//calibrate joystick to a new center everytime esp restarts due to inconsistent center readings
void calibrateJoystick() {
  long sx = 0, sy = 0;
  for (int i = 0; i < CAL_SAMPLES; ++i) {
    sx += analogRead(vrx);
    sy += analogRead(vry);
    delay(3);           
  }
  CENTER_X = sx / CAL_SAMPLES >> 4;
  CENTER_Y = sy / CAL_SAMPLES >> 4;
}


void setup() {
  Serial.begin(115200);         // Serial setup
  wifi_setup();                 // start wifi mode and ESP-NOW
  peer_setup(receiverAddress);  // configure receiving address

  pinMode(vrx, INPUT);
  pinMode(vry, INPUT);
  pinMode(sw, INPUT_PULLUP);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);

  calibrateJoystick();
}

void loop() {


  uint8_t rawX = readStable(vrx) >> 4;
  uint8_t rawY = readStable(vry) >> 4;

  //Raw data debug
  data.vx = rawX;
  data.vy = rawY;
  data.sw = analogRead(sw);
  Serial.print("VX:");
  Serial.println(data.vx);
  Serial.print("VY:");
  Serial.println(data.vy);
  ///////raw data debug/////

  int nx = normalizeAxis(data.vx, CENTER_X);
  int ny = normalizeAxis(data.vy, CENTER_Y);

  if(nx == 0 && ny == 0) {
  //red if not moving
  digitalWrite(red_led, HIGH);
  digitalWrite(green_led,LOW);
  } else {
  //green if moving
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);
  } 

  //send data to receiver
  send_data(receiverAddress, data);



  delay(20);
}