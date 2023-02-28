#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define CMPS12_ADDRESS 0x60 // Address of CMPS12 shifted right one bit for arduino wire library
#define ANGLE_8 0x01        // Register to read 8bit angle from

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0; // variable to store the servo position

unsigned char high_byte, low_byte, angle8;
char pitch, roll;
unsigned int angle16;
int SERVO = 4;
const int offset = 3;
float erreur = 0;
float SP = 150;
float sortie = 0;
//float Kp = (1700 - 1300)/360;
float Kp = 0.1;
float K = 1500;

int convertionDegree(float degree)
{
  // convertir un angle en degré en angle en microsecondes
  int tempsHaut = 0;
  tempsHaut = degree * 11 + 1500;
  return tempsHaut;
}

// void convertionVitesse(float vitesse)
// {
//   if (vitesse > 0)
//   {
//     digitalWrite(SERVO, HIGH); // sets the LED on (5V on pin 13)
//     delayMicroseconds(1450);
//     digitalWrite(SERVO, LOW); // sets the LED off (0V on pin 13)
//     delay(20);
//   }
//   else if (vitesse < 0)
//   {
//     digitalWrite(SERVO, HIGH); // sets the LED on (5V on pin 13)
//     delayMicroseconds(1550);
//     digitalWrite(SERVO, LOW); // sets the LED off (0V on pin 13)
//     delay(20);
//   }
//   else
//   {
//     digitalWrite(SERVO, HIGH); // sets the LED on (5V on pin 13)
//     delayMicroseconds(1500);
//     digitalWrite(SERVO, LOW); // sets the LED off (0V on pin 13)
//     delay(20);
//   }
// }

void convertionVitesse(float vitesse)
{
  sortie = Kp * vitesse + K;
  digitalWrite(SERVO, HIGH); // sets the LED on (5V on pin 13)
  delayMicroseconds(sortie);
  digitalWrite(SERVO, LOW); // sets the LED off (0V on pin 13)
  delay(20);
}

void setup()
{
  Serial.begin(9600); // Start serial port
  Wire.begin();       // Start I2C
  // myservo.attach(4);  // attaches the servo on pin 9 to the servo object
  pinMode(SERVO, OUTPUT);
}

void loop()
{

  // myservo.write(90 + offset);             // vitesse de rotation du servo nulle
  // myservo.write(0 + offset);              // vitesse de rotation du servo CW max
  // myservo.write(180 + offset);            // vitesse de rotation du servo CCW max
  // myservo.write(10 + offset);             // vitesse de rotation du servo CW autre
  // myservo.write(135 + offset);            // vitesse de rotation du servo CCW autre
  Wire.beginTransmission(CMPS12_ADDRESS); // starts communication with CMPS12
  Wire.write(ANGLE_8);                    // Sends the register we wish to start reading from

  Wire.endTransmission();

  // Request 5 bytes from the CMPS12
  // this will give us the 8 bit bearing,
  // both bytes of the 16 bit bearing, pitch and roll
  Wire.requestFrom(CMPS12_ADDRESS, 5);

  while (Wire.available() < 5)
    ; // Wait for all bytes to come back

  angle8 = Wire.read(); // Read back the 5 bytes
  high_byte = Wire.read();
  low_byte = Wire.read();
  pitch = Wire.read();
  roll = Wire.read();

  angle16 = high_byte; // Calculate 16 bit angle
  angle16 <<= 8;
  angle16 += low_byte;

  erreur = SP - (angle16 / 10.0);
  //sortie = erreur * K;
  convertionVitesse(erreur);
  Serial.print(erreur);
  Serial.print("\t");
  Serial.println(sortie);
  // Serial.println(angle16 / 10.0);
  // Serial.print("roll: ");               // Display roll data
  // Serial.print(roll, DEC);

  // Serial.print("    pitch: ");          // Display pitch data
  // Serial.print(pitch, DEC);

  // Serial.print("angle full: "); // Display 16 bit angle with decimal place
  // Serial.print(angle16 / 10, DEC);
  // Serial.print(".");
  // Serial.println(angle16 % 10, DEC);

  // Serial.print("    angle 8: "); // Display 8bit angle
  // Serial.println(angle8, DEC);
}