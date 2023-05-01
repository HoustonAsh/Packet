#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <FastCRC.h>


#if PACKET_STACK_SIZE == 0
#define BUFFER_INIT_SIZE 32
#endif
#define HEAD1 0xAB
#define HEAD2 0xCD

#define FOOT1 0xDC
#define FOOT2 0xBA

class Packet {
private:
#if PACKET_STACK_SIZE == 0
  uint32_t cap = BUFFER_INIT_SIZE;
#else
  uint32_t cap;
#endif
protected:
  FastCRC16 CRC16;
  uint16_t len;

#if PACKET_STACK_SIZE == 0
  uint8_t* data;
#else
  uint8_t data[PACKET_STACK_SIZE];
#endif

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