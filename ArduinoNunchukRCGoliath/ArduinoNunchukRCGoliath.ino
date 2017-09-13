/*

  Library doing all the Nunchuk lifting:
  Copyright 2011-2013 Gabriel Bianconi, http://www.gabrielbianconi.com/

  Nunchuk project URL: http://www.gabrielbianconi.com/projects/arduinonunchuk/


  Pin overview of the nunchuk attachment:
  + to 5v of the arduino
  - to GND of the arduino
  Data (d) to Analog pin 4 (A4)
  Clock (c) to Analog pin 5 (A5)
*/

#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 19200

#define ACCELERATIONPIN 10 //PWM for vertical forward motor contror 
#define IN1A 13
#define IN1B 12


#define STEERINGPIN 3
#define IN2A 4
#define IN2B 5

#define BUZZERPIN 7
#define LIGHTSPIN 8

#define FWRDDEADZONE 140
#define BACKDEADZONE 110

#define REVERSEFREQ 800
#define HORNFREQ 200
#define REVERSELENGHT 500

#define ArrayLenght(x) (sizeof(x) / sizeof(x[0]))

byte acceleration;
byte steering;
int reverseTimer = 0;

byte lastCButtonState = 0;
byte lastZButtonState = 0;

bool isForward = false;
bool isReversing = false;

bool goingLeft = false;
bool goingRight = false;


ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{

  byte digitalPins[] = {IN1A, IN1B, IN2A, IN2B, BUZZERPIN, LIGHTSPIN};
  byte analogPins[] = {ACCELERATIONPIN, STEERINGPIN};
  //Serial.begin(BAUDRATE);

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


  delay(5000);

  nunchuk.init();

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
      case 140 ... 180:
        acceleration = map(nunchuk.analogY, 140, 180, 160, 200);
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

    acceleration = map(nunchuk.analogY, 110, 0, 125, 255);

    if (!isReversing)
    {
      isReversing = true;
      digitalWrite(IN1A, LOW);
      digitalWrite(IN1B, HIGH);
    }

    analogWrite(ACCELERATIONPIN, acceleration);

    if (reverseTimer % 90 == 1)
    {
      reverseTone();
    }

    reverseTimer++;
  }

  else
  {
    isForward = false;
    isReversing = false;
    analogWrite(ACCELERATIONPIN, 0);
    digitalWrite(IN1A, LOW);
    digitalWrite(IN1B, LOW);
    reverseTimer = 0;
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

void reverseTone()
{
  tone(BUZZERPIN, REVERSEFREQ, REVERSELENGHT);
}


