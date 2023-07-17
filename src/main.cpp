#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#include "config.h"
#include "cert.h"

DynamicJsonDocument doc(1024);
String payload = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"Du bist eine KI die sich im Comodore 64 Computer befindet. Der Benutzer wird dir Fragen über den Comodore 64 stellen. Du sollst dem Benutzer helfen, einfache Programme in Basic zu schreiben, die im Comodore 64 laufen können, aber auch kurze Antworten zum Comodore 64 geben. Brich nicht den Charakter.\"},{\"role\":\"user\",\"content\":\"%s\"}],\"temperature\":0.7}";

String serialInput;
HTTPClient httpClient;

void readSerialInput(String *inputString);
String getOpenAiAnswer(HTTPClient *httpClient, String *inputString);

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

	X509List cert(IRG_Root_X1);

	WiFiClientSecure client;

	client.setTrustAnchors(&cert);

	httpClient.begin(client, "https://api.openai.com/v1/chat/completions");
	httpClient.addHeader("Content-Type", "application/json");
	httpClient.addHeader("Authorization", OPENAI_TOKEN);
}

void loop()
{
	readSerialInput(&serialInput);

	if (true == serialInput.endsWith(String('\n')))
	{
		serialInput.trim();
		Serial.println(getOpenAiAnswer(&httpClient, &serialInput));
		serialInput.clear();
	}
}

void readSerialInput(String *inputString)
{
	while (Serial.available() > 0)
	{
		char charReceived = Serial.read();
		inputString->concat(charReceived);
	}
}

String getOpenAiAnswer(HTTPClient *httpClient, String *inputString)
{
	String responseMessage;

	int httpCode = httpClient->POST(payload);

	if (httpCode > 0)
	{
#ifdef ESP_DEBUG
		Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
#endif

		if (httpCode == HTTP_CODE_OK)
		{
			String response = httpClient->getString();
			// String response = httpClient->getString(1024);  // optionally pre-reserve string to avoid reallocations in chunk mode
			deserializeJson(doc, response);
			responseMessage = doc["choices"][0]["message"]["content"].as<String>();
#ifdef ESP_DEBUG
			Serial.println(responseMessage);
#endif
		}
		else
		{
#ifdef ESP_DEBUG
			Serial.printf("[HTTPS] POST... failed, error: %s\n", httpClient->errorToString(httpCode).c_str());
#endif
		}
		httpClient->end();
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.printf("[HTTPS] Unable to connect\n");
#endif
	}

	return responseMessage;
}
