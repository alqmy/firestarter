#include <SPI.h>
#include <LoRa.h>

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6

const int maxPacketSize = 256;
const String alphaNum = "asdfghjklqwertyuiopzxcvbnmASDFGHJKLQWERTYUIOPZXCVBNM1234567890";

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

int sendPacket(const char data[], int n, bool repeated) 
{ 
  int bytes_written = 0;
  String id;

  for (int i = 0; i < 7; i++) {
    id += alphaNum.charAt(random(alphaNum.length())-1);
  }


  String packet = id + ";" + String(n) + ";" + String(data) + ";" + String(repeated);

  if(!LoRa.beginPacket()) {
    return 0;
  }

  byte buff[packet.length()+1];
  packet.getBytes(buff, packet.length()+1);
  bytes_written = LoRa.write(buff, packet.length()+1);
  if (!bytes_written) {
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

void receive() {
  String received = receivePacket();
  if (received.length() <= 0) { return; }
  
  Serial.println("r");
  Serial.println(String(received.length()));
  Serial.println(received);
}

void send() {
  if (!Serial.available()) { return; }
  
  char buf[maxPacketSize+1];
  size_t n = Serial.readBytesUntil('\n', buf, maxPacketSize);
  buf[n] = '\0';
  Serial.println("w");
  int wrote = sendPacket(buf, n, false);
  if (!wrote) {
    Serial.println("0");
  }

  Serial.println(String(wrote));
}

void loop()
{
  receive();
  send();
}

