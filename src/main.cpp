#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "config.h"

// put function declarations here:
int myFunction(int, int);

void setup()
{
#ifdef ESP_DEBUG
	Serial.begin(9600);
#endif

	IPAddress ip;

#ifdef IP_ADDRESS
	if (ip.fromString(IP_ADDRESS))
	{
#ifdef ESP_DEBUG
		Serial.println(IP_ADDRESS);
#endif
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.print("UnParsable IP");
		Serial.println(IP_ADDRESS);
#endif
	}

	IPAddress dns;

	if (ip.fromString(DNS))
	{
#ifdef ESP_DEBUG
		Serial.println(DNS);
#endif
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.print("UnParsable IP");
		Serial.println(DNS);
#endif
	}

	IPAddress gateway;

	if (ip.fromString(GATEWAY))
	{
#ifdef ESP_DEBUG
		Serial.println(GATEWAY);
#endif
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.print("UnParsable IP");
		Serial.println(GATEWAY);
#endif
	}

	IPAddress subnet;

	if (ip.fromString(SUBNET))
	{
#ifdef ESP_DEBUG
		Serial.println(SUBNET);
#endif
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.print("UnParsable IP");
		Serial.println(SUBNET);
#endif
	}

	// Configures static IP address
	if (!WiFi.config(ip, dns, gateway, subnet))
	{
#ifdef ESP_DEBUG
		Serial.println("STA Failed to configure");
#endif
	}

#endif

#ifdef ESP_DEBUG
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);
#endif

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
#ifdef ESP_DEBUG
		Serial.print(".");
#endif
	}

#ifdef ESP_DEBUG
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
#endif
}

void loop()
{
	// put your main code here, to run repeatedly:
}
