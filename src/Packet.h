#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <FastCRC.h>

#define BUFFER_INIT_SIZE 32
#define HEAD1 0xAB
#define HEAD2 0xCD

#define FOOT1 0xDC
#define FOOT2 0xBA

class Packet {
private:
  uint32_t cap;
protected:
  FastCRC16 CRC16;
  uint16_t len;
  uint8_t* data;

public:
  Packet();
  Packet(uint8_t* payload, uint16_t buffSize);
  ~Packet();
  uint16_t size();

  uint8_t* operator[] (int i);
  Packet operator= (Packet a);
  void fixCRC();
  void insertPacket(uint8_t* buf, uint16_t bufferSize);
  bool checkCRC();
  bool isValid();
  void clear();

  void print();

  uint8_t* getData();
};

#endif