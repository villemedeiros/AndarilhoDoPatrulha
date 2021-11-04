// Do not remove the include below
#include "PatrulhaTank_ESP8266.h"

// Replace with your network credentials
//const char *ssid = "PATRULHAEUREKA.ORG";
//const char *password = "eureka#harbatolx";

// Replace with your network credentials
//const char *ssid = "Patrulha Patrulheiros";
//const char *password = "eureka#nos";

const char *ssid = "PatrulhaTANK";
const char *password = "33333333";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String getValue(String data, char separator, int index) {
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
	AwsFrameInfo *info = (AwsFrameInfo*) arg;
	if (info->final && info->index == 0 && info->len == len
			&& info->opcode == WS_TEXT) {
		data[len] = 0;

		String protocolo = (char*) data;

		switch (protocolo[0]) {

		case 'l':
			ledState = !ledState;
			Serial.print("Led.: ");
			Serial.println(ledState);
			break;

		case 'g':
			String strAux = protocolo.substring(1);

			String strAngulo = getValue(strAux, '/', 0);
			String strForca = getValue(strAux, '/', 1);

			unsigned int angulo = strAngulo.toInt();
			unsigned int forca = strForca.toInt();

			Serial.print("Angulo/Força.: ");
			Serial.print(angulo);
			Serial.print("º");

			Serial.print("/");
			Serial.print(map(forca, 0, 255, 0, 100));
			Serial.println("%");

			analogWrite(D5, forca);
			analogWrite(D7, forca);

			int speed = map(forca, 0, 255, 0, 255);

			double vx = sin((angulo * PI) / 180.0) * speed;
			double vy = cos((angulo * PI) / 180.0) * speed;

			if (angulo >= 0 && angulo <= 90) {
				analogWrite(D5, speed - round(vy));
				analogWrite(D7, speed);

				digitalWrite(D0, LOW);
				digitalWrite(D1, HIGH);
				digitalWrite(D2, LOW);
				digitalWrite(D3, HIGH);

			} else if (angulo > 90 && angulo <= 180) {

				analogWrite(D5, speed);
				analogWrite(D7, speed + round(vy));

				digitalWrite(D0, LOW);
				digitalWrite(D1, HIGH);
				digitalWrite(D2, LOW);
				digitalWrite(D3, HIGH);

			} else if (angulo > 180 && angulo <= 270) {

				analogWrite(D5, speed);
				analogWrite(D7, speed + round(vy));

				digitalWrite(D0, HIGH);
				digitalWrite(D1, LOW);
				digitalWrite(D2, HIGH);
				digitalWrite(D3, LOW);

			} else if (angulo > 270 && angulo < 360) {

				analogWrite(D5, speed - round(vy));
				analogWrite(D7, speed);

				digitalWrite(D0, HIGH);
				digitalWrite(D1, LOW);
				digitalWrite(D2, HIGH);
				digitalWrite(D3, LOW);

			}
			break;
		}
	}
}

void setup() {
// Serial port for debugging purposes
	Serial.begin(115200);

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);

	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);

	pinMode(D7, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);

	digitalWrite(D6, HIGH);    //stan

//  CONECTANDO EM UMA REDE FIXA
//	WiFi.begin(ssid, password);
//	while (WiFi.status() != WL_CONNECTED) {
//		delay(1000);
//		Serial.println("Connecting to WiFi..");
//	}
//
//	Serial.println(patrulhalogo());
//
//// Print ESP Local IP Address
//	Serial.println(ssid);
//	Serial.print("Servidor on.: ");
//	Serial.println(WiFi.localIP());
	//---------------------------------------------------

//	CONECTANDO EM UMA CONFIGURAÇÃO WEBMANAGER
//	Serial.println("\nBuscando Rede Wifi para o PatrulhaTANK");
////	Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION);
//
//	ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer,
//			"PatrulhaTANK");
////	ESPAsync_wifiManager.resetSettings();   //reset saved settings
//	//ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192,168,186,1), IPAddress(192,168,186,1), IPAddress(255,255,255,0));
//	ESPAsync_wifiManager.autoConnect("PatrulhaTANK");
//	if (WiFi.status() == WL_CONNECTED) {
//
//		Serial.print(patrulhalogo());
//
//		Serial.print("\nSSID.....: ");
//		Serial.println(WiFi.SSID());
//		Serial.print(F("Local IP.: "));
//		Serial.println(WiFi.localIP());
//	} else {
//		Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status()));
//	}
	//----------------------------------------------

	//CONECTANDO EM UMA AP
	uint32_t chipid = ESP.getChipId();

	//ESP.getEfuseMac();
	String str = ssid + String("_") + String((uint32_t) (chipid), HEX);
	Serial.println();
	Serial.print(patrulhalogo());
	Serial.print("\nAP NET..: ");
	Serial.println(str);

	//Set device in STA mode to begin with
	WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1),
			IPAddress(255, 255, 255, 0));

	WiFi.softAP(str.c_str(), password);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP...: ");
	Serial.println(myIP);
	//------------------------------------------------------------

	initWebSocket();

// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html","Vivo!");
	});

// Start server
	server.begin();
}

void loop() {
	ws.cleanupClients();
	digitalWrite(ledPin, ledState);
}

void initWebSocket() {
	ws.onEvent(onEvent);
	server.addHandler(&ws);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
		AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
	case WS_EVT_CONNECT:
		Serial.printf("Cliente Conectado #%u IP %s\n", client->id(),
				client->remoteIP().toString().c_str());
		break;
	case WS_EVT_DISCONNECT:
		Serial.printf("Cliente #%u Desconectado\n", client->id());
		ws.closeAll(0, "desconectado");
		break;
	case WS_EVT_DATA:
		handleWebSocketMessage(arg, data, len);
		break;
	case WS_EVT_PONG:
	case WS_EVT_ERROR:
		break;
	}
}
