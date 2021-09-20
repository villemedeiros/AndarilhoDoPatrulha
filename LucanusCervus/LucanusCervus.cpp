// Do not remove the include below
#include "LucanusCervus.h"

//The setup function is called once at startup of the sketch
void setup() {
	M5.begin();
	M5.update();
	Wire.begin(0, 26, 10000);

	uint64_t chipid = ESP.getEfuseMac();
	String str = ssid + String("_") + String((uint32_t) (chipid >> 32), HEX);
	char SSID[32];

	str.toCharArray(SSID, str.length() + 1);

	M5.Lcd.setRotation(1);
	M5.Lcd.setSwapBytes(false);
	Disbuff.createSprite(240, 135);
	Disbuff.setSwapBytes(true);
	Disbuff.fillRect(0, 0, 240, 20, Disbuff.color565(50, 50, 50));
	Disbuff.setTextSize(2);
	Disbuff.setTextColor(WHITE);
	Disbuff.setCursor(15, 35);
	Disbuff.print(str);
	Disbuff.pushSprite(0, 0);

	SetChargingCurrent(4);

#ifdef SETSERIAL
	// put your setup code here, to run once:
	Serial.begin(115200); // make sure your Serial Monitor is also set at this baud rate.
#endif

	Dabble.begin(SSID);

}

// The loop function is called in an endless loop
void loop() {

	currentTime = millis();

	//this function is used to refresh data obtained
	//from smartphone.Hence calling this function is
	//mandatory in order to get data properly from your mobile.
	Dabble.processInput();

	if (GamePad.isStartPressed()) {
		strSerial = "Start";
	}

	//INICIO CONTROLE DA GARRA
	if (GamePad.isTrianglePressed())
		if (currentTime - previousTimeTriangulo >= 20) {
			strSerial = "Triangle";
			garra(garraAngulo += 1);
			garraAberta = false;
			previousTimeTriangulo = currentTime;
		}

	if (GamePad.isCrossPressed())
		if (currentTime - previousTimeX >= 20) {
			strSerial = "Cross";
			garra(garraAngulo -= 1);
			garraAberta = true;
			previousTimeX = currentTime;

		}

	if (GamePad.isSelectPressed())
		if (currentTime - previousTimeSelect >= 300) {
			strSerial = "Select";

			if (garraAberta) {
				garra(MAX_GARRA);
			} else {
				garra(MIN_GARRA);
			}
			garraAberta = !garraAberta;

			/* Update the timing for the next time around */
			previousTimeSelect = currentTime;

		}
	//FIM CONTROLE DA GARRA

	//INICIO CONTROLE DOS MOTORES
	if (GamePad.getRadius() > 0) {
		//Gira a referencia dos angulos
		//para 0 frente 90 esquerda
		// 180 para traz e 270 direita
		int a = GamePad.getAngle();
		a = (a < 0 && a >= -90) ? 360 + a : a;
		go(a, GamePad.getRadius());
	} else if (!temAlgumComandoDeMotorPresionado()) {
		rover_stop();
	}

	if (GamePad.isSquarePressed()) {
		move_rover(270, 50);
	}

	if (GamePad.isCirclePressed()) {
		move_rover(90, 50);
	}
	//FIM CONTROLE DOS MOTORES

#ifdef SETSERIAL
	if (currentTime - previousTimeSerial >= 500) {
		Serial.print(currentTime);
		Serial.print("--> ");
		Serial.println(strSerial);
		strSerial = "";
		previousTimeSerial = currentTime;
	}
#endif
}

void go(int angle, int speed) {

	speed = map(speed, 0, 7, 0, 100);

	double vx = sin((angle * PI) / 180.0) * speed;
	double vy = cos((angle * PI) / 180.0) * speed;

	if (angle >= 0 && angle <= 90) {
		motor_speeds[0] = speed;
		motor_speeds[1] = speed - round(vy);
		motor_speeds[2] = motor_speeds[0];
		motor_speeds[3] = motor_speeds[1];
	} else if (angle > 90 && angle <= 180) {
		motor_speeds[0] = speed + round(vy);
		motor_speeds[1] = speed;
		motor_speeds[2] = motor_speeds[0];
		motor_speeds[3] = motor_speeds[1];
	} else if (angle > 180 && angle <= 270) {
		motor_speeds[0] = -speed - round(vy);
		motor_speeds[1] = -speed;
		motor_speeds[2] = motor_speeds[0];
		motor_speeds[3] = motor_speeds[1];
	} else if (angle > 270 && angle < 360) {
		motor_speeds[0] = -speed;
		motor_speeds[1] = -speed + round(vy);
		motor_speeds[2] = motor_speeds[0];
		motor_speeds[3] = motor_speeds[1];
	}

#ifdef SETSERIAL
	Serial.printf(
			"Angulo.: %d / Speed.: %d - / M1.: %d / M2.: %d / M3.: %d / M4.: %d",
			angle, speed, motor_speeds[0], motor_speeds[1], motor_speeds[2],
			motor_speeds[3]);
	Serial.println();
#endif
	Wire.beginTransmission(ROVERC_I2C_Address);
	short address = 0x00;
	Wire.write(address);
	Wire.write(motor_speeds[0]);
	Wire.write(motor_speeds[1]);
	Wire.write(motor_speeds[2]);
	Wire.write(motor_speeds[3]);
	Wire.endTransmission();
}

