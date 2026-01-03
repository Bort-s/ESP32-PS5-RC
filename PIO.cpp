//Code for Platform IO

#include <Arduino.h>
#include <ps5Controller.h>

//Leds for seeing connection
const int led_con = 16;
const int led_acon = 17;

bool connected = false;

const char MAC[] = "XX:XX:XX:XX:XX:XX"; //Replace with your MAC address

// Pins for TBB6612FNG
const int AIN1 = 33;
const int AIN2 = 25;
const int PWMA = 32;
const int BIN1 = 26;
const int BIN2 = 27;
const int PWMB = 14;


const int CH_A = 0;
const int CH_B = 1;

const int max_speed = 255;
int valL2 = 0;
int valR2 = 0;
bool L2 = false;     
bool R2 = false;

int xAxis = 0;
int16_t lxAxis = 0;
int16_t rxAxis = 0;

const int deadzone = 20; //Joystick's deadzone
const int delaytime = 150; // Dead-time between direction changes (µs)


void setup() 
{
  Serial.begin(115200);
  ps5.begin(MAC);

  pinMode(led_con, OUTPUT);
  pinMode(led_acon, OUTPUT);
  digitalWrite(led_acon, HIGH);
  digitalWrite(led_con, LOW);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  ledcSetup(CH_A, 20000, 8);
  ledcSetup(CH_B, 20000, 8);

  ledcAttachPin(PWMA, CH_A);
  ledcAttachPin(PWMB, CH_B);

}

void forward(int speed) 
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(CH_A, speed);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(CH_B, speed);
}

void backward(int speed) 
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(CH_A, speed);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(CH_B, speed);
}

void right(int speed) 
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(CH_A, speed);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(CH_B, speed);
}

void left(int speed) 
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(CH_A, speed);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(CH_B, speed);
}

void stop() 
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  ledcWrite(CH_A, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  ledcWrite(CH_B, 0);
}

void loop() 
{
  connected = ps5.isConnected();

  valL2 = ps5.L2Value();
  valR2 = ps5.R2Value();
  L2 = ps5.L2();
  R2 = ps5.R2();

  xAxis = ps5.data.analog.stick.lx;

  // Check connection
  if (connected == false)
  {
    stop();
    digitalWrite(led_acon, HIGH);
    digitalWrite(led_con, LOW);
    do
    {
      connected = ps5.isConnected();
      if (connected == true) 
      {
        digitalWrite(led_con, HIGH);
        digitalWrite(led_acon, LOW);
        connected = true;  
      } 
    } 
    while (connected == false);
  }


  //Drive control
  if (R2 == true && L2 == false)
  {
    // Forward
    do
    {
      R2 = ps5.R2();
      valR2 = ps5.R2Value();
      forward(valR2);
      printf("Forward Speed: %d\n", valR2);
    } while (R2);
    delay(delaytime);
  } 
  else if (R2 == false && L2 == true) 
  {
    // Backward
    do
    {
      L2 = ps5.L2();
      valL2 = ps5.L2Value();
      backward(valL2);
      printf("Backward Speed: %d\n", valL2);
    } while (L2);
    delay(delaytime);
  }
  else if(xAxis < -deadzone)
  {
    do
    {
      xAxis = ps5.data.analog.stick.lx;
      lxAxis = map(xAxis, 0, -128, 0, 255);
      left(lxAxis);
      printf("Left Speed: %d\n", lxAxis);
    }
    while(xAxis < -deadzone);
    delay(delaytime);
  }
  else if(xAxis > deadzone)
  {
    do
    {
      xAxis = ps5.data.analog.stick.lx;
      rxAxis = map(xAxis, 0, 127, 0, 255);
      right(rxAxis);
      printf("Right Speed: %d\n", rxAxis);
    }
    while(xAxis > deadzone);
    delay(delaytime);
  }
  else
  {
    stop();
  }
}
