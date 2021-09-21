// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _LucanusCervus_H_
#define _LucanusCervus_H_
#include "Arduino.h"
//add your includes for the project LucanusCervus here
// Do not remove the include below
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
//#define SETSERIAL
#include <DabbleESP32.h>
#include <M5StickCPlus.h>
#include <Wire.h>

//add your includes for the project PtLINO_Rover here

const unsigned long eventInterval = 1000;
unsigned long previousTimeBateria = 0;
unsigned long previousTimeTriangulo = 0;
unsigned long previousTimeStart = 0;
unsigned long previousTimeX = 0;
unsigned long previousTimeSelect = 0;
unsigned long previousTimeSerial = 0;
unsigned long previousTimeSquare = 0;
unsigned long previousTimeCircle = 0;

unsigned long currentTime = millis();

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
char *ssid = "LucanusCervus";
char SSID[32];

// I2C address of the RoverC Hat
#define ROVERC_I2C_Address 0x38

#define MIN_GARRA 5
#define MAX_GARRA 75
int8_t garraAngulo = MIN_GARRA;
bool garraAberta = false;

//String serial
String strSerial = "";


// container for the actual motor speed
int8_t motor_speeds[4] = { 0, 0, 0, 0 };
// motor numbers:
// 1 ------ 2
//   |    |
//   |    |
//   | M5 |
// 3 ------ 4


void go(int angle, int speed);
int8_t speed_ramp(int8_t actual_speed, int8_t target_speed, int8_t freio);
void rover_set_motors(int8_t M1, int8_t M2, int8_t M3, int8_t M4, int8_t freio);
void rover_stop();
void move_rover(double angle, int speed);

//add your function definitions for the project PtLINO_Rover here

#define LEDPIN 10

void garra(int angulo);

bool temAlgumComandoDeMotorPresionado();


void showBateriaStatus();
void SetChargingCurrent(uint8_t CurrentLevel) {
	Wire1.beginTransmission(0x34);
	Wire1.write(0x33);
	Wire1.write(0xC0 | (CurrentLevel & 0x0f));
	Wire1.endTransmission();
}

double getBatteryLevel(void) {
	uint16_t vbatData = M5.Axp.GetVbatData();
	double vbat = vbatData * 1.1 / 1000;
	uint16_t vapsData = M5.Axp.GetVapsData();
	double vaps = vapsData * 1.4 / 1000;
	return 100.0 * ((vaps - 3.0) / (4.07 - 3.0));
}
#endif /* _LucanusCervus_H_ */
