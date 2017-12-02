
/* 
The circuit:
* Left motors on Pin 5 and 6
* Right motors on Pin 9 and 10
* Forward led on Pin 3 with 220 Ohm resistor in series
* Reverse led on Pin 4 with 220 ohm resistor in series
* LM35 connected to 5V, Pin A0 and Gnd
* JY-MCU Bluetooth Wireless Serial Port Module (slave) connected as follows:
    VCC <--> 5V
    GND <--> GND
    TXD <--> Pin 0 (Rx)
    RXD <--> Pin 1 (Tx)

* Gas Sensor connected to 5V, Pin A1 and Gnd
* IR sensor on Pin 2
* Ultrasonic on 5V , trigger on Pin 11, echo pin on Pin 12, Gnd

*/


// Serial Parameters: COM11 9600 8 N 1 
// \r or \n to end command line
// Bluetooth is on Pin 0 & 1 @ 9600 speed

// Command structure
// CMD RED|GREEN|YELLOW=ON|OFF
// CMD TMAX|SECONDS=value
// CMD SECONDS=value
// CMD STATUS

// Status message structure
// STATUS RED|GREEN|YELLOW|TMIN|TMAX|SECONDS|TEMP|THIGH=value

#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

float maxTemp = 30.0; // switch on led when temp > maxTemp
int maxTempSensor = (int) ((maxTemp / 100 + .5) * 204.8);
float temperature = 0.0;
const int gasPin = A1;

int maxSeconds = 10; // send status message every maxSeconds

const int ledPin = 13;   // temperature led
const int tempPin = A0;  // LM35 temperature sensor analog input pin

const int led1Pin = 3; // forward
const int led2Pin = 4; // reverse
const int lft1 = 5;
const int lft2 = 6;
const int rgt1 = 9;
const int rgt2 = 10;
int sensor = 2;
int sensor_read;

volatile int tempVal;
volatile int seconds = 0;
volatile boolean tempHigh = false;
volatile boolean statusReport = false;

String inputString = "";
String command = "";
String value = "";
boolean stringComplete = false;

void setup(){
  //start serial connection
  Serial.begin(9600);
  Serial.print("Max T: ");
  Serial.print(maxTemp);
  Serial.print(" Sensor: ");
  Serial.println(maxTempSensor);

  inputString.reserve(50);
  command.reserve(50);
  value.reserve(50);
  
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW);
  
  pinMode(led1Pin, OUTPUT); 
  pinMode(led2Pin, OUTPUT); 
  pinMode(lft1, OUTPUT); 
  pinMode(lft2, OUTPUT);
  pinMode(rgt1, OUTPUT); 
  pinMode(rgt2, OUTPUT);
  pinMode(sensor, INPUT);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(lft1, LOW);
  digitalWrite(lft2, LOW);
  digitalWrite(rgt1, LOW);
  digitalWrite(rgt2, LOW);


  cli();          // disable global interrupts
  
  // initialize Timer1 for interrupt @ 1000 msec
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  
  sei();          // enable global interrupts
}

// timer interrupt routine
ISR(TIMER1_COMPA_vect)
{
  tempVal = analogRead(tempPin);
  
  if (tempVal > maxTempSensor) {
    digitalWrite(ledPin, HIGH);
    tempHigh = true;
  }
  else {
    digitalWrite(ledPin, LOW);
    tempHigh = false;
  }

  if (seconds++ >= maxSeconds) {
    statusReport = true;
    seconds = 0;
  }
}

void forward(){
  int a;
  a=digitalRead(2);
  if (a==HIGH)
  {
  digitalWrite(lft1, HIGH); digitalWrite(lft2, LOW); digitalWrite(rgt1, HIGH); digitalWrite(rgt2, LOW);}
  else
  {
  digitalWrite(lft1, LOW); digitalWrite(lft2, LOW); digitalWrite(rgt1, LOW); digitalWrite(rgt2, LOW);}
}

