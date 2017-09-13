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


byte acceleration;
byte steering;
//int yRead = 0;
int reverseTimer = 0;

byte digitalPins = 6;
byte analogPins = 2;




ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{

  byte digitalPinsArr[6] = {IN1A, IN1B, IN2A, IN2B, BUZZERPIN, LIGHTSPIN};
  byte analogPinsArr[2] = {ACCELERATIONPIN, STEERINGPIN};
  //Serial.begin(BAUDRATE);

  for (int i = 0; i < analogPins; i++)
  {
    pinMode(analogPinsArr[i], OUTPUT);
    analogWrite(analogPinsArr[i], 0);
  }

  //  pinMode(ACCELERATIONPIN, OUTPUT);
  //  pinMode(STEERINGPIN, OUTPUT);

  for (int i = 0; i < digitalPins; i++)
  {
    pinMode(digitalPinsArr[i], OUTPUT);
    digitalWrite(digitalPinsArr[i], LOW);
  }

  //  pinMode(BUZZERPIN, OUTPUT);
  //  pinMode(LIGHTSPIN, OUTPUT);
  //  pinMode(IN1A, OUTPUT);
  //  pinMode(IN1B, OUTPUT);
  //  pinMode(IN2A, OUTPUT);
  //  pinMode(IN2B, OUTPUT);

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
    //yRead = nunchuk.analogY;

    //switch (yRead)
    //{
    // case 200 ... 255:
    //  acceleration = 255;
    // break;

    //case 140 ... 199:
    acceleration = map(nunchuk.analogY, 140, 255, 125, 255);
    //break;

    // }

    digitalWrite(IN1A, HIGH);
    digitalWrite(IN1B, LOW);
    analogWrite(ACCELERATIONPIN, acceleration);

  }
  else if (nunchuk.analogY < BACKDEADZONE)
  {

    acceleration = map(nunchuk.analogY, 110, 0, 125, 255);


    digitalWrite(IN1A, LOW);
    digitalWrite(IN1B, HIGH);
    analogWrite(ACCELERATIONPIN, acceleration);

    if (reverseTimer % 90 == 1)
    {
      reverseTone();
    }

    reverseTimer++;
  }

  else
  {
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
      digitalWrite(IN2A, HIGH);
      digitalWrite(IN2B, LOW);
      digitalWrite(STEERINGPIN, HIGH);
      break;

    case 170 ... 255:
      digitalWrite(IN2A, LOW);
      digitalWrite(IN2B, HIGH);
      digitalWrite(STEERINGPIN, HIGH);
      break;

    default:
      digitalWrite(IN2A, LOW);
      digitalWrite(IN2B, LOW);
      digitalWrite(STEERINGPIN, LOW);
      break;
  }
}


void policeLights()
{
  switch (nunchuk.zButton)
  {
    case 1:
      digitalWrite(LIGHTSPIN, HIGH);
      break;

    case 0:
      digitalWrite(LIGHTSPIN, LOW);
      break;

  }
}

void buzzer()
{
  switch (nunchuk.cButton)
  {
    case 1:
      tone(BUZZERPIN, HORNFREQ, 10);

    default:
      break;
  }

}

void reverseTone()
{
  tone(BUZZERPIN, REVERSEFREQ, REVERSELENGHT);
}


