#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define TEMPERATURE_INDOOR 5
 
const int ledPin = 9;
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x7D, 0x54 };
byte ip[] = { 10, 150, 41, 3 }; 
byte subnet[] = { 255, 255, 255, 0 };
byte gateway[] = { 10, 150, 41, 254 };
  
OneWire oneWireIndoor(TEMPERATURE_INDOOR);
DallasTemperature sensorIndoor(&oneWireIndoor);  
 
EthernetServer server(80);
 
void setup() {

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	sensorIndoor.begin();  

	Ethernet.begin(mac, ip, gateway, subnet);
}
 
void loop() {

	float temperatureIndoor;

	EthernetClient client = server.available();

	if (client) {

		boolean currentLineIsBlank = true;
		
		while (client.connected()) {
		
			if (client.available()) {
			
				char c = client.read();
				if (c == '\n' && currentLineIsBlank) {
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Server: Arduino");
					client.println("Connnection: close");
					client.println();
					temperatureIndoor = readTemperatureCelsius(sensorIndoor);
					client.print("myCB({'temperature':");
					client.print(temperatureIndoor);
					client.print(",'humidity':");
					client.print(temperatureIndoor);
					client.print("})");
					client.println();
					break;
				}
				
				if (c == '\n') currentLineIsBlank = true;
				else if (c != '\r') currentLineIsBlank = false;
			}
		}
		
		delay(1);
		client.stop();
		blinkLED();
	}

}

float readTemperatureCelsius(DallasTemperature sensor) {

	sensor.requestTemperatures();
	float temperature = sensor.getTempFByIndex(0); 
	return temperature;
}

void blinkLED() {

	digitalWrite(ledPin, HIGH);
	delay(500);
	digitalWrite(ledPin, LOW);
	delay(500);
	return;
}