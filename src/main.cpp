#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#include "config.h"
#include "cert.h"

DynamicJsonDocument doc(1024);
String payload = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"Du bist ein Comodore 64 Computer. Der Benutzer wird dir Fragen über den Comodore 64 stellen. Du sollst dem Benutzer helfen, einfache Programme in Basic zu schreiben, die im Comodore 64 laufen können, aber auch kurze Antworten zum Comodore 64 geben. Brich nicht den Charakter.\"},{\"role\":\"user\",\"content\":\"%s\"}],\"temperature\":0.7}";

String serialInput;
WiFiClientSecure client;

void slowPrint(String *output);
String getOpenAiAnswer(String *inputString);

void setup()
{
	Serial.begin(1200);
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

#ifndef ESP_DEBUG
	slowPrint(new String("WIFI CONNECTED"));
	Serial.print('\r');
	Serial.print('\n');
#endif

#ifdef ESP_DEBUG
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
#endif

	// X509List cert(IRG_Root_X1);

	// client.setTrustAnchors(&cert);
	// Ignore SSL certificate validation
	client.setInsecure();
}

void loop()
{
	if (Serial.available() > 0)
	{
		char charReceived = Serial.read();

		if (charReceived == '\r' || charReceived == '\n')
		{
			Serial.print('\r');
			Serial.print('\n');
			Serial.print('\r');
			Serial.print('\n');

			String output = getOpenAiAnswer(&serialInput);

			output.replace("ä", "ae");
			output.replace("ö", "oe");
			output.replace("ü", "ue");
			output.replace("ß", "ss");
			output.toUpperCase();

			slowPrint(&output);
			Serial.print('\r');
			Serial.print('\n');
			Serial.print('\r');
			Serial.print('\n');

			serialInput.clear();
		}
		else if (20 == charReceived)
		{
			serialInput.remove(serialInput.length() - 1, 1);
			Serial.write(charReceived);
		}
		else if (isAscii(charReceived))
		{
			serialInput.concat(charReceived);
			Serial.write(charReceived);
		}
	}
}

void slowPrint(String *output)
{
	for (unsigned int i = 0; i < output->length(); i++)
	{
		Serial.print(output->charAt(i));
		delay(10);
	}
}

String getOpenAiAnswer(String *inputString)
{
	String responseMessage;
	HTTPClient httpClient;
	httpClient.begin(client, "https://api.openai.com/v1/chat/completions");
	httpClient.addHeader("Content-Type", "application/json");
	httpClient.addHeader("Authorization", OPENAI_TOKEN);
	String preparedPayload = payload;
	preparedPayload.replace("%s", *inputString);
#ifdef ESP_DEBUG
	Serial.println(preparedPayload);
#endif
	int httpCode = httpClient.POST(preparedPayload);

	if (httpCode > 0)
	{
#ifdef ESP_DEBUG
		Serial.printf("[HTTPS] POST... code: %d\r\n", httpCode);
#endif

		if (httpCode == HTTP_CODE_OK)
		{
			String response = httpClient.getString();
			// String response = httpClient->getString(1024);  // optionally pre-reserve string to avoid reallocations in chunk mode
			deserializeJson(doc, response);
			responseMessage = doc["choices"][0]["message"]["content"].as<String>();
		}
		else
		{
#ifdef ESP_DEBUG
			Serial.printf("[HTTPS] POST... failed, error: %s\r\n", httpClient.errorToString(httpCode).c_str());
#endif
		}
		httpClient.end();
	}
	else
	{
#ifdef ESP_DEBUG
		Serial.printf("[HTTPS] Unable to connect\r\n");
#endif
	}

	return responseMessage;
}
