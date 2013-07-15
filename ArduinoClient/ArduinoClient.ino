#include <EtherCard.h>
#include "DHT.h"

#define DHTPIN   3
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);

static byte mymac[] = {0xDD,0xDD,0xDD,0x00,0x00,0x01};
byte Ethernet::buffer[700];

void setup () {
 
  Serial.begin(57600);
  Serial.println("AmbientMonitor client");
  Serial.println();
  
  dht.begin();
  Serial.println("DHT initialized");
  Serial.println();
 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 10)) {
    Serial.println( "Failed to access Ethernet controller");
    while(1); 
  } else Serial.println("Ethernet controller initialized");
  Serial.println();
 
  if (!ether.dhcpSetup()) {
    Serial.println("Failed to get configuration from DHCP");
    while(1);
  } else Serial.println("DHCP configuration done"); 
 
  Serial.println();
  ether.printIp("IP Address:\t", ether.myip);
  ether.printIp("Netmask:\t", ether.mymask);
  ether.printIp("Gateway:\t", ether.gwip);
  Serial.println();
}

void loop() {

  word pos = ether.packetLoop(ether.packetReceive());

  if(pos) {
        
    Serial.println("Got a request...");
    
    float h = dht.readHumidity();
    char h_string[7];
    dtostrf(h, 4, 2, h_string);
    
    float t = dht.readTemperature();
    char t_string[7];
    dtostrf(t, 4, 2, t_string);
    
    BufferFiller bfill = ether.tcpOffset();
    bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
      "Content-Type: text/html\r\n\r\n"
      "myCB({'temperature':$S,'humidity':$S})"), 
      t_string, h_string);

    ether.httpServerReply(bfill.position());

    Serial.print("... sent the following values: T=");
    Serial.print(t);
    Serial.print(", H=");
    Serial.println(h);
  }    
}

