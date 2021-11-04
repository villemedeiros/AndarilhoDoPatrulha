// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _PatrulhaTank_ESP8266_H_
#define _PatrulhaTank_ESP8266_H_
#include "Arduino.h"
#include <ESP.h>
//add your includes for the project PatrulhaTank_ESP8266 here


#if !( defined(ESP8266) )
  #error This code is intended to run on ESP8266 platform! Please check your Tools->Board setting.
#endif
#include <ESP8266WiFi.h>
//#include <ESPAsync_WiFiManager.h>              //https://github.com/khoih-prog/ESPAsync_WiFiManager
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <String.h>

//AsyncWebServer webServer(80);
//DNSServer dnsServer;
//add your function definitions for the project PatrulhaTank_ESP8266_testes here

String patrulhalogo(void) {
	String patrulhaLogo = "\n         /\\ \n";
	patrulhaLogo += "        /  \\ \n";
	patrulhaLogo += "       /____\\ \n";
	patrulhaLogo += "      /......\\ \n";
	patrulhaLogo += "     /........\\ \n";
	patrulhaLogo += "    /..../\\....\\ \n";
	patrulhaLogo += "   /\\.../__\\.../\\ \n";
	patrulhaLogo += "  /  \\......../  \\ \n";
	patrulhaLogo += " /____\\______/____\\ \n";
	patrulhaLogo += " PatrulhaEUREKA.org\n";

	return patrulhaLogo;
}

void initWebSocket();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
		AwsEventType type, void *arg, uint8_t *data, size_t len);
//Do not add code below this line
#endif /* _PatrulhaTank_ESP8266_ */
