#include <SoftwareSerial.h>
SoftwareSerial ArduinoUno(13,12);

#include <NewPing.h>
#include <Servo.h>

#define trigger_pin A0
#define echo_pin A1
#define max_distance 200

int motor1_1 = 9;
int motor1_2 = 8;
int motor2_1 = 7;
int motor2_2 = 6;
int motor1_en = 10;
int motor2_en = 5;

NewPing sonar(trigger_pin, echo_pin, max_distance);
Servo myservo;

boolean moving_forward = false;
int distance = 100;
void setup(){
	pinMode(motor1_1, OUTPUT);
  pinMode(motor1_2, OUTPUT);
  pinMode(motor2_1, OUTPUT);
  pinMode(motor2_2, OUTPUT);
  pinMode(motor1_en, OUTPUT);
  pinMode(motor2_en, OUTPUT);
  myservo.attach(11);
  myservo.write(90);
  delay(2000);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
	Serial.begin(9600);
	ArduinoUno.begin(4800);

}




int val;
void loop(){
	
	while(ArduinoUno.available()>0){
	val = ArduinoUno.parseFloat();
	if(ArduinoUno.read()== '\n'){
	Serial.println(val);
	}
}
if (val == -1){
   int right_distance = 0;
  int left_distance =  0;
  distance = readPing();
  Serial.println(distance);
  analogWrite(10, 255);
  analogWrite(5, 255);

  delay(40);

  if (distance <= 20)
  {
    move_stop();
    delay(100);
    move_backward();
    delay(300);
    move_stop();
    delay(200);
    right_distance = look_right();
    delay(200);
    left_distance = look_left();
    delay(200);

    if (right_distance >= left_distance)
    {
      turn_right();
      move_stop();
    } else
    {
      turn_left();
      move_stop();
    }
  } else
  {
    move_forward();
  }
}
if (val == 0){
  move_forward();

}
if (val == 1){
  turn_right_45();

}
if (val == 2){
  turn_left_45();

}
delay(30);
}




int look_right()
{
  myservo.write(40);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(90);
  return distance;
}

int look_left()
{
  myservo.write(180);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(90);
  return distance;
  delay(100);
}

int readPing() {
  delay(50);
  int dist_cm = sonar.ping_cm();
  if (dist_cm == 0)
  {
    dist_cm = 250;
  }
  return dist_cm;
}

void move_stop() {
  digitalWrite(motor1_1, LOW);
  digitalWrite(motor1_2, LOW);
  digitalWrite(motor2_1, LOW);
  digitalWrite(motor2_2, LOW);
}

void move_forward() {
  if (!moving_forward)
  {
    moving_forward = true;
    digitalWrite(motor1_1, HIGH);
    digitalWrite(motor1_2, LOW);
    digitalWrite(motor2_1, HIGH);
    digitalWrite(motor2_2, LOW);
  }
}

void move_backward() {
  moving_forward = false;
  digitalWrite(motor1_1, LOW);
  digitalWrite(motor1_2, HIGH);
  digitalWrite(motor2_1, LOW);
  digitalWrite(motor2_2, HIGH);
}

void turn_right() {
  digitalWrite(motor1_1, LOW);
  digitalWrite(motor1_2, HIGH);
  digitalWrite(motor2_1, HIGH);
  digitalWrite(motor2_2, LOW);
  delay(1000);
  digitalWrite(motor1_1, HIGH);
  digitalWrite(motor1_2, LOW);
  digitalWrite(motor2_1, HIGH);
  digitalWrite(motor2_2, LOW);
}

void turn_left() {
  digitalWrite(motor1_1, HIGH); 
  digitalWrite(motor1_2, LOW);
  digitalWrite(motor2_1, LOW);
  digitalWrite(motor2_2, HIGH);
  delay(1000);
  digitalWrite(motor1_1, HIGH);
  digitalWrite(motor1_2, LOW);
  digitalWrite(motor2_1, HIGH);
  digitalWrite(motor2_2, LOW);
}

void turn_right_45(){
  turn_right();
  // appropriate delay determind by calibration -- to steer the car 45
  move_forward();
  // appropriate delay determind by calibration -- to move a little forward in the corner
  turn_right();
  // delay
  move_forward();
  // delay
  move_stop();
  // appropriate delay -- wait for esp signal to confirm that you should move forward
  val = 0; 
}
void turn_left_45(){
  turn_left();
  // appropriate delay determind by calibration
  move_forward();
  // appropriate delay determind by calibration -- to move a little forward in the corner
  turn_left();
  // delay
  move_forward();
  // delay
  move_stop();
  // appropriate delay -- wait for esp signal to confirm that you should move forward
  val = 0;
}