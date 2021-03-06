﻿/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
/*End of auto generated code by Atmel studio */

void receiveEvent(int howMany) ;

Servo cctvPitchServo;  // create servo object to control a servo
Servo cctvYawServo;  // create servo object to control a servo


char buf [64];
volatile byte pos;
volatile boolean process_it;

volatile int servo_speed = 1000;
volatile int servo_pitch_pos = 60;
volatile int servo_yaw_pos = 90;
volatile int servo_pitch_pos_last = servo_pitch_pos;
volatile int servo_yaw_pos_last = servo_yaw_pos;
volatile bool servo_pos_update_flag = 0;

// main loop - wait for flag set in interrupt routine
unsigned long cctv_pitch_timer;
long unsigned  cctv_yaw_timer;

unsigned long beep_timer;
volatile bool beep_request = 0;
volatile bool beep_state = 0;
volatile int beep_times = 3;
volatile int beep_rate_ms = 200;

volatile bool led1state = 0;
volatile bool led2state = 0;
volatile bool led3state = 0;
volatile bool ledstateupdate_flag = 0;

void *command_handler(char **args,int arg_count){

	if(!strcmp(args[0], "cctv") ) {
		if(!strcmp(args[1], "pitch")) {
			servo_pitch_pos = atoi(args[2]);
			servo_pos_update_flag = 1;
			//cctvPitchServo.write(servo_pitch_pos);
			//cctvPitchServo.write(atoi(args[2]));
		}
		else if(!strcmp(args[1], "yaw")) {
			servo_yaw_pos = atoi(args[2]);
			servo_pos_update_flag = 1;
			//cctvYawServo.write(servo_yaw_pos);
			//cctvYawServo.write(atoi(args[2]));
		}
		else if(!strcmp(args[1], "speed")) {
			servo_speed = (atoi(args[2]));
		}
	}
	else if(!strcmp(args[0], "beep")) {
		if(!strcmp(args[1], "set_rate")) {
			beep_rate_ms = (atoi(args[2]));
		}
		else if(!strcmp(args[1], "now")) {
			beep_times = (atoi(args[2]));
			if(beep_times >0){
				beep_request = 1;
			}
			//beep_rate_ms = (atoi(args[2]));
		}
	}
	else if(!strcmp(args[0], "led")) {
		if(!strcmp(args[1], "1")) {
			led1state = atoi(args[2]);
			ledstateupdate_flag = 1;
		}
		else if(!strcmp(args[1], "2")) {
			led2state = atoi(args[2]);
			ledstateupdate_flag = 1;
		}
		else if(!strcmp(args[1], "3")) {
			led3state = atoi(args[2]);
			ledstateupdate_flag = 1;
		}
		
	}
}

void command_parse_execute(char *command){

	asm("nop");
	int arg_index = 0;
	char *pch;
	char *remotch_args[ 10];
	pch = strtok(command, " ,");
	while(pch != NULL) {
		remotch_args[arg_index] = pch;
		arg_index++;
		if(arg_index >=10) break;
		pch = strtok (NULL, " ,");
	}
	command_handler(remotch_args,arg_index);
}

//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio

void setup() {
	
	cctvPitchServo.attach(5);  // attaches the servo on pin 9 to the servo object
	cctvYawServo.attach(6);

	cctvPitchServo.write(servo_pitch_pos);
	cctvYawServo.write(servo_yaw_pos);
	
	TCCR2B = /*_BV(CS22)| */_BV(CS21) | _BV(CS20);
	OCR2A = 63;
	OCR2B = 0;

	pinMode(3, OUTPUT);
		
	pinMode(7,OUTPUT);
	pinMode(8,OUTPUT);
	pinMode(9,OUTPUT);
		
	digitalWrite(7,LOW);
	digitalWrite(8,LOW);
	digitalWrite(9,LOW);
		
		
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop() {
	if(ledstateupdate_flag){
		if(led1state) digitalWrite(7,HIGH);
		else digitalWrite(7,LOW);
		
		if(led2state) digitalWrite(8,HIGH);
		else digitalWrite(8,LOW);
		
		if(led3state) digitalWrite(9,HIGH);
		else digitalWrite(9,LOW);
		
		ledstateupdate_flag = 0;
	}
	
	
	
	
	if( (beep_request==1) && ( millis() - beep_timer) > beep_rate_ms ){
		beep_state = !beep_state;
		
		if(beep_state) {
			analogWrite(3, 127);
		}
		else {
			analogWrite(3, 0);
			beep_times--;
		}
		
		if(beep_times==0){
			beep_request = 0;
		}
		
		beep_timer = millis();
		//beep_request = 0;
	}
	
	
	if (process_it)
	{
		buf [pos] = 0;
		pos = 0;
		Serial.println (buf);
		command_parse_execute(buf);
		
		process_it = false;
	}  // end of flag set
	

	
	if((servo_pitch_pos_last > servo_pitch_pos) && (millis() - cctv_pitch_timer > (1000-servo_speed)) ){
		servo_pitch_pos_last--;
		cctv_pitch_timer = millis();
	}
	
	if((servo_pitch_pos_last < servo_pitch_pos) && (millis() - cctv_pitch_timer > (1000-servo_speed)) ){
		servo_pitch_pos_last++;
		cctv_pitch_timer = millis();
	}
	
	if((servo_yaw_pos_last > servo_yaw_pos) && (millis() - cctv_yaw_timer > (1000-servo_speed)) ){
		servo_yaw_pos_last--;
		cctv_yaw_timer = millis();
	}
	
	if((servo_yaw_pos_last < servo_yaw_pos) && (millis() - cctv_yaw_timer > (1000-servo_speed)) ){
		servo_yaw_pos_last++;
		cctv_yaw_timer = millis();
	}

	if(servo_pos_update_flag){
		cctvPitchServo.write(servo_pitch_pos_last);
		cctvYawServo.write(servo_yaw_pos_last);
		//servo_pos_update_flag = 0;
	}

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int c = Wire.read();    // receive byte as an integer
   if (pos < sizeof(buf))
   {
	   buf [pos++] = c;
	   
	   // example: newline means time to process buffer
	   if (c == '\n')
	   process_it = true;
	   
   }  // end of room available
  Serial.println(c);         // print the integer
}