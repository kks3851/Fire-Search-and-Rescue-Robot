#include<Servo.h>
Servo myservo1;
Servo myservo2;
Servo myservo4;
Servo myservo5;
int pos1=90;
void setup() {
  // put your setup code here, to run once:
// myservo1.attach(2);
 myservo2.attach(3);
 myservo4.attach(4);
 myservo5.attach(5);
  
}
void initialize()
{
// myservo4.write(90); 
// delay(1000);
// myservo4.write(90);
// delay(100);
// myservo4.write(0);
 myservo2.write(90);
 delay(1000);
 myservo4.write(90);
 delay(1000);
 myservo5.write(45);
delay(1000);
}
/*
void right()
{
  pos1=pos1+10;
  myservo1.write(pos1);
}

void left()
{
  pos1=pos1-10;
  myservo1.write(pos1);
}

void halt()
{
  myservo1.write(90);
  myservo2.write(90);
}
*/
void pick()
{
  myservo2.write(60);
  delay(1000);
  myservo2.write(30);
  delay(1000);
  myservo2.write(0);
  delay(1000);
 /* myservo4.write(70);
  delay(1000);
  myservo4.write(90);
  delay(1000);
  myservo4.write(110);
  delay(1000);*/
  myservo5.write(60);
  delay(1000);
  myservo5.write(90);
  delay(1000);
}

void drop()
{
 
 myservo2.write(90);
 delay(1000); 
 myservo2.write(60);
 delay(1000); 
 myservo2.write(30);
 delay(1000); 
 myservo2.write(0);
 delay(1000); 
 myservo5.write(160);
 delay(1000);
 myservo5.write(180);
 delay(1000);
}

void carry()
{
// myservo1.write(90);
 myservo2.write(30);
 delay(1000);
 myservo2.write(60);
 delay(1000);
 myservo2.write(90);
 delay(1000);
 myservo2.write(135);
 delay(1000);
// myservo4.write(90);
// delay(1000);
}

void loop()
{
  initialize();
  delay(1000);
  pick();
  delay(3000);
  carry();
  delay(3000);
  drop();
  delay(2000);

}
