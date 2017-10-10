/*
  RC Goliath Arduino Mod code, project by Rafal Boguszewski.
  Main project URL: http://wp.me/p91MV0-2C

  Library doing all the Nunchuk lifting:
  Copyright 2011-2013 Gabriel Bianconi, http://www.gabrielbianconi.com/

  Nunchuk project URL: http://www.gabrielbianconi.com/projects/arduinonunchuk/

*/


#define ArrayLenght(x) (sizeof(x) / sizeof(x[0]))

#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 19200

#define ACCELERATIONPIN 10 
#define IN1A 12
#define IN1B 11

#define STEERINGPIN 3
#define IN2A 4
#define IN2B 2

#define BUZZERPIN 7
#define LIGHTSPIN 8

#define FWRDDEADZONE 150
#define BACKDEADZONE 80

#define INITFREQ 800
#define HORNFREQ 600
#define INITLENGHT 500


byte acceleration;
byte steering;

byte lastCButtonState = 0;
byte lastZButtonState = 0;

bool isForward = false;
bool isReversing = false;

bool goingLeft = false;
bool goingRight = false;


ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{

 //Serial.begin(BAUDRATE);

 
  byte digitalPins[] = {IN1A, IN1B, IN2A, IN2B, BUZZERPIN, LIGHTSPIN, STEERINGPIN};
  byte analogPins[] = {ACCELERATIONPIN};
 
  for (int i = 0; i < ArrayLenght(analogPins); i++)
  {
    pinMode(analogPins[i], OUTPUT);
    analogWrite(analogPins[i], 0);
  }

  for (int i = 0; i < ArrayLenght(digitalPins); i++)
  {
    pinMode(digitalPins[i], OUTPUT);
    digitalWrite(digitalPins[i], LOW);
  }

  nunchuk.init();
  nunchuk.update();

  while (!(nunchuk.zButton == 1 && nunchuk.cButton == 1))
  {
    nunchuk.update();
    delay(250);
  }

  carInitialized();
  delay(1500);
}

void loop()
{

  nunchuk.update();

  //  Serial.print(nunchuk.analogX, DEC);
  //  Serial.print(' ');
  //  Serial.print(nunchuk.analogY, DEC);
  //  Serial.print(' ');
  //  Serial.print(nunchuk.accelX, DEC);
  //  Serial.print(' ');
  //  Serial.print(nunchuk.accelY, DEC);
  //  Serial.print(' ');
  //  Serial.print(nunchuk.accelZ, DEC);
  //  Serial.print(' ');
  //  Serial.print(nunchuk.zButton, DEC);
  //  Serial.print(' ');
  //  Serial.println(nunchuk.cButton, DEC);

  accelerationMotorControl();
  steeringMotorControl();

  policeLights();
  buzzer();

}

void accelerationMotorControl ()
{
  /* Y deadzone 110 - 140,
     if Y is between 140-255, the car goes forward at acceleration = PWM,
     if Y is between 110-0, the car goes backwards at acceleration = PWM,
  */

  if (nunchuk.analogY > FWRDDEADZONE)
  {
    switch (nunchuk.analogY)
    {
      case 151 ... 180:
        acceleration = map(nunchuk.analogY, 150, 180, 160, 200);
        break;

      case 181 ... 256:
        acceleration = 255;
        break;
    }


    if (!isForward)
    {
      isForward = true;
      digitalWrite(IN1A, HIGH);
      digitalWrite(IN1B, LOW);
    }

    analogWrite(ACCELERATIONPIN, acceleration);

  }
  
  else if (nunchuk.analogY < BACKDEADZONE)
  {

    acceleration = map(nunchuk.analogY, 79, 0, 125, 255);

    if (!isReversing)
    {
      isReversing = true;
      digitalWrite(IN1A, LOW);
      digitalWrite(IN1B, HIGH);
    }

    analogWrite(ACCELERATIONPIN, acceleration);
  }

  else
  {
    isForward = false;
    isReversing = false;
    analogWrite(ACCELERATIONPIN, 0);
    digitalWrite(IN1A, LOW);
    digitalWrite(IN1B, LOW);
  }
}

void steeringMotorControl()
{

  switch (nunchuk.analogX)
  {
    case 0 ... 80:

      if (!goingLeft)
      {
        digitalWrite(IN2A, HIGH);
        digitalWrite(IN2B, LOW);
        digitalWrite(STEERINGPIN, HIGH);
        goingLeft = true;
      }
      break;

    case 170 ... 255:

      if (!goingRight)
      {
        digitalWrite(IN2A, LOW);
        digitalWrite(IN2B, HIGH);
        digitalWrite(STEERINGPIN, HIGH);
        goingRight = true;
      }
      break;

    default:
      digitalWrite(IN2A, LOW);
      digitalWrite(IN2B, LOW);
      digitalWrite(STEERINGPIN, LOW);
      goingLeft = false;
      goingRight = false;
      break;
  }
}

void policeLights()
{

  if (nunchuk.cButton != lastCButtonState)
  {
    if (nunchuk.cButton == 1)
    {
      digitalWrite(LIGHTSPIN, HIGH);
    }
    else
    {
      digitalWrite(LIGHTSPIN, LOW);
    }

  }

  lastCButtonState = nunchuk.cButton;

}

void buzzer()
{

  if (nunchuk.zButton != lastZButtonState)
  {
    if (nunchuk.zButton == 1)
    {
      tone(BUZZERPIN, HORNFREQ);
    }
    else
    {
      noTone(BUZZERPIN);
    }

  }

  lastZButtonState = nunchuk.zButton;

}

void carInitialized()
{
  tone(BUZZERPIN, INITFREQ, INITLENGHT);
}



