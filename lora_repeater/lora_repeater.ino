#include <SPI.h>              // include libraries
#include <LoRa.h>

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6

const int maxPacketSize = 256;

void setup()
{
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  LoRa.setTxPower(2);

  Serial.begin(115200);
  delay(1000);

  if (!LoRa.begin(BAND)) {
    Serial.println("failure");
    while (1);
  }

  // timeout input after 200ms
  Serial.setTimeout(50);
}

int sendPacket(const byte packet[], int n) 
{  
  if(!LoRa.beginPacket()) {
    return 0;
  }
  
  int bytes_written = LoRa.write(packet, n);
  if (bytes_written != n) {
    return 0;
  }
  
  if(!LoRa.endPacket()) {
    return 0;
  }

  return bytes_written;
}

String receivePacket()
{
  int packetSize = LoRa.parsePacket();
  String packet = "";
  
  if (packetSize == 0) { return ""; }

  while (LoRa.available()) {
    char receivedText = (char)LoRa.read();
    packet += String(receivedText);
  }

  return packet;
}

void loop()
{
  String packet = receivePacket();
  if (packet == "") {
    return;
  }

  packet.setCharAt(packet.length()-1, '1');

  Serial.println("Forwarding " + packet);

  byte buf[packet.length()+1];
  packet.getBytes(buf, packet.length()+1);

  int sent = sendPacket(buf, packet.length()+1);
  if (!sent) {
    Serial.println("failed");
  }
}