void stop(){
  digitalWrite(lft1, LOW); digitalWrite(lft2, LOW); digitalWrite(rgt1, LOW); digitalWrite(rgt2, LOW);
}
 void reverse() {
   digitalWrite(lft1, LOW); digitalWrite(lft2, HIGH); digitalWrite(rgt1, LOW); digitalWrite(rgt2, HIGH);
 }
void right() {
    digitalWrite(lft1, HIGH); digitalWrite(lft2, LOW); digitalWrite(rgt1, LOW); digitalWrite(rgt2, HIGH);
}
void left() {
  digitalWrite(lft1, LOW); digitalWrite(lft2, HIGH); digitalWrite(rgt1, HIGH); digitalWrite(rgt2, LOW);
}
  
  
// interpret and execute command when received
// then report status if flag raised by timer interrupt
void loop(){
  int intValue = 0;
  float mv = (tempVal/1024.0)*5000; 
  float cel = mv/10;
  if (stringComplete) {
    Serial.println(inputString);
    boolean stringOK = false;
    if (inputString.startsWith("CMD ")) {
      inputString = inputString.substring(4);
      int pos = inputString.indexOf('=');
      if (pos > -1) {
        command = inputString.substring(0, pos);
        value = inputString.substring(pos+1, inputString.length()-1);  // extract command up to \n exluded
        //Serial.println(command);
        //Serial.println(value);
        if (command.equals("MAGENTA")) { // FORWARD=ON|OFF
          value.equals("ON") ? digitalWrite(led2Pin, HIGH) : digitalWrite(led2Pin, LOW);
          stringOK = true;
        }
        else if (command.equals("PINK")) { // REVERSE=ON|OFF
          value.equals("ON") ? digitalWrite(led1Pin, HIGH) : digitalWrite(led1Pin, LOW);
          stringOK = true;
        }
        else if (command.equals("RED")) { // forward is Red
          value.equals("ON") ? forward() : stop();
          stringOK = true;
        }
        else if (command.equals("GREEN")) { // reverse is Green
          value.equals("ON") ? reverse() : stop();
          stringOK = true;
        }
        else if (command.equals("CYAN")) { // right is cyan
          value.equals("ON") ? right() : stop();
          stringOK = true;
        }
        else if (command.equals("YELLOW")) { // left is yellow
          value.equals("ON") ? left() : stop();
          stringOK = true;
        }
        else if (command.equals("SECONDS")) { // SECONDS=value
          intValue = value.toInt();
          if (intValue > 0) {
            maxSeconds = intValue;
            stringOK = true;
          }
        }
      } // pos > -1
      else if (inputString.startsWith("STATUS")) {
        Serial.print("STATUS Forward_Led=");
        Serial.println(digitalRead(led1Pin));
        Serial.print("STATUS Reverse_Led=");
        Serial.println(digitalRead(led2Pin));
        Serial.print("STATUS Left Motor =");
        Serial.println(digitalRead(lft1));
        Serial.println(digitalRead(lft2));
        Serial.print("STATUS Right Motor =");
        Serial.println(digitalRead(rgt1));
        Serial.println(digitalRead(rgt2));
        Serial.print("STATUS SECONDS=");
        Serial.println(maxSeconds);
        Serial.print("TEMPRATURE = ");
        Serial.print(cel);
        Serial.print("*C");
        Serial.println();
        Serial.print("GAS SENSOR = ");
        Serial.println(analogRead(gasPin));
        Serial.print("Ping: ");
        Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
        Serial.println("cm");
        stringOK = true;
      } // inputString.startsWith("STATUS")
    } // inputString.startsWith("CMD ")
    stringOK ? Serial.println("Command Executed") : Serial.println("Invalid Command");
    // clear the string for next iteration
    inputString = "";
    stringComplete = false;
  } // stringComplete
  
  if (statusReport) {
    float mv = (tempVal/1024.0)*5000; 
    float cel = mv/10;;
    Serial.print("TEMPERATURE = ");
    Serial.print(cel);
    Serial.print("*C");
    Serial.println();
    Serial.print("GAS SENSOR = ");
    Serial.println(analogRead(gasPin));
    Serial.print("Ping: ");
    Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
    statusReport = false;
  }

}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    //Serial.write(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline or a carriage return, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    } 
  }
}