void garra(int angulo) {

	if (angulo >= MAX_GARRA) {
		angulo = MAX_GARRA;
		strSerial = "Garra FECHADA";
		garraAberta = true;
	}

	if (angulo <= MIN_GARRA) {
		angulo = MIN_GARRA;
		strSerial = "Garra ABRETA";
		garraAberta = false;
	}

	garraAngulo = angulo;

	//Enviando angulo para garra
	Wire.beginTransmission(ROVERC_I2C_Address);
	short address = 0x10;
	Wire.write(address);
	Wire.write(angulo);
	Wire.endTransmission();
}

//==============================================================
// speed_ramp is limiting the acceleration to a maximum of
// 5 steps per call.
// higher accelerations make the rover behaves "jumpy"
int8_t speed_ramp(int8_t actual_speed, int8_t target_speed, int8_t freio) {
	// acceleration not larger than 5 can be applied directly
	// so init calc_speed with target_speed and check if the
	// acceleration is higher than 5
	int8_t calc_speed = target_speed;
	// acceleration larger than 5?
	if (abs(target_speed - actual_speed) > freio) {
		if (target_speed > actual_speed)
			calc_speed = actual_speed + freio;
		else
			calc_speed = actual_speed - freio;
	}
	// ensure that the result is between -100 and +100
	calc_speed = (calc_speed > 100) ? 100 : calc_speed;
	calc_speed = (calc_speed < -100) ? -100 : calc_speed;
	return calc_speed;
}

void rover_set_motors(int8_t M1, int8_t M2, int8_t M3, int8_t M4,
		int8_t freio) {
	motor_speeds[0] = speed_ramp(motor_speeds[0], M1, freio);
	motor_speeds[1] = speed_ramp(motor_speeds[1], M2, freio);
	motor_speeds[2] = speed_ramp(motor_speeds[2], M3, freio);
	motor_speeds[3] = speed_ramp(motor_speeds[3], M4, freio);
	//Serial.print(motor_speeds[0]);Serial.print(" ");Serial.print(motor_speeds[1]);Serial.print(" ");
	//Serial.print(motor_speeds[2]);Serial.print(" ");Serial.println(motor_speeds[3]);
	// send the Motor speed to the Rover board via I2C
	Wire.beginTransmission(ROVERC_I2C_Address);
	short address = 0x00;
	Wire.write(address);
	Wire.write(motor_speeds[0]);
	Wire.write(motor_speeds[1]);
	Wire.write(motor_speeds[2]);
	Wire.write(motor_speeds[3]);
	Wire.endTransmission();
}

//==============================================================
// stop the rover under consideration of the maximum acceleration.
// you have to call this function once. The function returns, after
// the rover stops (motor values == 0)
void rover_stop() {
	while (motor_speeds[0] != 0 || motor_speeds[1] != 0 || motor_speeds[2] != 0
			|| motor_speeds[3] != 0) {
		rover_set_motors(0, 0, 0, 0, 5);
		delay(20);
	}
}

//==============================================================
// calculate the speed for the four motors to let the rover
// move into a specific direction (angle) at a given speed
// angle is:
//    0deg = forward
//   90deg = right
//  180deg = backwards
//   45deg = diagonal right forward
// speed is:
//    +100 = maximum speed forward
//    -100 = maximum speed backwards
// NOTE:
// this function returns after the desired speed is reached
// Due to the acceleration and deceleration, the process may
// take different amounts of time for different calls.
void move_rover(double angle, int speed) {
	// ensure that speed is between -100 and 100
	speed = (speed > 100) ? 100 : speed;
	speed = (speed < -100) ? -100 : speed;
	double vx = sin((angle * PI) / 180.0) * speed;
	double vy = cos((angle * PI) / 180.0) * speed;
	// calculate the target speed for each motor
	int8_t M1 = (int8_t) round(vy + vx);
	int8_t M2 = (int8_t) round(vy - vx);
	int8_t M3 = (int8_t) round(vy - vx);
	int8_t M4 = (int8_t) round(vy + vx);
	// repeat until desired speed is reached
	while (motor_speeds[0] != M1 || motor_speeds[1] != M2
			|| motor_speeds[2] != M3 || motor_speeds[3] != M4) {
		rover_set_motors(M1, M2, M3, M4, 5);
		delay(20);
	}
}

bool temAlgumComandoDeMotorPresionado() {
	return GamePad.isSquarePressed() || GamePad.isCirclePressed();
}
