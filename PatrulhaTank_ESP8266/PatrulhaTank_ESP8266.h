// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _PatrulhaTank_ESP8266_H_
#define _PatrulhaTank_ESP8266_H_
#include "Arduino.h"
//add your includes for the project PatrulhaTank_ESP8266 here



//end of add your includes here

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <String.h>
//add your function definitions for the project PatrulhaTank_ESP8266_testes here

String patrulhalogo(void) {
	String patrulhaLogo = "         /\\ \n";
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


//Do not add code below this line
#endif /* _PatrulhaTank_ESP8266_ */
